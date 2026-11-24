#include <utility>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "dashboard.h"

namespace {
constexpr char VERSION[] = "0.2.0";
constexpr uint8_t MAX_NETWORKS = 32;
constexpr unsigned long SCAN_COOLDOWN = 15000;
ESP8266WebServer server(80);
String apName, apPassword;
bool fsReady = false;
bool scanning = false;
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

String jsonString(const String& input) {
  String out; out.reserve(input.length() + 10); out += '"';
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
void loadPassword() {
  // Mount a provisioned filesystem; never autoformat an existing one.
  LittleFSConfig config; config.setAutoFormat(false); LittleFS.setConfig(config);
  fsReady = LittleFS.begin();
  if (fsReady) {
    File f = LittleFS.open("/ap.key", "r");
    if (f && f.size() == 16) apPassword = f.readString();
    if (f) f.close();
  }
  bool valid = apPassword.length() == 16;
  for (unsigned int i = 0; valid && i < apPassword.length(); ++i) {
    char c = apPassword[i]; valid = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
  }
  if (!valid) {
    apPassword = makePassword();
    if (fsReady) {
      File f = LittleFS.open("/ap.key", "w");
      if (f) { if (f.print(apPassword) != apPassword.length()) fsReady = false; f.close(); }
      else fsReady = false;
    }
  }
}
void sendJson(int code, const String& body) {
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("X-Content-Type-Options", "nosniff");
  server.send(code, "application/json", body);
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
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_RUNNING) {
    // Do not force-delete an in-progress SDK scan. Report timeout to the UI,
    // keep polling until the SDK completes, and avoid overlapping radio scans.
    if (millis()-scanStarted > SCAN_TIMEOUT && !scanFailed) { scanFailed = true; ++scanErrors; }
    return;
  }
  scanning = false; scanFinished = millis();
  lastScanMs = scanFinished-scanStarted;
  if (n < 0) { if (!scanFailed) ++scanErrors; scanFailed = true; WiFi.scanDelete(); return; }
  ++scanCount;
  totalFound = n;
  for (auto &count : channelCounts) count = 0;
  for (int i=0; i<min(n,255); ++i) {
    int c=WiFi.channel(i); if(c>=1 && c<=14) ++channelCounts[c-1];
  }
  String result; result.reserve(5000); result += '[';
  // Cap response size; sort by signal so the strongest networks are represented.
  uint8_t indices[255]; int limit = min(n, 255);
  for (int i = 0; i < limit; ++i) indices[i] = i;
  for (int i = 0; i < min(limit, int(MAX_NETWORKS)); ++i) {
    int best = i;
    for (int j = i + 1; j < limit; ++j) if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[best])) best = j;
    uint8_t index = indices[best]; indices[best] = indices[i]; indices[i] = index;
    if (i) result += ',';
    result += F("{\"ssid\":"); result += jsonString(WiFi.SSID(index));
    result += F(",\"rssi\":"); result += String(WiFi.RSSI(index));
    result += F(",\"channel\":"); result += String(WiFi.channel(index));
    result += F(",\"security\":"); result += jsonString(encryption(WiFi.encryptionType(index)));
    result += '}';
  }
  result += ']'; WiFi.scanDelete();
  cachedNetworks = std::move(result); hasResult = true; scanFailed = false;
}
void handleStatus() {
  String s; s.reserve(350);
  s += F("{\"version\":\""); s += VERSION;
  s += F("\",\"board\":\"ESP8266\",\"cpuMHz\":"); s += ESP.getCpuFreqMHz();
  s += F(",\"flashBytes\":"); s += ESP.getFlashChipRealSize();
  s += F(",\"freeHeap\":"); s += ESP.getFreeHeap();
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
  if (server.header("X-Phantom-Request") != "scan") { sendJson(403, F("{\"error\":\"Request header required\"}")); return; }
  if (scanning) { sendJson(202, F("{\"state\":\"scanning\"}")); return; }
  if ((hasResult || scanFailed) && millis()-scanStarted < SCAN_COOLDOWN) {
    sendJson(429, F("{\"error\":\"Wait 15 seconds between scans\"}")); return;
  }
  if (ESP.getFreeHeap() < 14000) { sendJson(503, F("{\"error\":\"Insufficient free memory\"}")); return; }
  scanStarted = millis(); scanFailed = false; scanning = true;
  WiFi.scanDelete(); WiFi.scanNetworks(true, false);
  sendJson(202, F("{\"state\":\"scanning\"}"));
}
void handleScanResults() {
  String s; s.reserve(cachedNetworks.length() + 140);
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
  bool ok = WiFi.softAP(apName.c_str(), apPassword.c_str(), 1, false, 2);
  Serial.println(); Serial.println(F("Phantom OS 0.2.0"));
  if (!ok) { Serial.println(F("AP start failed; check board and power.")); return; }
  Serial.print(F("Wi-Fi: ")); Serial.println(apName);
  Serial.print(F("Password: ")); Serial.println(apPassword);
  Serial.print(F("Dashboard: http://")); Serial.println(WiFi.softAPIP());
  if (!fsReady) Serial.println(F("Key storage unavailable; password changes on restart. See README."));
  static const char* headers[] = {"X-Phantom-Request"}; server.collectHeaders(headers, 1);
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("X-Content-Type-Options", "nosniff");
    server.sendHeader("X-Frame-Options", "DENY");
    server.sendHeader("Content-Security-Policy", "default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; base-uri 'none'; frame-ancestors 'none'");
    server.send_P(200, "text/html; charset=utf-8", DASHBOARD);
  });
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/scan", HTTP_POST, handleScanStart);
  server.on("/api/scan", HTTP_GET, handleScanResults);
  server.onNotFound([](){ sendJson(404, F("{\"error\":\"Not found\"}")); });
  server.begin(); serverReady = true;
  minHeap = maxHeap = ESP.getFreeHeap();
}
void loop() {
  if (serverReady) { server.handleClient(); updateScan(); }
  if (millis()-lastHeapSample >= 1000) {
    lastHeapSample = millis(); uint32_t h = ESP.getFreeHeap();
    if (h < minHeap) minHeap = h; if (h > maxHeap) maxHeap = h;
  }
  delay(2);
}
