const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
const html=fs.readFileSync('dashboard-preview.html','utf8').trim();
const embedded=fs.readFileSync('include/dashboard.h','utf8').split('R"PHANTOM(\n')[1].split('\n)PHANTOM";')[0];
assert.equal(html,embedded,'Preview must match embedded dashboard');
const js=html.match(/<script>([\s\S]*?)<\/script>/)[1];
function element(){return {children:[],style:{},textContent:'',append(...x){this.children.push(...x)},replaceChildren(){this.children=[]},click(){}}}
const elements=new Map(),storage=new Map();const ctx=vm.createContext({location:{protocol:'file:'},document:{getElementById(id){if(!elements.has(id))elements.set(id,element());return elements.get(id)},createElement:element},localStorage:{getItem:k=>storage.get(k)||null,setItem:(k,v)=>storage.set(k,v),removeItem:k=>storage.delete(k)},setTimeout,performance,console});
vm.runInContext(js,ctx);
(async()=>{await vm.runInContext('status()',ctx);assert.equal(elements.get('connection').textContent,'DEMO / SAMPLE DATA');assert.equal(elements.get('cpu').textContent,'80 MHz');await vm.runInContext("api('/api/scan',{method:'POST'}).then(()=>api('/api/scan')).then(render)",ctx);assert.equal(elements.get('networks').children.length,4);assert.equal(elements.get('bars').children.length,14);assert.equal(vm.runInContext('csvCell("=1+1")',ctx),'"\'=1+1"');assert.equal(vm.runInContext('csvCell(\'a"b\')',ctx),'"a""b"');elements.get('filter').value='Guest';vm.runInContext('renderRows()',ctx);assert.equal(elements.get('networks').children.length,1);elements.get('filter').value='';elements.get('sort').value='channel';vm.runInContext('renderRows()',ctx);assert.equal(elements.get('networks').children.length,4);assert.equal(elements.get('scans').textContent,1);assert.equal(vm.runInContext('snapshotData().networks.length',ctx),4);assert.equal(vm.runInContext('snapshotData().channelCounts.length',ctx),14);vm.runInContext('writeSnapshots([snapshotData()]);readSnapshots()',ctx);assert.equal(vm.runInContext('saved.length',ctx),1);vm.runInContext('writeSnapshots([])',ctx);assert.equal(vm.runInContext('saved.length',ctx),0);await vm.runInContext("api('/api/link/join',{method:'POST'}).then(()=>linkStatus())",ctx);assert.equal(elements.get('gateway').textContent,'192.168.1.1');assert.equal(elements.get('stationIp').textContent,'192.168.1.50');assert.equal(elements.get('checkDns').disabled,false);await vm.runInContext("$('checkDns').onclick()",ctx);assert.match(elements.get('dnsResult').textContent,/Sample/);await vm.runInContext("api('/api/link/leave',{method:'POST'}).then(()=>linkStatus())",ctx);assert.equal(elements.get('gateway').textContent,'—');assert.equal(elements.get('checkDns').disabled,true);console.log('PASS: network toolkit demo, link state, DNS sample and disconnect');console.log('PASS: embedded/preview match, sample status, scan rendering, channel bars, CSV escaping, filtering, scan telemetry snapshot schema and storage round-trip');})().catch(e=>{console.error(e);process.exitCode=1});

async function testLiveRequests() {
  const nodes = new Map();
  let intervalCallback;
  let statusCalls = 0;
  const liveStorage = new Map();
  const live = vm.createContext({
    location: {protocol: 'http:'},
    document: {
      getElementById(id) {
        if (!nodes.has(id)) nodes.set(id, element());
        return nodes.get(id);
      },
      createElement: element,
    },
    localStorage: {
      getItem: key => liveStorage.get(key) ?? null,
      setItem: (key,value) => liveStorage.set(key,value),
      removeItem: key => liveStorage.delete(key),
    },
    setInterval(callback, delay) {
      assert.equal(delay, 15000);
      intervalCallback = callback;
      return 1;
    },
    clearInterval() { intervalCallback = undefined; },
    AbortController,
    performance,
    setTimeout: callback => setTimeout(callback, 15),
    clearTimeout,
    fetch: async (path, {signal}) => {
      if (path === '/api/status') ++statusCalls;
      if (path === '/stalled') {
        return {ok: true, json: () => new Promise((_, reject) => {
          signal.addEventListener('abort', () => reject(signal.reason), {once: true});
        })};
      }
      if (path === '/error') return {ok: false, json: async () => ({error: 'Radio busy'})};
      if (path === '/api/link') return {ok: true, json: async () => ({state: 'disabled', connected: false})};
      return {ok: true, json: async () => ({state: 'idle'})};
    },
  });
  vm.runInContext(js, live);
  await vm.runInContext('linkStatus()', live);
  assert.match(nodes.get('linkMessage').textContent, /disabled/);
  assert.equal(nodes.get('joinLink').disabled, true);
  assert.equal(nodes.get('joinKey').disabled, true);
  await assert.rejects(vm.runInContext("api('/error')", live), /Radio busy/);
  await assert.rejects(vm.runInContext("api('/stalled')", live), /Device request timed out/);
  await vm.runInContext('status()', live);
  const baselineCalls = statusCalls;
  vm.runInContext("$('monitor').onclick(); document.hidden = true;", live);
  intervalCallback();
  assert.equal(statusCalls, baselineCalls);
  vm.runInContext('document.hidden = false;', live);
  intervalCallback();
  intervalCallback();
  assert.equal(statusCalls, baselineCalls + 1);
  await vm.runInContext('status()', live);
  vm.runInContext("$('monitor').onclick()", live);
  assert.equal(intervalCallback, undefined);
  const snapshot = {schema: 1, savedAt: '2026-11-24', total: 0, networks: []};
  liveStorage.set('phantom.snapshot.v1', JSON.stringify([snapshot]));
  liveStorage.set('phantom.snapshot.v2.phantom-a', JSON.stringify([snapshot]));
  liveStorage.set('phantom.snapshot.v2.phantom-b', '{"schema":1}');
  vm.runInContext("selectSnapshotDevice('Phantom-a');showLegacyControls()", live);
  assert.equal(vm.runInContext('saved.length', live),1);
  assert.equal(nodes.get('legacyExport').hidden,false);
  vm.runInContext("selectSnapshotDevice('Phantom-b')", live);
  assert.equal(vm.runInContext('saved.length', live),0);
  assert.ok(liveStorage.has('phantom.snapshot.v2.phantom-a'));
  assert.ok(liveStorage.has('phantom.snapshot.v1'));
  console.log('PASS: mocked live errors, request timeout, station gate, foreground monitor and device-scoped snapshots');
}
testLiveRequests().catch(error => {
  console.error(error);
  process.exitCode = 1;
});

vm.runInContext("$('readPins').onclick()",ctx).then(() => {
  assert.match(elements.get('pinLevels').textContent, /Sample · D1 \(GPIO 5\): HIGH/);
  console.log('PASS: read-only GPIO preview is labeled sample data');
}).catch(error => {
  console.error(error);
  process.exitCode = 1;
});
