#include <utility>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "dashboard.h"
#include "electronics.h"

namespace {
constexpr char VERSION[] = "0.3.0";
constexpr uint8_t MAX_NETWORKS = 32;
constexpr size_t SCAN_JSON_LIMIT = 8192;
#ifndef PHANTOM_EXPERIMENTAL_STATION
#define PHANTOM_EXPERIMENTAL_STATION 0
#endif
constexpr unsigned long SCAN_COOLDOWN = 15000;
const IPAddress AP_IP(192,168,4,1);
ESP8266WebServer server(AP_IP,80);
String apName, apPassword;
bool fsReady = false;
bool scanning = false;
int completedScanCount = -1;
bool hasResult = false;
bool scanFailed = false;
unsigned long scanStarted = 0;
unsigned long scanFinished = 0;
int totalFound = 0;
uint32_t scanCount = 0, scanErrors = 0, lastScanMs = 0;
uint32_t minHeap = UINT32_MAX, maxHeap = 0;
uint32_t channelCounts[14] = {};
unsigned long lastHeapSample = 0;
constexpr unsigned long SCAN_TIMEOUT = 30000;
bool serverReady = false;
String cachedNetworks = "[]";
#if PHANTOM_EXPERIMENTAL_STATION
String pendingSsid, pendingKey;
bool joinQueued=false, joining=false;
unsigned long joinAt=0, lastDnsAt=0;
bool dnsAttempted=false;
String linkState="disconnected";
#endif

String jsonString(const String& input) {
  String out; if (!out.reserve(input.length() * 6 + 2)) return {}; out += '"';
  const char hex[] = "0123456789abcdef";
  for (unsigned int i = 0; i < input.length(); ++i) {
    unsigned char c = input[i];
    if (c == '"' || c == '\\') { out += '\\'; out += char(c); }
    else if (c < 32 || c >= 127) { // preserve valid UTF-8; escape invalid individual bytes
      if (c >= 0xc2 && c <= 0xf4) {
        unsigned int len = c < 0xe0 ? 2 : c < 0xf0 ? 3 : 4;
        bool valid = i + len <= input.length();
        for (unsigned int j = 1; valid && j < len; ++j) {
          unsigned char b = input[i+j];
          valid = (b >= 0x80 && b <= 0xbf);
          if (j == 1 && ((c == 0xe0 && b < 0xa0) || (c == 0xed && b > 0x9f) || (c == 0xf0 && b < 0x90) || (c == 0xf4 && b > 0x8f))) valid = false;
        }
        if (valid) { for (unsigned int j=0; j<len; ++j) out += input[i+j]; i += len-1; continue; }
      }
      out += F("\\u00"); out += hex[c >> 4]; out += hex[c & 15];
    } else out += char(c);
  }
  out += '"'; return out;
}
String makePassword() {
  const char alphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789";
  String result; result.reserve(16);
  for (int i = 0; i < 16; ++i) result += alphabet[ESP.random() % (sizeof(alphabet)-1)];
  return result;
}
String readPassword(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file || file.size() != 16) return {};
  String key = file.readString();
  if (key.length() != 16) return {};
  for (unsigned int i = 0; i < key.length(); ++i) {
    char c = key[i];
    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))) return {};
  }
  return key;
}
void loadPassword() {
  LittleFSConfig config;
  config.setAutoFormat(false);
  LittleFS.setConfig(config);
  fsReady = LittleFS.begin();
  if (fsReady) apPassword = readPassword("/ap.key");
  if (!apPassword.isEmpty()) return;
  apPassword = makePassword();
  if (!fsReady) return;

  File file = LittleFS.open("/ap.key.tmp", "w");
  if (!file) { fsReady = false; return; }
  const size_t written = file.print(apPassword);
  file.close();
  fsReady = written == apPassword.length() && readPassword("/ap.key.tmp") == apPassword &&
            LittleFS.rename("/ap.key.tmp", "/ap.key") && readPassword("/ap.key") == apPassword;
}
void sendJson(int code, const String& body) {
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("X-Content-Type-Options", "nosniff");
  server.send(code, "application/json", body);
}
// The server is bound to the AP address; verify destination and host again.
bool apRequest() {
  String host=server.hostHeader();
  if(server.client().localIP()!=AP_IP || (host!="192.168.4.1" && host!="192.168.4.1:80")) {
    sendJson(403,F("{\"error\":\"Use the private Phantom AP address\"}")); return false;
  }
  return true;
}
bool command(const char* name) {
  if(!apRequest()) return false;
  String origin = server.header("Origin");
  if ((!origin.isEmpty() && origin != "http://192.168.4.1" && origin != "http://192.168.4.1:80") ||
      server.header("X-Phantom-Request") != name) {
    sendJson(403,F("{\"error\":\"Request header required\"}")); return false;
  }
  return true;
}
#if PHANTOM_EXPERIMENTAL_STATION
bool connected() { return !joining && !joinQueued && linkState=="connected" && WiFi.status()==WL_CONNECTED; }
void clearJoin() {
  // SDK also holds current station credentials in RAM; persistent(false) is set at boot.
  pendingKey=""; pendingSsid="";
}
void stopLink(const char* state) {
  WiFi.disconnect(false); clearJoin(); joining=false; joinQueued=false; linkState=state;
}
bool overlappingSubnet() {
  uint32_t ip=uint32_t(WiFi.localIP()), mask=uint32_t(WiFi.subnetMask()), ap=uint32_t(AP_IP);
  uint32_t apMask=uint32_t(IPAddress(255,255,255,0));
  return mask==0 || (ip & mask)==(ap & mask) || (ip & apMask)==(ap & apMask);
}
void updateLink() {
  if(joinQueued) {
    if(millis()-joinAt<250) return;
    joinQueued=false; joining=true; joinAt=millis();
    WiFi.begin(pendingSsid.c_str(), pendingKey.length()?pendingKey.c_str():nullptr);
    clearJoin(); return;
  }
  if(joining) {
    if(WiFi.status()==WL_CONNECTED) {
      if(overlappingSubnet()) { stopLink("subnet_conflict"); return; }
      joining=false; linkState="connected";
    } else if(millis()-joinAt>=20000) stopLink("timeout");
  } else if(linkState=="connected" && WiFi.status()!=WL_CONNECTED) stopLink("lost");
}
void handleLinkStatus() {
  if(!apRequest())return;
  String s=F("{\"state\":"); s+=jsonString(linkState);
  s+=F(",\"connected\":");s+=connected()?"true":"false";
  if(connected()) {
    s+=F(",\"ssid\":");s+=jsonString(WiFi.SSID());
    s+=F(",\"ip\":");s+=jsonString(WiFi.localIP().toString());
    s+=F(",\"gateway\":");s+=jsonString(WiFi.gatewayIP().toString());
    s+=F(",\"subnet\":");s+=jsonString(WiFi.subnetMask().toString());
    s+=F(",\"dns\":");s+=jsonString(WiFi.dnsIP(0).toString());
    s+=F(",\"rssi\":");s+=WiFi.RSSI();
    s+=F(",\"channel\":");s+=WiFi.channel();
  }
  s+='}';sendJson(200,s);
}
void handleJoin() {
  if(!command("join"))return;
  if(scanning||joining||joinQueued||connected()) {sendJson(409,F("{\"error\":\"Finish scan or disconnect first\"}"));return;}
  if(ESP.getFreeHeap()<14000) {sendJson(503,F("{\"error\":\"Insufficient free memory\"}"));return;}
  String ssid=server.arg("ssid"), key=server.arg("password");
  if(ssid.length()<1||ssid.length()>32||(key.length()!=0&&(key.length()<8||key.length()>63))) {
    sendJson(400,F("{\"error\":\"SSID 1–32 bytes; password 8–63 bytes or empty for an open network\"}"));return;
  }
  for(unsigned i=0;i<ssid.length();++i) if(ssid[i]==0) {sendJson(400,F("{\"error\":\"Invalid SSID\"}"));return;}
  for(unsigned i=0;i<key.length();++i) if(key[i]<32||key[i]>126) {sendJson(400,F("{\"error\":\"Use an ASCII WPA passphrase\"}"));return;}
  pendingSsid=ssid;pendingKey=key;joinQueued=true;joinAt=millis();linkState="connecting";
  sendJson(202,F("{\"state\":\"connecting\"}"));
}
void handleLeave() {
  if(!command("leave"))return;
  if(scanning) {sendJson(409,F("{\"error\":\"Wait for scan to finish\"}"));return;}
  stopLink("disconnected");sendJson(200,F("{\"state\":\"disconnected\"}"));
}
void handleDns() {
  if(!command("dns"))return;
  if(!connected()||scanning) {sendJson(409,F("{\"error\":\"Join your network and wait for scans to finish\"}"));return;}
  if(dnsAttempted&&millis()-lastDnsAt<10000) {sendJson(429,F("{\"error\":\"Wait 10 seconds between DNS checks\"}"));return;}
  dnsAttempted=true;lastDnsAt=millis();
  IPAddress result;unsigned long start=millis();
  bool ok=WiFi.hostByName("example.com",result,1500)==1;
  String s=F("{\"hostname\":\"example.com\",\"resolved\":");s+=ok?"true":"false";
  s+=F(",\"elapsedMs\":");s+=millis()-start;
  s+=F(",\"address\":");if(ok)s+=jsonString(result.toString());else s+="null";
  s+='}';sendJson(200,s);
}
#else
void handleLinkStatus() {
  if (apRequest()) sendJson(200, F("{\"state\":\"disabled\",\"connected\":false}"));
}
void handleDisabledLink() {
  if (apRequest()) sendJson(403, F("{\"error\":\"Station tools are disabled in this build\"}"));
}
#endif

void handleCapabilities() {
  if (!apRequest()) return;
#if PHANTOM_EXPERIMENTAL_STATION
  sendJson(200, F("{\"schema\":1,\"wifiScan\":true,\"gpioRead\":true,\"experimentalStation\":true}"));
#else
  sendJson(200, F("{\"schema\":1,\"wifiScan\":true,\"gpioRead\":true,\"experimentalStation\":false}"));
#endif
}
const char* encryption(uint8_t value) {
  switch (value) {
    case ENC_TYPE_NONE: return "Open";
    case ENC_TYPE_WEP: return "WEP";
    case ENC_TYPE_TKIP: return "WPA/TKIP";
    case ENC_TYPE_CCMP: return "WPA2/CCMP";
    case ENC_TYPE_AUTO: return "WPA/WPA2";
    default: return "Unknown";
  }
}
void updateScan() {
  if (!scanning) return;
  const int n = completedScanCount;
  if (n < 0) {
    // Do not force-delete an in-progress SDK scan. Report timeout to the UI,
    // keep polling until the SDK completes, and avoid overlapping radio scans.
    if (millis()-scanStarted > SCAN_TIMEOUT && !scanFailed) { scanFailed = true; ++scanErrors; }
    return;
  }
  scanning = false; scanFinished = millis();
  lastScanMs = scanFinished-scanStarted;
  if (scanFailed) { WiFi.scanDelete(); return; }
  String result;
  if (!result.reserve(SCAN_JSON_LIMIT)) {
    WiFi.scanDelete(); scanFailed = true; ++scanErrors; return;
  }
  result += '[';
  // Cap response size; sort by signal so the strongest networks are represented.
  uint8_t indices[255]; int limit = min(n, 255);
  for (int i = 0; i < limit; ++i) indices[i] = i;
  for (int i = 0; i < min(limit, int(MAX_NETWORKS)); ++i) {
    int best = i;
    for (int j = i + 1; j < limit; ++j) if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[best])) best = j;
    uint8_t index = indices[best]; indices[best] = indices[i]; indices[i] = index;
    String entry;
    if (!entry.reserve(256)) {
      WiFi.scanDelete(); scanFailed = true; ++scanErrors; return;
    }
    entry += F("{\"ssid\":"); const String ssid = jsonString(WiFi.SSID(index).substring(0, 32));
    if (ssid.isEmpty()) {
      WiFi.scanDelete(); scanFailed = true; ++scanErrors; return;
    }
    entry += ssid;
    entry += F(",\"rssi\":"); entry += String(WiFi.RSSI(index));
    entry += F(",\"channel\":"); entry += String(WiFi.channel(index));
    entry += F(",\"security\":"); entry += '"'; entry += encryption(WiFi.encryptionType(index)); entry += '"';
    entry += '}';
    if (result.length() + entry.length() + 2 > SCAN_JSON_LIMIT) break;
    if (i) result += ',';
    result += entry;
  }
  result += ']';
  totalFound = n;
  for (int i = 0; i < limit; ++i) {
    int channel = WiFi.channel(i);
    if (channel >= 1 && channel <= 14) ++channelCounts[channel - 1];
  }
  ++scanCount;
  WiFi.scanDelete();
  cachedNetworks = std::move(result); hasResult = true; scanFailed = false;
}
void handleStatus() {
  if(!apRequest())return;
  String s; s.reserve(350);
  s += F("{\"version\":\""); s += VERSION;
  s += F("\",\"board\":\"ESP8266\",\"cpuMHz\":"); s += ESP.getCpuFreqMHz();
  s += F(",\"flashBytes\":"); s += ESP.getFlashChipRealSize();
  s += F(",\"freeHeap\":"); s += ESP.getFreeHeap();
  s += F(",\"largestFreeBlock\":"); s += ESP.getMaxFreeBlockSize();
  s += F(",\"minHeap\":"); s += minHeap == UINT32_MAX ? ESP.getFreeHeap() : minHeap;
  s += F(",\"maxHeap\":"); s += maxHeap;
  s += F(",\"scanCount\":"); s += scanCount;
  s += F(",\"scanErrors\":"); s += scanErrors;
  s += F(",\"lastScanMs\":"); s += lastScanMs;
  s += F(",\"uptimeSeconds\":"); s += millis()/1000;
  s += F(",\"clients\":"); s += WiFi.softAPgetStationNum();
  s += F(",\"ap\":"); s += jsonString(apName);
  s += F(",\"ip\":"); s += jsonString(WiFi.softAPIP().toString());
  s += F(",\"persistentKey\":"); s += fsReady ? "true" : "false";
  s += '}'; sendJson(200, s);
}
void handleScanStart() {
  // Non-simple header prevents cross-origin browser requests from starting scans.
  if (!command("scan")) return;
 #if PHANTOM_EXPERIMENTAL_STATION
  if (joining||joinQueued) { sendJson(409,F("{\"error\":\"Wait for connection attempt\"}")); return; }
 #endif
  if (scanning && scanFailed) {
    sendJson(503, F("{\"error\":\"Radio is recovering from a timeout. Restart if it does not recover\"}"));
    return;
  }
  if (scanning) { sendJson(202, F("{\"state\":\"scanning\"}")); return; }
  if ((hasResult || scanFailed) && millis()-scanStarted < SCAN_COOLDOWN) {
    sendJson(429, F("{\"error\":\"Wait 15 seconds between scans\"}")); return;
  }
  if (ESP.getFreeHeap() < 14000) { sendJson(503, F("{\"error\":\"Insufficient free memory\"}")); return; }
  scanStarted = millis(); scanFailed = false; scanning = true;
  cachedNetworks = "[]";
  hasResult = false;
  totalFound = 0;
  for (auto &count : channelCounts) count = 0;
  // scanComplete() returns int8_t in core 3.1.2. Capture the full count
  // through the callback so dense scans cannot collide with its sentinels.
  completedScanCount = -1;
  WiFi.scanNetworksAsync([](int count) { completedScanCount = count; }, false);
  if (completedScanCount < 0 && WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
    scanning = false;
    scanFailed = true;
    ++scanErrors;
    sendJson(503, F("{\"error\":\"Radio scan could not start\"}"));
    return;
  }
  sendJson(202, F("{\"state\":\"scanning\"}"));
}
void handleScanResults() {
  if(!apRequest())return;
  String s;
  if (!s.reserve(cachedNetworks.length() + 256)) {
    sendJson(503, F("{\"error\":\"Insufficient memory for scan response\"}"));
    return;
  }
  s += F("{\"state\":\""); s += scanFailed ? "error" : scanning ? "scanning" : hasResult ? "ready" : "idle";
  s += F("\",\"total\":"); s += totalFound;
  s += F(",\"ageSeconds\":"); if (hasResult && !scanning && !scanFailed) s += (millis()-scanFinished)/1000; else s += "null";
  s += F(",\"channelCounts\":[");
  for (int i=0;i<14;++i) { if(i) s+=','; s+=channelCounts[i]; }
  s += F("],\"counted\":"); s += min(totalFound,255);
  s += F(",\"networks\":"); s += cachedNetworks; s += '}';
  sendJson(200, s);
}
}
void setup() {
  Serial.begin(115200); delay(100);
  WiFi.persistent(false); WiFi.mode(WIFI_AP_STA); WiFi.setAutoReconnect(false); WiFi.disconnect(false);
  apName = "Phantom-" + String(ESP.getChipId(), HEX);
  loadPassword();
  bool ok = WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255,255,255,0)) && WiFi.softAP(apName.c_str(), apPassword.c_str(), 1, false, 2);
  Serial.println(); Serial.println(F("Phantom OS 0.3.0"));
  if (!ok) { Serial.println(F("AP start failed; check board and power.")); return; }
  Serial.print(F("Wi-Fi: ")); Serial.println(apName);
  Serial.print(F("Password: ")); Serial.println(apPassword);
  Serial.print(F("Dashboard: http://")); Serial.println(WiFi.softAPIP());
  if (!fsReady) Serial.println(F("Key storage unavailable; password changes on restart. See README."));
  server.collectHeaders("X-Phantom-Request", "Origin");
  server.on("/", HTTP_GET, []() {
    if(!apRequest())return;
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("X-Content-Type-Options", "nosniff");
    server.sendHeader("X-Frame-Options", "DENY");
    server.sendHeader("Content-Security-Policy", "default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; base-uri 'none'; frame-ancestors 'none'");
    server.send_P(200, "text/html; charset=utf-8", DASHBOARD);
  });
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/scan", HTTP_POST, handleScanStart);
  server.on("/api/scan", HTTP_GET, handleScanResults);
  server.on("/api/capabilities", HTTP_GET, handleCapabilities);
  server.on("/api/electronics/status", HTTP_GET, []() {
    if (!apRequest()) return;
    const String inputs = electronics::readInputs();
    if (inputs.isEmpty()) {
      sendJson(503, F("{\"error\":\"Insufficient memory for GPIO response\"}"));
      return;
    }
    sendJson(200, inputs);
  });
  server.on("/api/link", HTTP_GET, handleLinkStatus);
#if PHANTOM_EXPERIMENTAL_STATION
  server.on("/api/link/join", HTTP_POST, handleJoin);
  server.on("/api/link/leave", HTTP_POST, handleLeave);
  server.on("/api/link/dns", HTTP_POST, handleDns);
#else
  server.on("/api/link/join", HTTP_POST, handleDisabledLink);
  server.on("/api/link/leave", HTTP_POST, handleDisabledLink);
  server.on("/api/link/dns", HTTP_POST, handleDisabledLink);
#endif
  server.onNotFound([](){ if(apRequest())sendJson(404, F("{\"error\":\"Not found\"}")); });
  server.begin(); serverReady = true;
  minHeap = maxHeap = ESP.getFreeHeap();
}
void loop() {
  if (serverReady) {
    server.handleClient();
    updateScan();
#if PHANTOM_EXPERIMENTAL_STATION
    updateLink();
#endif
  }
  if (millis()-lastHeapSample >= 1000) {
    lastHeapSample = millis(); uint32_t h = ESP.getFreeHeap();
    if (h < minHeap) minHeap = h; if (h > maxHeap) maxHeap = h;
  }
  delay(2);
}
