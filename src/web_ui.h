#ifndef WEB_UI_H
#define WEB_UI_H

#include <Arduino.h>

static const char kWebUiHtml[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>STM32F407 Console — Web Studio & SD Manager</title>
<style>
:root{
  --bg:#0b0f18;--panel:#131b2b;--card:#1a2438;--border:#2a3954;
  --accent:#00e5ff;--green:#00ff66;--amber:#ffae00;--red:#ff5252;--text:#e6edf3;--muted:#8b9bb4;
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--text);font-family:'Segoe UI',system-ui,-apple-system,sans-serif;min-height:100vh;display:flex;flex-direction:column}
header{background:var(--panel);border-bottom:2px solid var(--accent);padding:10px 16px;display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:10px}
.brand{display:flex;align-items:center;gap:10px;font-weight:700;font-size:1.05rem;color:var(--accent)}
.badge{padding:3px 9px;border-radius:12px;font-size:0.76rem;font-weight:700}
.b-on{background:rgba(0,255,102,0.18);color:var(--green);border:1px solid var(--green)}
.b-off{background:rgba(255,82,82,0.18);color:var(--red);border:1px solid var(--red)}
.tele-strip{display:flex;flex-wrap:wrap;gap:12px;align-items:center;font-size:0.84rem}
.tele-item{background:var(--card);padding:4px 10px;border-radius:6px;border:1px solid var(--border)}
.tele-item b{color:var(--accent)}
main{flex:1;display:grid;grid-template-columns:360px 1fr;gap:12px;padding:12px;max-width:1600px;width:100%;margin:0 auto}
@media(max-width:900px){main{grid-template-columns:1fr}}
.pane{background:var(--panel);border:1px solid var(--border);border-radius:10px;display:flex;flex-direction:column;overflow:hidden}
.pane-hdr{background:var(--card);padding:9px 12px;border-bottom:1px solid var(--border);display:flex;align-items:center;justify-content:space-between;gap:8px;flex-wrap:wrap}
.pane-title{font-weight:600;font-size:0.92rem;color:var(--accent)}
.toolbar{display:flex;gap:6px;flex-wrap:wrap;padding:8px 10px;border-bottom:1px solid var(--border);background:rgba(0,0,0,0.18);align-items:center}
button,select,.btn-label{background:var(--card);color:var(--text);border:1px solid var(--border);padding:5px 10px;border-radius:6px;font-size:0.8rem;cursor:pointer;transition:0.15s;display:inline-flex;align-items:center;gap:4px}
button:hover,select:hover,.btn-label:hover{border-color:var(--accent);background:#22304a}
.btn-accent{background:#0277bd;border-color:#00e5ff;color:#fff;font-weight:600}
.btn-green{background:#008948;border-color:#00ff66;color:#fff;font-weight:600}
.btn-red{background:#8e2424;border-color:#ff5252;color:#fff}
.btn-amber{background:#9c6500;border-color:#ffae00;color:#fff}
.path-bar{font-family:Consolas,monospace;font-size:0.82rem;padding:6px 10px;background:#090d14;border-bottom:1px solid var(--border);color:var(--amber);display:flex;justify-content:space-between;align-items:center}
.file-list{flex:1;overflow-y:auto;max-height:calc(100vh - 220px);min-height:320px}
.f-row{display:flex;align-items:center;justify-content:space-between;padding:7px 10px;border-bottom:1px solid rgba(255,255,255,0.05);gap:6px;font-size:0.84rem}
.f-row:hover{background:rgba(0,229,255,0.08)}
.f-row.active{background:rgba(0,229,255,0.16);border-left:3px solid var(--accent)}
.f-name{flex:1;cursor:pointer;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;display:flex;align-items:center;gap:6px}
.f-dir{color:var(--amber);font-weight:600}
.f-meta{color:var(--muted);font-size:0.74rem;min-width:52px;text-align:right}
.f-acts{display:flex;gap:4px}
.f-acts button{padding:3px 6px;font-size:0.74rem}
.editor-wrap{flex:1;display:flex;flex-direction:column;min-height:420px;position:relative}
#codeArea{flex:1;width:100%;background:#090d14;color:#e8f0fe;border:none;padding:12px;font-family:'Cascadia Code',Consolas,'Courier New',monospace;font-size:0.9rem;line-height:1.45;resize:none;outline:none;tab-size:4}
.ac-pop{position:absolute;z-index:50;min-width:280px;max-width:480px;background:#121a2a;border:1px solid var(--accent);border-radius:6px;box-shadow:0 8px 24px rgba(0,0,0,0.6);font-family:Consolas,monospace;font-size:0.8rem;display:none;overflow:hidden}
.ac-list{max-height:216px;overflow-y:auto}
.ac-item{display:flex;align-items:center;gap:8px;padding:3px 8px;cursor:pointer;white-space:nowrap}
.ac-item.sel{background:rgba(0,229,255,0.22)}
.ac-ico{width:14px;text-align:center;font-weight:700}
.ac-name b{color:var(--accent)}
.ac-hint{margin-left:auto;color:var(--muted);overflow:hidden;text-overflow:ellipsis;padding-left:14px}
.ac-doc{border-top:1px solid var(--border);padding:5px 8px;color:#b8c7dc;white-space:normal;background:#0e1522}
.sig-tip{position:absolute;z-index:49;background:#0e1522;border:1px solid var(--amber);border-radius:5px;padding:3px 8px;font-family:Consolas,monospace;font-size:0.78rem;color:#e8f0fe;display:none;white-space:nowrap;pointer-events:none;box-shadow:0 4px 14px rgba(0,0,0,0.5)}
.sig-tip b{color:var(--amber)}
.img-viewer{flex:1;display:none;flex-direction:column;align-items:center;justify-content:center;gap:14px;padding:18px;background:#090d14;min-height:420px;overflow:auto}
.img-stage{padding:12px;border-radius:10px;border:1px dashed var(--border);background:repeating-conic-gradient(#121824 0% 25%, #0c111a 0% 50%) 50% / 20px 20px;display:flex;align-items:center;justify-content:center;max-width:100%}
#previewImg{max-width:100%;max-height:58vh;image-rendering:pixelated;border:1px solid var(--border);box-shadow:0 6px 24px rgba(0,0,0,0.6)}
.img-meta{font-family:Consolas,monospace;font-size:0.84rem;color:var(--accent)}
.status-bar{padding:6px 12px;background:var(--card);border-top:1px solid var(--border);font-size:0.78rem;color:var(--muted);display:flex;justify-content:space-between;align-items:center;gap:8px;flex-wrap:wrap}
.prog-wrap{height:4px;background:#090d14;width:100%;overflow:hidden}
.prog-bar{height:100%;width:0%;background:var(--green);transition:width 0.15s}
.help-box{background:#0e1522;border-top:1px solid var(--border);padding:10px 14px;font-size:0.78rem;line-height:1.45;color:#b8c7dc;display:none;max-height:190px;overflow-y:auto;font-family:Consolas,monospace}
.pad-box{background:#0d1421;border-top:1px solid var(--border);padding:12px 16px;display:none;align-items:center;justify-content:space-around;gap:16px;flex-wrap:wrap;user-select:none}
.dpad{display:grid;grid-template-columns:repeat(3,44px);grid-template-rows:repeat(3,40px);gap:4px}
.act-pad{display:grid;grid-template-columns:repeat(2,64px);gap:8px}
.pad-btn{justify-content:center;font-weight:700;font-size:0.86rem;border-radius:8px;padding:8px;touch-action:none}
.pad-btn.held{background:var(--accent)!important;color:#000!important;border-color:#fff!important;transform:scale(0.96)}
.ota-box{background:#0d1421;border-top:1px solid var(--border);padding:12px 16px;display:none;flex-direction:column;gap:10px}
.ota-row{display:flex;flex-wrap:wrap;gap:12px;align-items:center;justify-content:space-between;background:var(--card);padding:8px 12px;border-radius:8px;border:1px solid var(--border)}
</style>
</head>
<body>
<header>
  <div class="brand">
    <span>🎮 STM32F407 + ESP32-C6 Web Studio</span>
    <span id="stBadge" class="badge b-off">LINK...</span>
  </div>
  <div class="tele-strip">
    <div class="tele-item">App: <b id="tApp">--</b> <button id="btnStopTop" class="btn-red" style="padding:2px 6px;margin-left:4px;display:none" onclick="stopApp()">⏹ Stop</button></div>
    <div class="tele-item">In-Room: <b id="tRoom">--</b></div>
    <div class="tele-item">MCU: <b id="tMcu">--</b></div>
    <div class="tele-item">Outdoor: <b id="tOut">--</b></div>
    <div class="tele-item">STM32 Heap: <b id="tHeap">--</b></div>
    <button class="btn-amber" onclick="captureScreen()" title="Capture live 320x240 display to SD:/screenshots and preview">📸 Capture Screen</button>
    <button id="btnPadToggle" onclick="toggleGamepad()" title="Toggle Virtual Web Gamepad">🎮 Gamepad</button>
    <button id="btnOtaToggle" onclick="toggleOta()" title="Wireless OTA Firmware Update for ESP32-C6 & STM32F407">⚡ OTA</button>
    <div class="tele-item">Theme:
      <select id="thmSel" onchange="setTheme(this.value)">
        <option value="0">Cyber Blue</option>
        <option value="1">Emerald</option>
        <option value="2">Retro Amber</option>
        <option value="3">Synthwave</option>
      </select>
    </div>
  </div>
</header>

<main>
  <!-- Left Pane: SD Card File Explorer -->
  <section class="pane">
    <div class="pane-hdr">
      <span class="pane-title">📁 SD Card Explorer (STM32)</span>
      <div style="display:flex;gap:5px">
        <button onclick="goUp()" title="Parent Folder">⬆ Up</button>
        <button onclick="loadDir(curDir)" title="Refresh">🔄</button>
      </div>
    </div>
    <div class="toolbar">
      <button class="btn-accent" onclick="createNewFile()">+ File</button>
      <button onclick="createNewFolder()">+ Folder</button>
      <label class="btn-label btn-amber">
        📤 Upload
        <input type="file" id="upInput" style="display:none" onchange="uploadSelectedFile(this)">
      </label>
    </div>
    <div class="path-bar">
      <span id="curPathLbl">/</span>
      <span id="itemCountLbl">0 items</span>
    </div>
    <div class="prog-wrap"><div class="prog-bar" id="progBar"></div></div>
    <div class="file-list" id="fileList"></div>
  </section>

  <!-- Right Pane: Universal Editor, Image Viewer & Live C-Script IDE -->
  <section class="pane">
    <div class="pane-hdr">
      <div>
        <span class="pane-title" id="edFileTitle">📝 Universal Editor (.c .h .txt .md .ini .cfg)</span>
        <span id="edDirty" style="color:var(--amber);font-weight:700;display:none"> *modified</span>
      </div>
      <div style="display:flex;gap:6px;flex-wrap:wrap">
        <button id="btnWall" class="btn-amber" style="display:none" onclick="setAsWallpaper()">🖼 Set Console Wallpaper</button>
        <button id="btnDlImg" style="display:none" onclick="downloadFile(curFile)">⬇ Download Image</button>
        <button id="btnApi" onclick="toggleHelp()">📖 C-Script API</button>
        <button id="btnDemo" onclick="loadTemplate()">✨ Demo .c</button>
        <button id="btnSave" class="btn-accent" onclick="saveCurrentFile(false)">💾 Save (Ctrl+S)</button>
        <button class="btn-green" id="btnSaveRun" onclick="saveCurrentFile(true)">▶ Save & Run on Console</button>
        <button class="btn-red" id="btnStopApp" onclick="stopApp()">⏹ Stop App</button>
      </div>
    </div>
    <div class="editor-wrap" id="editorWrap">
      <textarea id="codeArea" spellcheck="false" placeholder="Select any .c, .h, .txt, .md, .ini, .cfg, .log file on the left to view/edit, or click an image (.bmp, .png, .jpg) to preview..."></textarea>
      <div id="acPop" class="ac-pop"><div id="acList" class="ac-list"></div><div id="acDoc" class="ac-doc"></div></div>
      <div id="sigTip" class="sig-tip"></div>
    </div>
    <div class="img-viewer" id="imgWrap">
      <div class="img-stage">
        <img id="previewImg" alt="SD Image Preview">
      </div>
      <div class="img-meta" id="imgMeta">--</div>
      <div style="display:flex;gap:8px;flex-wrap:wrap">
        <button onclick="setImgZoom(1)">1× Actual</button>
        <button onclick="setImgZoom(2)">2× Pixel Art</button>
        <button onclick="setImgZoom(4)">4× Icon Zoom</button>
        <button onclick="setImgZoom(0)">Fit Window</button>
      </div>
    </div>
    <div id="helpBox" class="help-box"></div>
    <div id="padBox" class="pad-box">
      <div class="dpad">
        <div></div>
        <button class="pad-btn" data-bit="4">▲</button>
        <div></div>
        <button class="pad-btn" data-bit="1">◀</button>
        <div style="display:flex;align-items:center;justify-content:center;color:var(--muted);font-size:0.7rem">D-PAD</div>
        <button class="pad-btn" data-bit="2">▶</button>
        <div></div>
        <button class="pad-btn" data-bit="8">▼</button>
        <div></div>
      </div>
      <div style="display:flex;flex-direction:column;gap:8px;align-items:center">
        <div style="display:flex;gap:8px">
          <button class="pad-btn btn-amber" data-bit="256" style="padding:6px 14px">⏯ START</button>
          <button class="pad-btn btn-red" data-bit="128" style="padding:6px 14px">⎋ EXIT / ESC</button>
        </div>
        <label style="font-size:0.76rem;color:var(--muted);cursor:pointer;display:flex;align-items:center;gap:6px">
          <input type="checkbox" id="chkKbPad" checked> PC Keyboard (Arrows / WASD, Enter/Space=A, Z/Backspace=B, C=X, Esc=Exit)
        </label>
      </div>
      <div class="act-pad">
        <button class="pad-btn" data-bit="64" style="border-color:var(--accent)">X</button>
        <button class="pad-btn btn-green" data-bit="16">A (OK)</button>
        <button class="pad-btn btn-amber" data-bit="32">B (Back)</button>
        <button class="pad-btn" onclick="captureScreen()">📸 Shot</button>
      </div>
    </div>
    <div id="otaBox" class="ota-box">
      <div class="ota-row">
        <div>
          <b style="color:var(--accent)">ESP32-C6 Companion OTA</b>
          <div style="font-size:0.76rem;color:var(--muted)">Select PlatformIO <code>firmware.bin</code> to flash dual-OTA slot over Wi-Fi</div>
        </div>
        <div style="display:flex;gap:8px;align-items:center">
          <input type="file" id="espOtaFile" accept=".bin" style="font-size:0.78rem">
          <button class="btn-accent" onclick="uploadEspOta()">⚡ Flash ESP32</button>
        </div>
      </div>
      <div class="ota-row">
        <div>
          <b style="color:var(--green)">STM32F407 Console OTA</b>
          <div style="font-size:0.76rem;color:var(--muted)">Select <code>fw_update.bin</code> (stages to <code>SD:/system/fw_update.bin</code> &amp; reboots into Bootloader)</div>
        </div>
        <div style="display:flex;gap:8px;align-items:center">
          <input type="file" id="stmOtaFile" accept=".bin" style="font-size:0.78rem">
          <button class="btn-green" onclick="uploadStm32Ota()">⚡ Flash STM32</button>
        </div>
      </div>
    </div>
    <div class="status-bar">
      <span id="statusMsg">Ready. Connected to ESP32-C6 Web Studio.</span>
      <span id="cursorInfo">Ln 1, Col 1 | 0 B</span>
    </div>
  </section>
</main>

<script>
let curDir = '/';
let curFile = '';
let isDirty = false;
let themeLockUntil = 0;
let busyCount = 0;
const imgRev = {};
let natW = 0, natH = 0;
const EDIT_EXTS = ['.c','.h','.txt','.md','.ini','.cfg','.log','.json','.csv'];
const IMG_EXTS  = ['.bmp','.png','.jpg','.jpeg','.gif','.sjpg'];

function setStatus(msg, color){
  const el = document.getElementById('statusMsg');
  el.textContent = msg;
  el.style.color = color || 'var(--muted)';
}

function setProgress(pct){
  document.getElementById('progBar').style.width = Math.max(0, Math.min(100, pct)) + '%';
}

function isEditable(name){
  const lower = name.toLowerCase();
  return EDIT_EXTS.some(ext => lower.endsWith(ext));
}

function isImage(name){
  const lower = name.toLowerCase();
  return IMG_EXTS.some(ext => lower.endsWith(ext));
}

function isRunnable(name){
  const lower = name.toLowerCase();
  return lower.endsWith('.c') || lower.endsWith('.bin');
}

function joinPath(dir, name){
  if(!dir || dir === '/') return '/' + name.replace(/^\/+/, '');
  return dir.replace(/\/+$/, '') + '/' + name.replace(/^\/+/, '');
}

function formatBytes(sz){
  if(sz < 1024) return sz + ' B';
  return (sz / 1024).toFixed(1) + ' KB';
}

function showPaneMode(mode){
  const isImg = (mode === 'image');
  document.getElementById('editorWrap').style.display = isImg ? 'none' : 'flex';
  document.getElementById('imgWrap').style.display    = isImg ? 'flex' : 'none';
  document.getElementById('btnWall').style.display    = isImg ? 'inline-flex' : 'none';
  document.getElementById('btnDlImg').style.display   = isImg ? 'inline-flex' : 'none';
  document.getElementById('btnApi').style.display     = isImg ? 'none' : 'inline-flex';
  document.getElementById('btnDemo').style.display    = isImg ? 'none' : 'inline-flex';
  document.getElementById('btnSave').style.display    = isImg ? 'none' : 'inline-flex';
  document.getElementById('btnSaveRun').style.display = isImg ? 'none' : 'inline-flex';
}

function highlightActiveRow(){
  document.querySelectorAll('.f-row').forEach(r => {
    r.classList.toggle('active', r.dataset.path === curFile);
  });
}

async function pollStatus(){
  if(busyCount > 0) return;
  try{
    const r = await fetch('/api/status');
    if(!r.ok) return;
    const d = await r.json();
    const b = document.getElementById('stBadge');
    if(d.stm32_online){
      b.textContent = 'STM32 ONLINE';
      b.className = 'badge b-on';
    } else {
      b.textContent = 'STM32 OFFLINE';
      b.className = 'badge b-off';
    }
    document.getElementById('tApp').textContent = d.app || '--';
    document.getElementById('btnStopTop').style.display = (d.app && d.app !== 'menu' && d.app !== 'connecting...') ? 'inline-flex' : 'none';
    document.getElementById('tRoom').textContent = (d.text_deci > -900) ? ((d.text_deci/10).toFixed(1) + ' °C') : 'N/A';
    document.getElementById('tMcu').textContent = (d.tmcu_deci/10).toFixed(1) + ' °C';
    document.getElementById('tHeap').textContent = d.stm32_heap_kb + ' KB';
    if(d.wth_valid){
      document.getElementById('tOut').textContent = d.wth_city + ' ' + (d.wth_t_deci/10).toFixed(1) + '°C';
    } else {
      document.getElementById('tOut').textContent = 'Syncing...';
    }
    const sel = document.getElementById('thmSel');
    if(document.activeElement !== sel && Date.now() > themeLockUntil) sel.value = String(d.theme || 0);
  }catch(e){}
}

async function loadDir(path){
  curDir = path || '/';
  document.getElementById('curPathLbl').textContent = 'SD:' + curDir;
  setStatus('Scanning SD:' + curDir + ' ...');
  busyCount++;
  try{
    const r = await fetch('/api/fs/list?path=' + encodeURIComponent(curDir));
    const d = await r.json();
    const box = document.getElementById('fileList');
    box.innerHTML = '';
    if(!d.ok){
      box.innerHTML = '<div style="padding:14px;color:var(--red)">SD directory read error or STM32 busy.</div>';
      setStatus('Failed to list directory.', 'var(--red)');
      return;
    }
    d.items.sort((a,b) => (b.dir - a.dir) || a.name.localeCompare(b.name));
    document.getElementById('itemCountLbl').textContent = d.items.length + ' items';
    for(const item of d.items){
      const full = joinPath(curDir, item.name);
      const row = document.createElement('div');
      row.className = 'f-row' + (full === curFile ? ' active' : '');
      row.dataset.path = full;

      const nameEl = document.createElement('div');
      nameEl.className = 'f-name ' + (item.dir ? 'f-dir' : '');
      const icon = item.dir ? '📁 ' : (isImage(item.name) ? '🖼 ' : (isEditable(item.name) ? '📄 ' : '📦 '));
      nameEl.innerHTML = icon + item.name;
      nameEl.onclick = () => {
        if(item.dir) loadDir(full);
        else if(isImage(item.name)) openImageInViewer(full, item.size);
        else if(isEditable(item.name)) openFileInEditor(full);
        else downloadFile(full);
      };

      const metaEl = document.createElement('div');
      metaEl.className = 'f-meta';
      metaEl.textContent = item.dir ? 'DIR' : formatBytes(item.size);

      const acts = document.createElement('div');
      acts.className = 'f-acts';

      if(!item.dir && isRunnable(item.name)){
        const bRun = document.createElement('button');
        bRun.className = 'btn-green';
        bRun.textContent = '▶';
        bRun.title = 'Run on Console';
        bRun.onclick = (e) => { e.stopPropagation(); runOnConsole(full); };
        acts.appendChild(bRun);
      }
      if(!item.dir){
        const bDl = document.createElement('button');
        bDl.textContent = '⬇';
        bDl.title = 'Download';
        bDl.onclick = (e) => { e.stopPropagation(); downloadFile(full); };
        acts.appendChild(bDl);
      }
      const bRen = document.createElement('button');
      bRen.textContent = '✏';
      bRen.title = 'Rename';
      bRen.onclick = (e) => { e.stopPropagation(); renameItem(full, item.name); };
      acts.appendChild(bRen);

      const bDel = document.createElement('button');
      bDel.className = 'btn-red';
      bDel.textContent = '🗑';
      bDel.title = 'Delete';
      bDel.onclick = (e) => { e.stopPropagation(); deleteItem(full, item.name); };
      acts.appendChild(bDel);

      row.appendChild(nameEl);
      row.appendChild(metaEl);
      row.appendChild(acts);
      box.appendChild(row);
    }
    setStatus('Loaded SD:' + curDir);
  }catch(e){
    setStatus('Connection error while listing SD.', 'var(--red)');
  }finally{
    busyCount = Math.max(0, busyCount - 1);
  }
}

function goUp(){
  if(!curDir || curDir === '/') return loadDir('/');
  const idx = curDir.lastIndexOf('/');
  loadDir(idx <= 0 ? '/' : curDir.substring(0, idx));
}

async function openFileInEditor(path){
  if(isDirty && !confirm('Discard unsaved changes in ' + curFile + '?')) return;
  showPaneMode('editor');
  setStatus('Reading ' + path + ' from STM32 SD...');
  setProgress(35);
  busyCount++;
  try{
    const r = await fetch('/api/fs/read?path=' + encodeURIComponent(path));
    if(!r.ok){
      setProgress(0);
      setStatus('Failed to read ' + path, 'var(--red)');
      return;
    }
    const txt = await r.text();
    curFile = path;
    isDirty = false;
    document.getElementById('codeArea').value = txt;
    document.getElementById('edFileTitle').textContent = '📝 ' + path;
    document.getElementById('edDirty').style.display = 'none';
    highlightActiveRow();
    updateCursorInfo();
    setProgress(100);
    setTimeout(() => setProgress(0), 400);
    setStatus('Opened ' + path + ' (' + txt.length + ' bytes)', 'var(--green)');
  }catch(e){
    setProgress(0);
    setStatus('Error reading file', 'var(--red)');
  }finally{
    busyCount = Math.max(0, busyCount - 1);
  }
}

function setImgZoom(scale){
  const img = document.getElementById('previewImg');
  if(!natW || !natH) return;
  if(scale === 0){
    img.style.width = '';
    img.style.height = '';
    img.style.maxWidth = '100%';
    img.style.maxHeight = '58vh';
  } else {
    img.style.maxWidth = 'none';
    img.style.maxHeight = 'none';
    img.style.width  = (natW * scale) + 'px';
    img.style.height = (natH * scale) + 'px';
  }
}

let imgBusy = false;
function openImageInViewer(path, byteSize){
  if(isDirty && !confirm('Discard unsaved changes in ' + curFile + '?')) return;
  isDirty = false;
  curFile = path;
  highlightActiveRow();
  showPaneMode('image');
  document.getElementById('edFileTitle').textContent = '🖼 ' + path;
  document.getElementById('edDirty').style.display = 'none';
  document.getElementById('imgMeta').textContent = 'Loading image from SD...';
  setStatus('Loading image ' + path + '...');
  setProgress(40);
  if(!imgBusy){ imgBusy = true; busyCount++; }

  const img = document.getElementById('previewImg');
  img.onload = () => {
    if(imgBusy){ imgBusy = false; busyCount = Math.max(0, busyCount - 1); }
    natW = img.naturalWidth;
    natH = img.naturalHeight;
    setImgZoom(natW <= 96 ? 3 : (natW <= 320 ? 2 : 0));
    document.getElementById('imgMeta').textContent =
      path + '  —  ' + natW + ' × ' + natH + ' px  (' + formatBytes(byteSize || 0) + ')';
    document.getElementById('cursorInfo').textContent = natW + '×' + natH + ' px';
    setProgress(100);
    setTimeout(() => setProgress(0), 350);
    setStatus('Loaded image ' + path, 'var(--green)');
  };
  img.onerror = () => {
    if(imgBusy){ imgBusy = false; busyCount = Math.max(0, busyCount - 1); }
    setProgress(0);
    document.getElementById('imgMeta').textContent = 'Failed to decode image';
    setStatus('Failed to load image ' + path, 'var(--red)');
  };
  const rev = imgRev[path] ? ('&v=' + imgRev[path]) : '';
  img.src = '/api/fs/read?path=' + encodeURIComponent(path) + rev;
}

async function setAsWallpaper(){
  if(!curFile) return;
  setStatus('Applying ' + curFile + ' as Console desktop wallpaper...', 'var(--amber)');
  const r = await fetch('/api/wallpaper?path=' + encodeURIComponent(curFile), {method:'POST'});
  if(r.ok) setStatus('Wallpaper updated on Console: ' + curFile, 'var(--green)');
  else setStatus('Failed to set wallpaper', 'var(--red)');
}

function toHexString(u8){
  let s = '';
  for(let i = 0; i < u8.length; i++){
    s += u8[i].toString(16).padStart(2, '0').toUpperCase();
  }
  return s;
}

async function writeBytesChunked(path, uint8Arr){
  const CHUNK = 768;
  const total = uint8Arr.length;
  busyCount++;
  try{
    if(total === 0){
      const r = await fetch('/api/fs/write?path=' + encodeURIComponent(path) + '&mode=W', {
        method: 'POST',
        headers: {'Content-Type': 'text/plain'},
        body: ''
      });
      imgRev[path] = Date.now();
      return r.ok;
    }
    for(let off = 0; off < total; off += CHUNK){
      const slice = uint8Arr.subarray(off, Math.min(off + CHUNK, total));
      const mode = (off === 0) ? 'W' : 'A';
      setProgress(Math.round(((off + slice.length) * 100) / total));
      const r = await fetch('/api/fs/write?path=' + encodeURIComponent(path) + '&mode=' + mode, {
        method: 'POST',
        headers: {'Content-Type': 'text/plain'},
        body: toHexString(slice)
      });
      if(!r.ok) return false;
    }
    imgRev[path] = Date.now();
    return true;
  }finally{
    busyCount = Math.max(0, busyCount - 1);
  }
}

async function saveCurrentFile(andRun){
  if(!curFile){
    const defPath = joinPath(curDir === '/' ? '/games' : curDir, andRun ? 'script.c' : 'notes.txt');
    const inp = prompt('Enter file path on SD card:', defPath);
    if(!inp) return;
    curFile = inp.startsWith('/') ? inp : ('/' + inp);
  }
  const text = document.getElementById('codeArea').value;
  const bytes = new TextEncoder().encode(text);
  setStatus('Saving ' + curFile + ' (' + bytes.length + ' B)...', 'var(--amber)');
  const ok = await writeBytesChunked(curFile, bytes);
  setTimeout(() => setProgress(0), 400);
  if(!ok){
    setStatus('Error saving ' + curFile + ' to SD!', 'var(--red)');
    return;
  }
  isDirty = false;
  document.getElementById('edFileTitle').textContent = '📝 ' + curFile;
  document.getElementById('edDirty').style.display = 'none';
  setStatus('Saved ' + curFile + ' (' + bytes.length + ' B)', 'var(--green)');
  await loadDir(curDir);
  if(andRun){
    await runOnConsole(curFile);
  }
}

async function uploadSelectedFile(input){
  if(!input.files || !input.files[0]) return;
  const f = input.files[0];
  const target = joinPath(curDir, f.name);
  setStatus('Uploading ' + target + ' (' + formatBytes(f.size) + ')...', 'var(--amber)');
  const buf = new Uint8Array(await f.arrayBuffer());
  const ok = await writeBytesChunked(target, buf);
  input.value = '';
  setTimeout(() => setProgress(0), 500);
  if(ok){
    setStatus('Uploaded ' + target + ' successfully!', 'var(--green)');
    loadDir(curDir);
  } else {
    setStatus('Upload failed for ' + target, 'var(--red)');
  }
}

function downloadFile(path){
  window.location.href = '/api/fs/read?dl=1&path=' + encodeURIComponent(path);
}

async function createNewFile(){
  const name = prompt('New file name (.c, .txt, .md, .ini, .cfg, .h):', 'new_script.c');
  if(!name) return;
  showPaneMode('editor');
  const full = joinPath(curDir, name);
  curFile = full;
  document.getElementById('codeArea').value = name.endsWith('.c')
    ? '// ' + name + '\nvoid main() {\n    clear_screen(COLOR_DARK_BG);\n    draw_text(40, 100, "HELLO FROM WEB IDE!", COLOR_CYAN, COLOR_DARK_BG, 2);\n    while (1) {\n        if (input_pressed(KEY_EXIT)) break;\n        frame_sync(16);\n    }\n}\n'
    : '';
  document.getElementById('edFileTitle').textContent = '📝 ' + full;
  await saveCurrentFile(false);
}

async function createNewFolder(){
  const name = prompt('New folder name:');
  if(!name) return;
  const full = joinPath(curDir, name);
  const r = await fetch('/api/fs/mkdir?path=' + encodeURIComponent(full), {method:'POST'});
  if(r.ok){ setStatus('Created folder ' + full, 'var(--green)'); loadDir(curDir); }
  else setStatus('Failed to create folder', 'var(--red)');
}

async function renameItem(fullPath, oldName){
  const newName = prompt('Rename ' + oldName + ' to:', oldName);
  if(!newName || newName === oldName) return;
  const newFull = joinPath(curDir, newName);
  const r = await fetch('/api/fs/rename?old=' + encodeURIComponent(fullPath) + '&new=' + encodeURIComponent(newFull), {method:'POST'});
  if(r.ok){
    if(curFile === fullPath){ curFile = newFull; document.getElementById('edFileTitle').textContent = '📝 ' + curFile; }
    setStatus('Renamed to ' + newFull, 'var(--green)');
    loadDir(curDir);
  } else setStatus('Rename failed!', 'var(--red)');
}

async function deleteItem(fullPath, name){
  if(!confirm('Delete "' + name + '" from SD card?')) return;
  const r = await fetch('/api/fs/delete?path=' + encodeURIComponent(fullPath), {method:'POST'});
  if(r.ok){
    if(curFile === fullPath){ curFile = ''; document.getElementById('edFileTitle').textContent = '📝 Universal Editor'; }
    setStatus('Deleted ' + fullPath, 'var(--green)');
    loadDir(curDir);
  } else setStatus('Delete failed (folder must be empty)!', 'var(--red)');
}

async function runOnConsole(path){
  setStatus('Launching ' + path + ' on STM32 Console...', 'var(--accent)');
  const r = await fetch('/api/app/run?path=' + encodeURIComponent(path), {method:'POST'});
  if(r.ok) setStatus('Running ' + path + ' on Console!', 'var(--green)');
  else setStatus('Failed to launch ' + path, 'var(--red)');
  setTimeout(pollStatus, 350);
}

async function stopApp(){
  await fetch('/api/app/stop', {method:'POST'});
  setStatus('Stopped running app on Console.', 'var(--amber)');
  setTimeout(pollStatus, 350);
}

async function setTheme(id){
  themeLockUntil = Date.now() + 2500;
  await fetch('/api/theme?id=' + id, {method:'POST'});
  setStatus('Switched console theme.');
}

function toggleHelp(){
  const b = document.getElementById('helpBox');
  b.style.display = (b.style.display === 'block') ? 'none' : 'block';
}

async function captureScreen(){
  setStatus('Capturing live 320×240 Console display to SD...', 'var(--amber)');
  setProgress(45);
  busyCount++;
  try{
    const r = await fetch('/api/screenshot', {method:'POST'});
    const d = await r.json();
    if(!r.ok || !d.ok || !d.path){
      setProgress(0);
      setStatus('Screen capture failed (check SD card).', 'var(--red)');
      return;
    }
    imgRev[d.path] = Date.now();
    if(curDir === '/screenshots') await loadDir('/screenshots');
    openImageInViewer(d.path, 153654);
    setStatus('Captured live screen -> ' + d.path, 'var(--green)');
  }catch(e){
    setProgress(0);
    setStatus('Screen capture request failed.', 'var(--red)');
  }finally{
    busyCount = Math.max(0, busyCount - 1);
  }
}

let padOpen = false;
let padMask = 0;
let padTimer = null;
let padQueue = Promise.resolve();

function sendPadMask(m){
  padQueue = padQueue.then(() =>
    fetch('/api/input?mask=' + m + '&lx=0&ly=0', {method:'POST'}).catch(() => {})
  );
}

function setPadBit(bit, down){
  const next = down ? (padMask | bit) : (padMask & ~bit);
  if(next === padMask) return;
  padMask = next;
  document.querySelectorAll('.pad-btn[data-bit]').forEach(b => {
    const bb = parseInt(b.dataset.bit, 10);
    b.classList.toggle('held', (padMask & bb) !== 0);
  });
  sendPadMask(padMask);
  if(padMask !== 0 && !padTimer){
    padTimer = setInterval(() => {
      if(padMask === 0){ clearInterval(padTimer); padTimer = null; return; }
      sendPadMask(padMask);
    }, 280);
  }
}

function toggleGamepad(){
  padOpen = !padOpen;
  document.getElementById('padBox').style.display = padOpen ? 'flex' : 'none';
  document.getElementById('btnPadToggle').classList.toggle('btn-accent', padOpen);
  if(!padOpen && padMask !== 0){
    padMask = 0;
    sendPadMask(0);
  }
}

document.querySelectorAll('.pad-btn[data-bit]').forEach(btn => {
  const bit = parseInt(btn.dataset.bit, 10);
  const press = (e) => { e.preventDefault(); setPadBit(bit, true); };
  const rel   = (e) => { e.preventDefault(); setPadBit(bit, false); };
  btn.addEventListener('pointerdown', press);
  btn.addEventListener('pointerup', rel);
  btn.addEventListener('pointerleave', rel);
  btn.addEventListener('pointercancel', rel);
});

/* Exact SDK_KEY_* bitmask mapping from console_abi.h:
 * 1=LEFT, 2=RIGHT, 4=UP, 8=DOWN, 16=A(Enter), 32=B(Back), 64=X, 128=EXIT(Esc), 256=START */
const KB_MAP = {
  'ArrowLeft': 1, 'KeyA': 1,
  'ArrowRight': 2, 'KeyD': 2,
  'ArrowUp': 4, 'KeyW': 4,
  'ArrowDown': 8, 'KeyS': 8,
  'Space': 16, 'Enter': 16, 'NumpadEnter': 16,
  'KeyZ': 32, 'KeyX': 32, 'Backspace': 32,
  'KeyC': 64,
  'Escape': 128,
  'KeyP': 256
};

window.addEventListener('keydown', (e) => {
  if(!padOpen || !document.getElementById('chkKbPad').checked) return;
  const el = document.activeElement;
  const tag = (el && el.tagName) || '';
  if(tag === 'TEXTAREA' || tag === 'SELECT' || (tag === 'INPUT' && el.type !== 'checkbox')) return;
  const bit = KB_MAP[e.code];
  if(bit){
    e.preventDefault();
    if(!e.repeat) setPadBit(bit, true);
  }
});

window.addEventListener('keyup', (e) => {
  if(!padOpen) return;
  const bit = KB_MAP[e.code];
  if(bit){
    e.preventDefault();
    setPadBit(bit, false);
  }
});

let otaOpen = false;
function toggleOta(){
  otaOpen = !otaOpen;
  document.getElementById('otaBox').style.display = otaOpen ? 'flex' : 'none';
  document.getElementById('btnOtaToggle').classList.toggle('btn-accent', otaOpen);
}

function uploadEspOta(){
  const inp = document.getElementById('espOtaFile');
  if(!inp.files || !inp.files[0]) return alert('Select ESP32 firmware.bin first!');
  const f = inp.files[0];
  if(!confirm('Flash ESP32-C6 OTA firmware "' + f.name + '" (' + formatBytes(f.size) + ')?')) return;
  busyCount++;
  setStatus('Uploading ESP32-C6 OTA firmware (' + formatBytes(f.size) + ')...', 'var(--amber)');
  setProgress(5);
  const fd = new FormData();
  fd.append('firmware', f, f.name);
  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/api/ota/esp32?size=' + f.size, true);
  xhr.upload.onprogress = (ev) => {
    if(ev.lengthComputable){
      const pct = Math.round((ev.loaded * 100) / ev.total);
      setProgress(pct);
      setStatus('Flashing ESP32-C6 OTA: ' + pct + '%', 'var(--amber)');
    }
  };
  xhr.onload = () => {
    busyCount = Math.max(0, busyCount - 1);
    if(xhr.status === 200){
      setProgress(100);
      setStatus('ESP32-C6 OTA complete! Rebooting... Reloading page in 5s.', 'var(--green)');
      inp.value = '';
      setTimeout(() => location.reload(), 5000);
    } else {
      setProgress(0);
      setStatus('ESP32-C6 OTA failed: ' + xhr.responseText, 'var(--red)');
    }
  };
  xhr.onerror = () => {
    busyCount = Math.max(0, busyCount - 1);
    setProgress(0);
    setStatus('ESP32-C6 OTA network error.', 'var(--red)');
  };
  xhr.send(fd);
}

async function uploadStm32Ota(){
  const inp = document.getElementById('stmOtaFile');
  if(!inp.files || !inp.files[0]) return alert('Select STM32 fw_update.bin first!');
  const f = inp.files[0];
  const buf = new Uint8Array(await f.arrayBuffer());
  if(buf.length < 64 || buf[0] !== 0x34 || buf[1] !== 0x57 || buf[2] !== 0x46 || buf[3] !== 0x55){
    return alert('Invalid STM32 firmware package! Expected fw_update.bin with 4WFU header.');
  }
  if(!confirm('Stage "' + f.name + '" (' + formatBytes(f.size) + ') to SD:/system/fw_update.bin and reboot STM32 into Bootloader?')) return;
  setStatus('Uploading STM32 firmware to SD:/system/fw_update.bin...', 'var(--amber)');
  await fetch('/api/fs/mkdir?path=/system', {method:'POST'}).catch(() => {});
  const ok = await writeBytesChunked('/system/fw_update.bin', buf);
  if(!ok){
    setProgress(0);
    setStatus('Failed to write SD:/system/fw_update.bin!', 'var(--red)');
    return;
  }
  setProgress(100);
  setStatus('Staged fw_update.bin! Rebooting STM32 into Bootloader...', 'var(--green)');
  inp.value = '';
  await fetch('/api/sys/reboot', {method:'POST'}).catch(() => {});
  setTimeout(() => setProgress(0), 1500);
}

function loadTemplate(){
  if(isDirty && !confirm('Replace editor content with Dual-Screen C-Script template?')) return;
  showPaneMode('editor');
  if(!curFile || !curFile.endsWith('.c')) curFile = joinPath(curDir === '/' ? '/games' : curDir, 'dual_hud_demo.c');
  document.getElementById('edFileTitle').textContent = '📝 ' + curFile;
  document.getElementById('codeArea').value =
`// Dual-Screen Companion HUD + Live Bounce Demo
void main() {
    int px = 132, old_px = 132;
    int bx = 160, by = 90, vx = 3, vy = 2;
    int score = 0, best = load_int("best.dat", 0);

    clear_screen(COLOR_DARK_BG);
    fill_rect(0, 0, 320, 20, COLOR_PANEL_BG);
    draw_text(8, 6, "DUAL-SCREEN PONG   [ESC: EXIT]", COLOR_CYAN, COLOR_PANEL_BG, 1);

    esp_hud_clear();
    esp_hud_text(0, "=== DUAL-SCREEN TELEMETRY ===");
    esp_hud_num(1, "SCORE:     ", score);
    esp_hud_num(2, "HIGH BEST: ", best);
    esp_hud_num(4, "ROOM TEMP: ", temp_ext());
    esp_hud_text(5, "LED = GREEN (READY)");
    esp_led(0, 48, 16);

    fill_rect(px, 222, 56, 6, COLOR_CYAN);
    while (1) {
        int k = input_poll();
        if (input_pressed(KEY_EXIT)) break;

        old_px = px;
        if (k & KEY_LEFT)  px -= 5;
        if (k & KEY_RIGHT) px += 5;
        px = clamp(px + pad_lx() / 24, 4, 260);
        if (px != old_px) {
            fill_rect(old_px, 222, 56, 6, COLOR_DARK_BG);
            fill_rect(px, 222, 56, 6, COLOR_CYAN);
        }

        fill_rect(bx - 4, by - 4, 9, 9, COLOR_DARK_BG);
        bx += vx;
        by += vy;
        if (bx <= 8 || bx >= 311) { vx = -vx; bx = clamp(bx, 8, 311); beep(12); }
        if (by <= 26)             { vy = -vy; by = 26; beep(12); }

        if (vy > 0 && by >= 216 && by <= 224 && bx >= px - 4 && bx <= px + 60) {
            by = 216;
            vy = -vy;
            score++;
            beep(25);
            esp_led(0, 60, 20);
            if (score > best) {
                best = score;
                save_int("best.dat", best);
                esp_led(0, 45, 65);
            }
            esp_hud_num(1, "SCORE:     ", score);
            esp_hud_num(2, "HIGH BEST: ", best);
        } else if (by > 234) {
            beep(120);
            score = 0;
            bx = 160; by = 80; vy = 2;
            esp_led(64, 0, 0);
            esp_hud_num(1, "SCORE:     ", score);
            esp_hud_text(5, "MISSED! RESETTING...");
        }

        fill_rect(bx - 4, by - 4, 9, 9, COLOR_AMBER);
        draw_num(276, 6, score, COLOR_NEON_GREEN, COLOR_PANEL_BG, 1, 3);
        frame_sync(16);
    }
}
`;
  isDirty = true;
  document.getElementById('edDirty').style.display = 'inline';
  updateCursorInfo();
}

const ta = document.getElementById('codeArea');
function updateCursorInfo(){
  const v = ta.value;
  const sel = ta.selectionStart || 0;
  const lines = v.substring(0, sel).split('\n');
  document.getElementById('cursorInfo').textContent =
    'Ln ' + lines.length + ', Col ' + (lines[lines.length-1].length + 1) + ' | ' + v.length + ' B';
}
ta.addEventListener('input', () => {
  isDirty = true;
  document.getElementById('edDirty').style.display = 'inline';
  updateCursorInfo();
});
ta.addEventListener('click', updateCursorInfo);
ta.addEventListener('keyup', updateCursorInfo);
ta.addEventListener('keydown', (e) => {
  if(acHandleKey(e)) return;
  if((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 's'){
    e.preventDefault();
    saveCurrentFile(false);
  } else if(e.key === 'Tab'){
    e.preventDefault();
    const s = ta.selectionStart, end = ta.selectionEnd;
    ta.value = ta.value.substring(0, s) + '    ' + ta.value.substring(end);
    ta.selectionStart = ta.selectionEnd = s + 4;
  }
});

/* ==========================================================================
 * C-Script autocomplete & signature help.
 * CS_FUNCS / CS_ALIASES / CS_CONSTS mirror the built-ins of Core/Src/script_engine.c:
 * keep them in sync when the interpreter gains new functions or constants.
 * ========================================================================== */
const CS_FUNCS = [
  ['clear_screen','color','Graphics','Fill the whole 320x240 screen'],
  ['fill_rect','x, y, w, h, color','Graphics','Fill a rectangle'],
  ['draw_pixel','x, y, color','Graphics','Plot one pixel'],
  ['draw_text','x, y, "text", fg, bg, scale','Graphics','Draw text; bg and scale are optional'],
  ['draw_num','x, y, value, fg, bg, scale, digits','Graphics','Draw an integer; bg, scale and min digits are optional'],
  ['RGB565','r, g, b','Graphics','Pack 8-bit R, G, B into an RGB565 color'],
  ['input_poll','','Input','Refresh input once per frame; returns the held KEY_* mask'],
  ['input_pressed','mask','Input','Keys newly pressed since the last input_poll()'],
  ['input_held','mask','Input','Keys currently held down'],
  ['input_repeat','mask','Input','Pressed with auto-repeat (menus, grid movement)'],
  ['get_keys','','Input','Raw KEY_* mask without edge detection'],
  ['pad_lx','','Input','Gamepad left stick X (-127..127)'],
  ['pad_ly','','Input','Gamepad left stick Y (-127..127)'],
  ['pad_rx','','Input','Gamepad right stick X (-127..127)'],
  ['pad_ry','','Input','Gamepad right stick Y (-127..127)'],
  ['pad_buttons','','Input','Raw gamepad SDK_PAD_* button mask'],
  ['mouse_x','','Input','Mouse / touch X (0..319)'],
  ['mouse_y','','Input','Mouse / touch Y (0..239)'],
  ['mouse_buttons','','Input','Mouse buttons (bit 0 = left)'],
  ['frame_sync','period_ms','Timing','Wait for the next frame (16 = ~60 FPS)'],
  ['sleep_ms','ms','Timing','Sleep and yield the CPU to the OS'],
  ['get_tick','','Timing','Milliseconds since boot'],
  ['beep','ms','Sound','Buzzer beep (default 40 ms)'],
  ['play_tone','freq_hz, ms','Sound','Tone (the current active buzzer ignores the frequency)'],
  ['rand','n','Math','Random 0..n-1 (no argument: 0..2^31-1)'],
  ['rand_range','min, max','Math','Random min..max-1'],
  ['abs','x','Math','Absolute value'],
  ['min','a, b','Math','Smaller of two values'],
  ['max','a, b','Math','Larger of two values'],
  ['clamp','value, lo, hi','Math','Limit value to lo..hi'],
  ['save_int','"file", value','Storage','Save an int to SD:/appdata/<app>/file; returns 1 on success'],
  ['load_int','"file", default','Storage','Load an int saved by save_int(), or default'],
  ['file_open','"path", flags','Storage','Open a file with SDK_FILE_* flags; returns a handle or an error < 0'],
  ['file_read','fd, buf, len','Storage','Read bytes into an array; returns the byte count'],
  ['file_write','fd, buf, len','Storage','Write bytes from an array; returns the byte count'],
  ['file_seek','fd, offset','Storage','Move to an absolute offset'],
  ['file_size','fd','Storage','File size in bytes'],
  ['file_close','fd','Storage','Close a file handle'],
  ['file_remove','"path"','Storage','Delete a file or an empty folder'],
  ['eeprom_read_u8','offset','Storage','Read an EEPROM byte (offset 0..127)'],
  ['eeprom_write_u8','offset, value','Storage','Write an EEPROM byte (offset 0..127)'],
  ['esp_online','','ESP32 & Sensors','1 if the ESP32-C6 companion is connected'],
  ['esp_hud_clear','','ESP32 & Sensors','Switch the companion screen to Game HUD and clear it'],
  ['esp_hud_text','line, "text"','ESP32 & Sensors','Set HUD line 0..5'],
  ['esp_hud_num','line, "label", value','ESP32 & Sensors','Set HUD line 0..5 to label + number'],
  ['esp_hud_restore','','ESP32 & Sensors','Return the companion to its normal pages'],
  ['esp_led','r, g, b','ESP32 & Sensors','Set the companion RGB LED (0..255)'],
  ['temp_mcu','','ESP32 & Sensors','MCU temperature in 0.1 \u00B0C'],
  ['temp_ext','','ESP32 & Sensors','DS18B20 temperature in 0.1 \u00B0C, or SDK_TEMP_INVALID'],
  ['weather_temp','','ESP32 & Sensors','Outdoor Open-Meteo temperature in 0.1 \u00B0C, or SDK_TEMP_INVALID'],
  ['led_toggle','id','System','Toggle a board LED (default 1)'],
  ['exit','','System','Quit the script and return to the OS']
];
const CS_ALIASES = {
  sys_clear_screen:'clear_screen', sys_lcd_fill_rect:'fill_rect', sys_draw_pixel:'draw_pixel',
  sys_draw_text:'draw_text', sys_draw_num:'draw_num', sdk_input_poll:'input_poll', sys_get_keys:'get_keys',
  sdk_frame_sync:'frame_sync', sys_sleep_ms:'sleep_ms', delay_ms:'sleep_ms', sys_delay_ms:'sleep_ms',
  sys_get_tick:'get_tick', sys_beep:'beep', sys_play_tone:'play_tone', random:'rand', sys_random:'rand',
  sys_random_range:'rand_range', sys_file_open:'file_open', sys_file_read:'file_read', sys_file_write:'file_write',
  sys_file_seek:'file_seek', sys_file_size:'file_size', sys_file_close:'file_close', sys_file_remove:'file_remove',
  sys_esp_is_online:'esp_online', sys_esp_hud_clear:'esp_hud_clear', sys_esp_hud_text:'esp_hud_text',
  sys_esp_hud_restore:'esp_hud_restore', sys_esp_set_led:'esp_led', sys_get_mcu_temp_deci:'temp_mcu',
  sys_get_ext_temp_deci:'temp_ext', sys_get_weather_temp_deci:'weather_temp', sys_led_toggle:'led_toggle',
  sys_exit:'exit', exit_app:'exit'
};
const CS_CONSTS = ('TRUE FALSE true false NULL SCREEN_W SCREEN_H SDK_SCREEN_W SDK_SCREEN_H ' +
  'KEY_LEFT KEY_RIGHT KEY_UP KEY_DOWN KEY_A KEY_B KEY_X KEY_EXIT KEY_START ' +
  'SDK_PAD_UP SDK_PAD_DOWN SDK_PAD_LEFT SDK_PAD_RIGHT SDK_PAD_A SDK_PAD_B SDK_PAD_X SDK_PAD_Y SDK_PAD_L1 SDK_PAD_R1 ' +
  'SDK_PAD_L2 SDK_PAD_R2 SDK_PAD_START SDK_PAD_SELECT SDK_PAD_L3 SDK_PAD_R3 ' +
  'COLOR_BLACK COLOR_WHITE COLOR_CYAN COLOR_NEON_GREEN COLOR_GREEN COLOR_AMBER COLOR_ORANGE COLOR_RED COLOR_BLUE ' +
  'COLOR_MAGENTA COLOR_YELLOW COLOR_DARK_BG COLOR_PANEL_BG COLOR_GRID_LINE ' +
  'SDK_FILE_READ SDK_FILE_WRITE SDK_FILE_CREATE SDK_FILE_TRUNC SDK_FILE_APPEND SDK_TEMP_INVALID').split(' ');
const CS_KEYWORDS = ('if else for while do break continue return int char void bool const static unsigned ' +
  'int8_t int16_t int32_t uint8_t uint16_t uint32_t define').split(' ');
const CS_SNIPPETS = [
  ['main', 'void main() {\n    clear_screen(COLOR_DARK_BG);\n    while (1) {\n        input_poll();\n        if (input_pressed(KEY_EXIT)) break;\n        $0\n        frame_sync(16);\n    }\n}', 'Game loop template'],
  ['for', 'for (int i = 0; i < $0; i++) {\n}', 'for loop'],
  ['while', 'while ($0) {\n}', 'while loop'],
  ['if', 'if ($0) {\n}', 'if block']
];

function constDoc(n){
  if(n.startsWith('KEY_')) return 'Key bit for input_pressed() / input_held() / get_keys()';
  if(n.startsWith('SDK_PAD_')) return 'Gamepad button bit for pad_buttons()';
  if(n.startsWith('COLOR_')) return 'RGB565 color';
  if(n.startsWith('SDK_FILE_')) return 'file_open() flag';
  if(n.indexOf('SCREEN_') >= 0) return 'Screen size in pixels (320 x 240)';
  if(n === 'SDK_TEMP_INVALID') return 'Returned when a temperature source is absent';
  return 'Built-in constant';
}

const CS_FN_MAP = {};
const CS_ITEMS = [];
CS_FUNCS.forEach(f => {
  CS_FN_MAP[f[0]] = { name:f[0], params:f[1], group:f[2], doc:f[3] };
  CS_ITEMS.push({ kind:'fn', name:f[0], params:f[1], hint:'(' + f[1] + ')', doc:f[3], rank:0 });
});
Object.keys(CS_ALIASES).forEach(a => {
  const t = CS_FN_MAP[CS_ALIASES[a]];
  CS_FN_MAP[a] = t;
  CS_ITEMS.push({ kind:'fn', name:a, params:t.params, hint:'(' + t.params + ')', doc:'SDK alias of ' + t.name + '(): ' + t.doc, rank:5, alias:true });
});
CS_CONSTS.forEach(c => CS_ITEMS.push({ kind:'const', name:c, hint:'const', doc:constDoc(c), rank:2 }));
CS_KEYWORDS.forEach(k => CS_ITEMS.push({ kind:'kw', name:k, hint:'keyword', doc:'C keyword', rank:3 }));
CS_SNIPPETS.forEach(s => CS_ITEMS.push({ kind:'snip', name:s[0], body:s[1], hint:'snippet', doc:s[2], rank:4 }));
const CS_BUILTIN_NAMES = new Set(CS_ITEMS.map(i => i.name));
const AC_ICONS = { fn:['\u0192','#00e5ff'], var:['v','#c792ea'], const:['#','#ffae00'], kw:['k','#ff7b72'], snip:['\u29C9','#00ff66'] };

const acPop = document.getElementById('acPop');
const acListEl = document.getElementById('acList');
const acDocEl = document.getElementById('acDoc');
const sigTip = document.getElementById('sigTip');
const acMirror = document.createElement('div');
document.body.appendChild(acMirror);
let acItems = [], acSel = 0, acOpen = false, acRange = null, acSuppress = false;

function escHtml(s){
  return String(s).replace(/[&<>"]/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]));
}

function acEnabled(){
  const f = (curFile || '').toLowerCase();
  return f === '' || f.endsWith('.c') || f.endsWith('.h');
}

/* True when pos is inside a string literal or comment (simple single-line + block comment check) */
function acInStringOrComment(text, pos){
  const lastOpen = text.lastIndexOf('/*', pos - 1);
  if(lastOpen >= 0 && text.lastIndexOf('*/', pos - 1) < lastOpen) return true;
  const ls = text.lastIndexOf('\n', pos - 1) + 1;
  const line = text.substring(ls, pos);
  let inStr = false;
  for(let i = 0; i < line.length; i++){
    const c = line[i];
    if(inStr){
      if(c === '\\'){ i++; continue; }
      if(c === '"') inStr = false;
    } else if(c === '"' || c === "'"){
      inStr = (c === '"');
      if(c === "'"){ const e = line.indexOf("'", i + 1); if(e < 0) return true; i = e; }
    } else if(c === '/' && line[i + 1] === '/'){
      return true;
    }
  }
  return inStr;
}

function caretXY(pos){
  const cs = getComputedStyle(ta);
  ['boxSizing','width','borderTopWidth','borderRightWidth','borderBottomWidth','borderLeftWidth',
   'paddingTop','paddingRight','paddingBottom','paddingLeft','fontStyle','fontVariant','fontWeight',
   'fontSize','lineHeight','fontFamily','letterSpacing','wordSpacing','tabSize','textIndent'].forEach(p => acMirror.style[p] = cs[p]);
  const s = acMirror.style;
  s.position = 'absolute'; s.visibility = 'hidden'; s.whiteSpace = 'pre-wrap'; s.wordWrap = 'break-word';
  s.top = '0'; s.left = '-9999px'; s.overflow = 'hidden'; s.height = 'auto';
  s.boxSizing = 'border-box'; s.width = ta.clientWidth + 'px'; s.borderWidth = '0';
  acMirror.textContent = ta.value.substring(0, pos);
  const sp = document.createElement('span');
  sp.textContent = ta.value.substring(pos, pos + 1) || '.';
  acMirror.appendChild(sp);
  const lh = parseFloat(cs.lineHeight) || (parseFloat(cs.fontSize) * 1.45) || 18;
  return { x: ta.offsetLeft + sp.offsetLeft - ta.scrollLeft, y: ta.offsetTop + sp.offsetTop - ta.scrollTop, h: lh };
}

function acCurrentWord(){
  const v = ta.value, pos = ta.selectionStart;
  let s = pos, e = pos;
  while(s > 0 && /\w/.test(v[s - 1])) s--;
  while(e < v.length && /\w/.test(v[e])) e++;
  return { start:s, end:e, prefix:v.substring(s, pos) };
}

function acMatchScore(name, prefix){
  if(name.startsWith(prefix)) return 0;
  const ln = name.toLowerCase(), lp = prefix.toLowerCase();
  if(ln.startsWith(lp)) return 1;
  if(ln.indexOf('_' + lp) > 0) return 2;
  return -1;
}

function acUpdate(force){
  if(!acEnabled() || ta.selectionStart !== ta.selectionEnd){ acHide(); return; }
  const w = acCurrentWord();
  const v = ta.value;
  if(acInStringOrComment(v, w.start)){ acHide(); return; }
  if(!force && w.prefix.length < 2){ acHide(); return; }
  if(w.prefix && /^\d/.test(w.prefix)){ acHide(); return; }

  const locals = new Set();
  const other = (v.substring(0, w.start) + ' ' + v.substring(w.end))
    .replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\\n])*"?|'(?:\\.|[^'\\\n])*'?/g, ' ');
  const re = /\b[A-Za-z_]\w{2,}\b/g;
  let m;
  while((m = re.exec(other)) !== null){
    if(!CS_BUILTIN_NAMES.has(m[0]) && m[0] !== w.prefix) locals.add(m[0]);
  }
  const pool = CS_ITEMS.concat(Array.from(locals).map(n => ({ kind:'var', name:n, hint:'in file', doc:'Identifier used in this file', rank:1 })));

  const found = [];
  pool.forEach(it => {
    if(!w.prefix){ if(!it.alias) found.push({ it, sc:0 }); return; }
    const sc = acMatchScore(it.name, w.prefix);
    if(sc < 0 || (sc === 2 && it.alias)) return;
    found.push({ it, sc });
  });
  found.sort((a, b) => (a.sc - b.sc) || (a.it.rank - b.it.rank) || (a.it.name.length - b.it.name.length) || a.it.name.localeCompare(b.it.name));
  acItems = found.slice(0, 40).map(f => f.it);

  if(acItems.length === 0 || (!force && acItems.every(i => i.name === w.prefix))){ acHide(); return; }
  acRange = { start:w.start, end:w.end, prefix:w.prefix };
  acSel = 0;
  acOpen = true;
  acRender();
  acPosition();
}

function acRender(){
  const p = acRange ? acRange.prefix.length : 0;
  acListEl.innerHTML = acItems.map((it, i) => {
    const ic = AC_ICONS[it.kind];
    const nm = it.name.toLowerCase().startsWith(acRange.prefix.toLowerCase())
      ? '<b>' + escHtml(it.name.substring(0, p)) + '</b>' + escHtml(it.name.substring(p))
      : escHtml(it.name);
    return '<div class="ac-item' + (i === acSel ? ' sel' : '') + '" data-i="' + i + '">' +
      '<span class="ac-ico" style="color:' + ic[1] + (it.alias ? ';opacity:0.55' : '') + '">' + ic[0] + '</span>' +
      '<span class="ac-name">' + nm + '</span><span class="ac-hint">' + escHtml(it.hint) + '</span></div>';
  }).join('');
  const cur = acItems[acSel];
  acDocEl.innerHTML = cur
    ? (cur.kind === 'fn' ? '<b style="color:var(--accent)">' + escHtml(cur.name + '(' + cur.params + ')') + '</b><br>' : '') + escHtml(cur.doc) +
      '<div style="color:var(--muted);margin-top:2px">\u2191\u2193 select \u00B7 Tab/Enter insert \u00B7 Esc close</div>'
    : '';
  const selEl = acListEl.querySelector('.ac-item.sel');
  if(selEl) selEl.scrollIntoView({ block:'nearest' });
}

function acPosition(){
  if(!acOpen || !acRange) return;
  const c = caretXY(acRange.start);
  const wrap = document.getElementById('editorWrap');
  acPop.style.display = 'block';
  const pw = acPop.offsetWidth, ph = acPop.offsetHeight;
  let left = Math.max(4, Math.min(c.x, wrap.clientWidth - pw - 4));
  let top = c.y + c.h + 2;
  if(top + ph > wrap.clientHeight - 4 && c.y - ph - 2 > 0) top = c.y - ph - 2;
  acPop.style.left = left + 'px';
  acPop.style.top = top + 'px';
}

function acHide(){
  acOpen = false;
  acItems = [];
  acPop.style.display = 'none';
}

function acInsert(text, start, end){
  ta.focus();
  ta.setSelectionRange(start, end);
  acSuppress = true;
  const ok = document.queryCommandSupported && document.queryCommandSupported('insertText') &&
             document.execCommand('insertText', false, text);
  if(!ok){
    ta.setRangeText(text, start, end, 'end');
    ta.dispatchEvent(new Event('input'));
  }
  acSuppress = false;
}

function acAccept(idx){
  const it = acItems[idx];
  if(!it || !acRange) return;
  const { start, end } = acRange;
  const v = ta.value;
  let text = it.name, caret = null;
  if(it.kind === 'fn'){
    if(!/^\s*\(/.test(v.substring(end))){
      text = it.name + '()';
      caret = it.params ? it.name.length + 1 : text.length;
    }
  } else if(it.kind === 'snip'){
    const ls = v.lastIndexOf('\n', start - 1) + 1;
    const indent = v.substring(ls, start).match(/^\s*/)[0];
    text = it.body.replace(/\n/g, '\n' + indent);
    caret = text.indexOf('$0');
    text = text.replace('$0', '');
    if(caret < 0) caret = text.length;
  }
  acInsert(text, start, end);
  if(caret !== null) ta.setSelectionRange(start + caret, start + caret);
  acHide();
  sigUpdate();
  updateCursorInfo();
}

function acHandleKey(e){
  if((e.ctrlKey || e.metaKey) && (e.code === 'Space' || e.key === ' ')){
    e.preventDefault();
    acUpdate(true);
    return true;
  }
  if(!acOpen){
    if(e.key === 'Escape' && sigTip.style.display === 'block'){ e.preventDefault(); sigHide(); return true; }
    return false;
  }
  const n = acItems.length;
  switch(e.key){
    case 'ArrowDown': acSel = (acSel + 1) % n; break;
    case 'ArrowUp':   acSel = (acSel - 1 + n) % n; break;
    case 'PageDown':  acSel = Math.min(n - 1, acSel + 8); break;
    case 'PageUp':    acSel = Math.max(0, acSel - 8); break;
    case 'Enter':
    case 'Tab':       e.preventDefault(); acAccept(acSel); return true;
    case 'Escape':    e.preventDefault(); acHide(); return true;
    default:          return false;
  }
  e.preventDefault();
  acRender();
  return true;
}

/* Finds the innermost unclosed call before pos: returns { name, arg } or null */
function sigFindCall(text, pos){
  let depth = 0, commas = 0, inStr = false;
  const lim = Math.max(0, pos - 600);
  for(let i = pos - 1; i >= lim; i--){
    const c = text[i];
    if(c === '"' && text[i - 1] !== '\\'){ inStr = !inStr; continue; }
    if(inStr) continue;
    if(c === ')'){ depth++; }
    else if(c === '('){
      if(depth === 0){
        let j = i - 1;
        while(j >= 0 && text[j] === ' ') j--;
        const e = j + 1;
        while(j >= 0 && /\w/.test(text[j])) j--;
        const name = text.substring(j + 1, e);
        return name ? { name, arg:commas } : null;
      }
      depth--;
    }
    else if(c === ',' && depth === 0){ commas++; }
    else if(c === ';' || c === '{' || c === '}'){ return null; }
  }
  return null;
}

function sigHide(){ sigTip.style.display = 'none'; }

function sigUpdate(){
  if(!acEnabled() || ta.selectionStart !== ta.selectionEnd){
    sigHide();
    return;
  }
  const call = sigFindCall(ta.value, ta.selectionStart);
  const fn = call ? CS_FN_MAP[call.name] : null;
  if(!fn){ sigHide(); return; }
  const params = fn.params ? fn.params.split(', ') : [];
  const shown = params.map((p, i) => i === call.arg ? '<b>' + escHtml(p) + '</b>' : escHtml(p)).join(', ');
  sigTip.innerHTML = escHtml(call.name) + '(' + shown + ')' + ' <span style="color:var(--muted)">\u2014 ' + escHtml(fn.doc) + '</span>';
  const c = caretXY(ta.selectionStart);
  const wrap = document.getElementById('editorWrap');
  sigTip.style.display = 'block';
  const tw = sigTip.offsetWidth;
  sigTip.style.left = Math.min(Math.max(4, c.x - 8), Math.max(4, wrap.clientWidth - tw - 4)) + 'px';
  sigTip.style.top = Math.max(2, c.y - sigTip.offsetHeight - 3) + 'px';
}

ta.addEventListener('input', (e) => {
  if(acSuppress) return;
  const t = e.inputType || '';
  if(t.startsWith('insert') && e.data && /^\w$/.test(e.data)) acUpdate(false);
  else if(t.startsWith('delete') && acOpen) acUpdate(false);
  else acHide();
  sigUpdate();
});
ta.addEventListener('keyup', (e) => {
  if(['ArrowLeft','ArrowRight','Home','End'].includes(e.key) || (!acOpen && ['ArrowUp','ArrowDown','PageUp','PageDown'].includes(e.key))){
    acHide();
    sigUpdate();
  }
});
ta.addEventListener('click', () => { acHide(); sigUpdate(); });
ta.addEventListener('scroll', () => { acPosition(); if(sigTip.style.display === 'block') sigUpdate(); });
ta.addEventListener('blur', () => { acHide(); sigHide(); });
acPop.addEventListener('mousedown', (e) => {
  e.preventDefault();
  const row = e.target.closest('.ac-item');
  if(row) acAccept(parseInt(row.dataset.i, 10));
});

function renderApiHelp(){
  const groups = {};
  CS_FUNCS.forEach(f => { (groups[f[2]] = groups[f[2]] || []).push(f[0] + '(' + f[1] + ')'); });
  let h = '<b style="color:var(--accent)">C-Script Built-in Functions</b> ' +
          '<span style="color:var(--muted)">(type 2+ letters or Ctrl+Space for autocomplete; Tab/Enter inserts)</span><br>';
  Object.keys(groups).forEach(g => { h += '\u2022 <b>' + escHtml(g) + ':</b> ' + groups[g].map(escHtml).join('; ') + '<br>'; });
  h += '\u2022 <b>Constants:</b> KEY_*, SDK_PAD_*, COLOR_*, SDK_FILE_*, SCREEN_W / SCREEN_H, SDK_TEMP_INVALID, TRUE / FALSE / NULL<br>';
  h += '\u2022 <b>SDK aliases:</b> sys_* / sdk_* names (e.g. sys_draw_text, sdk_frame_sync) are accepted too.';
  document.getElementById('helpBox').innerHTML = h;
}
renderApiHelp();

loadDir('/');
pollStatus();
setInterval(pollStatus, 1500);
</script>
</body>
</html>)rawliteral";

#endif /* WEB_UI_H */
