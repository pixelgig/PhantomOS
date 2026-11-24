#pragma once
#include <Arduino.h>
static const char DASHBOARD[] PROGMEM = R"PHANTOM(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><meta name="color-scheme" content="dark"><title>Phantom OS</title>
<style>
:root{--bg:#0b111a;--card:#141f2c;--line:#263747;--ink:#e8f0f7;--muted:#a2b3c4;--accent:#49e1b7}*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--ink);font:15px system-ui,sans-serif}main{max-width:840px;margin:auto;padding:22px 16px 40px}header{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:22px}h1{font-size:27px;letter-spacing:2px;margin:0;color:var(--accent)}h2{font-size:19px;margin:0 0 12px}p{line-height:1.5;margin:6px 0;color:var(--muted)}small{color:var(--muted)}.tag{font-size:11px;border:1px solid var(--line);padding:6px 9px;border-radius:16px;white-space:nowrap}.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}.tile,section{background:var(--card);border:1px solid var(--line);border-radius:14px;padding:15px}.tile span{display:block;font-size:21px;font-weight:650;color:var(--accent);margin-top:6px;overflow-wrap:anywhere}section{margin-top:14px}.row{display:flex;justify-content:space-between;align-items:center;gap:12px;flex-wrap:wrap}button{font:inherit;font-weight:650;border:0;border-radius:9px;padding:11px 16px;background:var(--accent);color:#071611;cursor:pointer}button:disabled{opacity:.45;cursor:wait}button.secondary{background:transparent;color:var(--ink);border:1px solid var(--line)}.bars{display:grid;grid-template-columns:repeat(14,minmax(0,1fr));gap:5px;align-items:end;height:106px;margin-top:12px}.barcol{height:100%;display:flex;flex-direction:column;justify-content:flex-end;text-align:center;font-size:10px;color:var(--muted);gap:5px}.bar{background:var(--accent);min-height:2px;border-radius:4px 4px 0 0}.network{padding:12px 0;border-top:1px solid var(--line);display:grid;grid-template-columns:minmax(0,1fr) auto;gap:5px 10px}.network strong{overflow-wrap:anywhere}.network span{color:var(--accent);text-align:right;font-variant-numeric:tabular-nums}.network small{grid-column:1/-1}.message{margin:12px 0;min-height:22px}footer{padding:18px 0;color:var(--muted);font-size:12px}button:focus-visible{outline:3px solid white;outline-offset:3px}@media(min-width:620px){.grid{grid-template-columns:repeat(4,minmax(0,1fr))}} 
input,select{font:inherit;background:var(--bg);color:var(--ink);border:1px solid var(--line);border-radius:8px;padding:10px;min-width:0}.controls{display:flex;gap:8px;flex-wrap:wrap;margin:12px 0}.controls input{flex:1;min-width:140px}.metrics{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px;margin:12px 0}.metrics div{background:var(--bg);padding:10px;border-radius:8px}.metrics strong{display:block;color:var(--accent);margin-top:5px}.history{max-height:220px;overflow:auto}.spark{height:64px;display:flex;align-items:end;gap:3px;margin:12px 0}.spark i{flex:1;background:var(--accent);border-radius:2px 2px 0 0;min-height:2px}.warn{color:#f9ce8d}
</style></head><body><main><header><div><h1>PHANTOM OS</h1><small>ESP8266 / LOCAL TOOLKIT</small></div><span class="tag" id="connection">Connecting</span></header>
<div class="grid"><div class="tile"><small>CPU</small><span id="cpu">—</span></div><div class="tile"><small>Flash</small><span id="flash">—</span></div><div class="tile"><small>Free heap</small><span id="heap">—</span></div><div class="tile"><small>Uptime</small><span id="uptime">—</span></div></div>
<section><div class="row"><h2>Wi-Fi explorer</h2><button id="scan">Scan nearby Wi-Fi</button></div><p>Discover 2.4 GHz access points and inspect signal, channel and advertised security. Use for your own network diagnostics. No packets or credentials are captured.</p><p class="message" id="message" role="status" aria-live="polite">Ready when connected.</p><div class="row"><small>Channel distribution</small><small id="count">No results</small></div><div class="bars" id="bars" aria-label="Network counts by Wi-Fi channel"></div><p id="hint"></p><div class="controls"><input id="filter" placeholder="Filter network name" aria-label="Filter network name"><select id="sort" aria-label="Sort networks"><option value="signal">Strongest first</option><option value="name">Name</option><option value="channel">Channel</option></select></div><div id="networks"></div><div class="controls"><button class="secondary" id="export" disabled>Export CSV</button><button class="secondary" id="exportJson" disabled>Export JSON</button><button class="secondary" id="snapshot" disabled>Save snapshot</button></div></section>
<section><div class="row"><h2>Network toolkit</h2><button class="secondary" id="linkRefresh">Refresh link</button></div><p>Optional: connect the board to a 2.4 GHz network you own or administer. Your phone stays on the Phantom access point. The board does not route traffic or save the station password to its filesystem. Use only trusted AP clients; local HTTP is not TLS.</p><div class="controls"><input id="joinSsid" maxlength="32" aria-label="Network name" placeholder="Your network SSID" autocomplete="off"><input id="joinKey" type="password" maxlength="63" aria-label="Network password" placeholder="WPA password (blank for open)" autocomplete="off"></div><div class="controls"><button id="joinLink">Connect</button><button class="secondary" id="leaveLink">Disconnect</button><button class="secondary" id="checkDns" disabled>Check DNS</button></div><p id="linkMessage" role="status">Not connected to an upstream network.</p><div class="metrics"><div><small>Station address</small><strong id="stationIp">—</strong></div><div><small>Gateway (DHCP)</small><strong id="gateway">—</strong></div><div><small>Subnet mask</small><strong id="subnet">—</strong></div><div><small>DNS server</small><strong id="dnsServer">—</strong></div><div><small>Station signal</small><strong id="stationSignal">—</strong></div><div><small>DNS result (example.com)</small><strong id="dnsResult">—</strong></div></div><p>DNS is an on-demand lookup and may use cache; it does not prove Internet access or gateway reachability. Radio channel changes can interrupt the local page. Reconnect to Phantom Wi-Fi if needed.</p></section><section><div class="row"><h2>Live device monitor</h2><button class="secondary" id="monitor">Start monitor</button></div><p>Measures local dashboard response time, not Internet speed. Monitoring pauses during Wi-Fi scans.</p><div class="metrics"><div><small>Local HTTP response</small><strong id="latency">—</strong></div><div><small>Heap range since boot</small><strong id="heapRange">—</strong></div><div><small>Completed scans</small><strong id="scans">—</strong></div><div><small>Scan errors / last duration</small><strong id="scanHealth">—</strong></div></div><small>Recent response times (relative scale)</small><div id="spark" class="spark" aria-label="Local response time history"></div><p id="monitorStatus" role="status">Monitoring is off.</p></section><section><div class="row"><h2>Saved snapshots</h2><button class="secondary" id="clear">Clear device snapshots</button></div><p>Optional, stored only in this browser (up to 10 per device). Network names can reveal nearby locations.</p><button class="secondary" id="legacyExport" hidden>Export previous shared snapshots</button><button class="secondary" id="legacyClear" hidden>Clear previous shared snapshots</button><div class="history" id="history"></div><p id="storageStatus" role="status"></p></section><section><div class="row"><h2>Device information</h2><button class="secondary" id="refresh">Refresh</button></div><p id="device">Connecting to device...</p><p id="key"></p><small>Bluetooth, NFC, IR, Sub-GHz and USB HID are not available on this hardware without extensions.</small></section><footer>PHANTOM OS v0.3.0 · No cloud · No accounts · Local data only</footer></main>
<section>
 <div class="row"><h2>GPIO input levels</h2><button class="secondary" id="readPins">Read pins</button></div>
 <p>Read-only levels for the NodeMCU profile. Unconnected pins can fluctuate. Confirm your board and wiring before connecting anything.</p>
 <p id="pinLevels" role="status" aria-live="polite"></p>
</section>
<script>
'use strict';const $=id=>document.getElementById(id);let data=[],busy=false,ready=false,lastResult=null,monitorTimer=null,statusRequest=null,samples=[];const LEGACY_STORAGE='phantom.snapshot.v1';const demo=location.protocol==='file:'||location.hash==='#demo';let storageKey=demo?'phantom.demo.v2':null,saved=[];let demoScanned=false,demoLinked=false,linkBusy=false,linkConnected=false;
async function api(path,options){if(demo){if(path==='/api/electronics/status')return {schema:1,pins:[{label:'D1',gpio:5,level:1}]};if(path==='/api/link/join'){demoLinked=true;return {state:'connecting'}}if(path==='/api/link/leave'){demoLinked=false;return {state:'disconnected'}}if(path==='/api/link/dns')return {hostname:'example.com',resolved:true,address:'192.0.2.10',elapsedMs:34};if(path==='/api/link')return demoLinked?{state:'connected',connected:true,ssid:'Example Home',ip:'192.168.1.50',gateway:'192.168.1.1',subnet:'255.255.255.0',dns:'192.168.1.1',rssi:-48,channel:6}:{state:'disconnected',connected:false};if(path==='/api/status')return {version:'0.3.0',cpuMHz:80,flashBytes:4194304,freeHeap:39200,uptimeSeconds:245,ap:'Phantom-DEMO',ip:'192.168.4.1',clients:1,persistentKey:true,minHeap:35000,maxHeap:42000,scanCount:1,scanErrors:0,lastScanMs:2100};if(options){demoScanned=true;return {state:'scanning'}}return demoScanned?{state:'ready',total:4,counted:4,channelCounts:[1,0,0,0,0,2,0,0,0,0,1,0,0,0],networks:[{ssid:'Example Home',rssi:-43,channel:1,security:'WPA2/CCMP'},{ssid:'Example Guest',rssi:-64,channel:6,security:'WPA/WPA2'},{ssid:'Example Lab',rssi:-71,channel:6,security:'WPA2/CCMP'},{ssid:'Example Open',rssi:-85,channel:11,security:'Open'}]}:{state:'idle',networks:[],total:0};}const controller=new AbortController();
const timer=setTimeout(()=>controller.abort(),4000);
try {
 const res=await fetch(path,{cache:'no-store',...options,signal:controller.signal});
 const body=await res.json();
 if(!res.ok)throw Error(body.error||'Request failed');
 return body;
} catch(error) {
 if(controller.signal.aborted)throw Error('Device request timed out');
 throw error;
} finally {clearTimeout(timer);}
}
function status(){
 if (!statusRequest) statusRequest=updateStatus().finally(()=>{statusRequest=null;});
 return statusRequest;
}
async function updateStatus(){try{let start=performance.now(),s=await api('/api/status');let ms=Math.round(performance.now()-start);$('latency').textContent=demo?'Sample only':ms+' ms';$('heapRange').textContent=s.minHeap==null?'—':(s.minHeap/1024).toFixed(1)+'–'+(s.maxHeap/1024).toFixed(1)+' KB';$('scans').textContent=s.scanCount??'—';$('scanHealth').textContent=s.scanErrors==null?'—':s.scanErrors+' / '+s.lastScanMs+' ms';if(monitorTimer&&!demo){samples.push(ms);if(samples.length>24)samples.shift();drawSpark();}$('connection').textContent=demo?'DEMO / SAMPLE DATA':'LOCAL / ONLINE';$('cpu').textContent=s.cpuMHz+' MHz';$('flash').textContent=(s.flashBytes/1048576).toFixed(1)+' MB';$('heap').textContent=(s.freeHeap/1024).toFixed(1)+' KB';$('uptime').textContent=Math.floor(s.uptimeSeconds/60)+'m '+s.uptimeSeconds%60+'s';$('device').textContent=s.ap+' · '+s.ip+' · '+s.clients+' client(s) · v'+s.version;$('key').textContent=s.persistentKey?'Access point key stored locally.':'Key is temporary; read USB serial after reboot.';if(!demo)selectSnapshotDevice(s.ap);ready=true;}catch(e){$('connection').textContent='OFFLINE';$('device').textContent='Reconnect to Phantom Wi-Fi and refresh.';ready=false;}finally{$('scan').disabled=!ready||busy;}}
function bars(items,provided){let counts=Array(14).fill(0);if(Array.isArray(provided)&&provided.length===14)counts=provided;else items.forEach(n=>{if(n.channel>=1&&n.channel<=14)counts[n.channel-1]++});let max=Math.max(1,...counts);$('bars').replaceChildren();counts.forEach((v,i)=>{let col=document.createElement('div'),bar=document.createElement('div'),label=document.createElement('span');col.className='barcol';bar.className='bar';bar.style.height=(v/max*75)+'px';bar.title=v+' networks';label.textContent=i+1;col.append(bar,label);$('bars').append(col)});}
function filtered(){let q=($('filter').value||'').toLowerCase(),sort=$('sort').value;return data.filter(n=>String(n.ssid).toLowerCase().includes(q)).sort((a,b)=>sort==='name'?a.ssid.localeCompare(b.ssid):sort==='channel'?a.channel-b.channel:b.rssi-a.rssi);}
function renderRows(){let rows=filtered();$('networks').replaceChildren();rows.forEach(n=>{let row=document.createElement('div'),title=document.createElement('strong'),signal=document.createElement('span'),details=document.createElement('small');row.className='network';title.textContent=n.ssid||'(Hidden)';signal.textContent=n.rssi+' dBm';details.textContent='Channel '+n.channel+' · '+n.security;row.append(title,signal,details);$('networks').append(row)});$('count').textContent=rows.length+' shown / '+(lastResult?.total||0)+' found';}
function render(result){lastResult=result;data=result.networks||[];bars(data,result.channelCounts);renderRows();$('export').disabled=!data.length;$('exportJson').disabled=false;$('snapshot').disabled=demo||!storageKey;$('hint').textContent='Channel counts: '+(result.counted??data.length)+' entries counted. Counts are not RF interference measurements. Export includes all cached entries, not just filtered rows.';}
$('filter').oninput=renderRows;$('sort').onchange=renderRows;
function drawSpark(){$('spark').replaceChildren();let max=Math.max(1,...samples);samples.forEach(v=>{let e=document.createElement('i');e.style.height=(v/max*100)+'%';e.title=v+' ms';$('spark').append(e)});}
$('monitor').onclick=()=>{
 if(monitorTimer){
   clearInterval(monitorTimer);
   monitorTimer=null;
   $('monitor').textContent='Start monitor';
   $('monitorStatus').textContent='Monitoring is off.';
   return;
 }
 if(demo){
   $('monitorStatus').textContent='Live monitoring requires actual hardware.';
   return;
 }
 samples=[];
 drawSpark();
 $('monitor').textContent='Stop monitor';
 $('monitorStatus').textContent='Samples every 15 seconds while this dashboard is visible. Pauses during scans and link changes.';
 monitorTimer=setInterval(()=>{
   if(!document.hidden&&!busy&&!linkBusy)status();
 },15000);
};
function download(name,text,type){let url=URL.createObjectURL(new Blob([text],{type}));let a=document.createElement('a');a.href=url;a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);}
function snapshotData(){return {schema:1,savedAt:new Date().toISOString(),total:lastResult.total,counted:lastResult.counted??data.length,channelCounts:lastResult.channelCounts||[],networks:data};}
function loadStoredSnapshots(key){
 if(!key)return [];
 try {
   const raw=localStorage.getItem(key)||'[]';
   if(raw.length>131072)return [];
   const value=JSON.parse(raw);
   if(!Array.isArray(value))return [];
   return value.slice(0,10).filter(snapshot=>
     snapshot?.schema===1&&typeof snapshot.savedAt==='string'&&snapshot.savedAt.length<=40&&
     Number.isInteger(snapshot.total)&&snapshot.total>=0&&
     Array.isArray(snapshot.networks)&&snapshot.networks.length<=32&&snapshot.networks.every(network=>
       typeof network?.ssid==='string'&&network.ssid.length<=32&&
       Number.isFinite(network.rssi)&&Number.isInteger(network.channel)&&
       typeof network.security==='string'&&network.security.length<=24
     )
   );
 } catch(error) {return [];}
}
function readSnapshots(){
 saved=loadStoredSnapshots(storageKey);
 renderHistory();
}
function selectSnapshotDevice(ap){
 if(typeof ap!=='string'||!/^Phantom-[0-9a-f]{1,8}$/i.test(ap))return;
 const key='phantom.snapshot.v2.'+ap.toLowerCase();
 if(storageKey===key)return;
 if(storageKey){
   lastResult=null;
   data=[];
   bars([]);
   renderRows();
   for(const id of ['export','exportJson','snapshot'])$(id).disabled=true;
 }
 storageKey=key;
 readSnapshots();
 $('snapshot').disabled=!lastResult;
}
function writeSnapshots(next){
 if(!storageKey)return;
 try {
   if(next.length)localStorage.setItem(storageKey,JSON.stringify(next));
   else localStorage.removeItem(storageKey);
   saved=next;
   renderHistory();
   $('storageStatus').textContent=next.length?'Saved locally.':'Device snapshots cleared.';
 } catch(error) {$('storageStatus').textContent='Browser storage unavailable or full.';}
}
function showLegacyControls(){
 const available=!demo&&loadStoredSnapshots(LEGACY_STORAGE).length>0;
 $('legacyExport').hidden=!available;
 $('legacyClear').hidden=!available;
}
$('legacyExport').onclick=()=>download(
 'phantom-shared-snapshots.json',JSON.stringify(loadStoredSnapshots(LEGACY_STORAGE),null,2),'application/json'
);
$('legacyClear').onclick=()=>{
 if(!confirm('Delete the previous shared snapshots from this browser?'))return;
 try {
   localStorage.removeItem(LEGACY_STORAGE);
   showLegacyControls();
   $('storageStatus').textContent='Previous shared snapshots cleared.';
 } catch(error) {$('storageStatus').textContent='Browser storage unavailable.';}
};
function renderHistory(){$('history').replaceChildren();if(!saved.length){$('history').textContent='No saved snapshots.';return;}saved.forEach(x=>{let r=document.createElement('div'),name=document.createElement('small'),b=document.createElement('button');r.className='row network';name.textContent=x.savedAt+' · '+x.networks.length+' entries';b.textContent='Export';b.className='secondary';b.onclick=()=>download('phantom-snapshot.json',JSON.stringify(x,null,2),'application/json');r.append(name,b);$('history').append(r)});}
$('exportJson').onclick=()=>{if(lastResult)download('phantom-scan.json',JSON.stringify(snapshotData(),null,2),'application/json');};$('snapshot').onclick=()=>{if(!demo&&lastResult)writeSnapshots([snapshotData(),...saved].slice(0,10));};$('clear').onclick=()=>{if(saved.length&&confirm('Delete locally saved snapshots for this device?'))writeSnapshots([]);};readSnapshots();showLegacyControls();
const sleep=ms=>new Promise(r=>setTimeout(r,ms));async function poll(){const deadline=performance.now()+34000;for(let i=0;i<30&&performance.now()<deadline;i++){await sleep(1000);try{let r=await api('/api/scan');if(r.state==='ready'){render(r);$('message').textContent='Scan complete.';return;}if(r.state==='error')throw Error('Radio scan failed');}catch(e){if(e.message==='Radio scan failed')throw e;}}throw Error('Scan timed out. Reconnect and retry.');}
$('scan').onclick=async()=>{if(busy)return;busy=true;$('scan').disabled=true;$('message').textContent='Scanning… Wi-Fi may briefly pause.';try{await api('/api/scan',{method:'POST',headers:{'X-Phantom-Request':'scan'}});await poll();}catch(e){$('message').textContent=e.message;}finally{busy=false;await status();}};
$('refresh').onclick=status;function csvCell(v){let s=String(v);if(/^[\s]*[=+\-@\t\r]/.test(s))s="'"+s;return '"'+s.replaceAll('"','""')+'"';}$('export').onclick=()=>{let rows=['SSID,RSSI_dBm,Channel,Security',...data.map(n=>[csvCell(n.ssid),n.rssi,n.channel,csvCell(n.security)].join(','))];let url=URL.createObjectURL(new Blob([rows.join('\r\n')],{type:'text/csv'}));let a=document.createElement('a');a.href=url;a.download='phantom-wifi.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};

$('readPins').onclick=async()=>{
 const button=$('readPins');
 if(button.disabled)return;
 button.disabled=true;
 try {
   const result=await api('/api/electronics/status');
   const levels=result.pins.map(({label,gpio,level})=>label+' (GPIO '+gpio+'): '+(level===1?'HIGH':'LOW'));
   $('pinLevels').textContent=(demo?'Sample · ':'')+levels.join(' · ');
 } catch(error) {
   $('pinLevels').textContent=error.message;
 } finally {
   button.disabled=false;
 }
};

function renderLink(s){
 linkConnected=!!s.connected;
 const labels={disabled:'Station tools disabled in this build',offline:'Reconnect to Phantom Wi-Fi and refresh link',disconnected:'Not connected',connecting:'Connecting…',connected:'Connected',timeout:'Connection timed out',lost:'Connection lost',subnet_conflict:'Network overlaps Phantom AP subnet; disconnected'};
 $('linkMessage').textContent=(demo?'SAMPLE · ':'')+(labels[s.state]||s.state)+(linkConnected?' · '+s.ssid:'');
 for(const [id,k] of [['stationIp','ip'],['gateway','gateway'],['subnet','subnet'],['dnsServer','dns']]) $(id).textContent=linkConnected?(s[k]||'—'):'—';
 $('stationSignal').textContent=linkConnected?s.rssi+' dBm · ch '+s.channel:'—';
 if(!linkConnected)$('dnsResult').textContent='—';
 const unavailable=s.state==='disabled'||s.state==='offline';
 $('joinSsid').disabled=unavailable;$('joinKey').disabled=unavailable;
 $('checkDns').disabled=!linkConnected||linkBusy;$('joinLink').disabled=unavailable||linkConnected||linkBusy||s.state==='connecting';$('leaveLink').disabled=linkBusy||(!linkConnected&&s.state!=='connecting');
}
async function linkStatus(){try{const s=await api('/api/link');renderLink(s);return s}catch(e){renderLink({state:'offline',connected:false});throw e}}
function linkPost(path,command,body){return api(path,{method:'POST',headers:{'X-Phantom-Request':command,'Content-Type':'application/x-www-form-urlencoded'},...(body?{body}: {})})}
$('linkRefresh').onclick=()=>linkStatus().catch(()=>{});
$('joinLink').onclick=async()=>{if(linkBusy||busy||linkConnected)return;const ssid=$('joinSsid').value||'',password=$('joinKey').value||'';if(!ssid){$('linkMessage').textContent='Enter your network name.';return;}if(!demo&&!confirm('Connect only to a network you own or administer? The password is sent over local HTTP on the private Phantom AP.'))return;linkBusy=true;$('joinLink').disabled=true;$('joinKey').value='';$('dnsResult').textContent='—';try{await linkPost('/api/link/join','join','ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(password));for(let i=0;i<24;i++){if(i)await sleep(1000);try{const s=await linkStatus();if(s.state!=='connecting')break;}catch(e){if(i===23)throw e;}}}catch(e){$('linkMessage').textContent=e.message+' — refresh link after reconnecting.';}finally{linkBusy=false;$('joinLink').disabled=linkConnected;$('leaveLink').disabled=false;$('checkDns').disabled=!linkConnected;}};
$('leaveLink').onclick=async()=>{if(linkBusy||busy)return;linkBusy=true;try{await linkPost('/api/link/leave','leave');}catch(e){$('linkMessage').textContent=e.message;}finally{linkBusy=false;await linkStatus().catch(()=>{});}};
$('checkDns').onclick=async()=>{if(linkBusy||busy||!linkConnected)return;linkBusy=true;$('checkDns').disabled=true;$('dnsResult').textContent='Checking…';try{const r=await linkPost('/api/link/dns','dns');$('dnsResult').textContent=(demo?'Sample · ':'')+(r.resolved?r.address+' · '+r.elapsedMs+' ms':'Not resolved');}catch(e){$('dnsResult').textContent=e.message;}finally{linkBusy=false;$('checkDns').disabled=!linkConnected;}};
renderLink({state:'offline',connected:false});
linkStatus().catch(()=>{});

bars([]);status();api('/api/scan').then(r=>{if(r.state==='ready')render(r);else if(r.state==='scanning'){busy=true;$('scan').disabled=true;poll().catch(e=>$('message').textContent=e.message).finally(()=>{busy=false;status()})}}).catch(()=>{});
</script></body></html>
)PHANTOM";
