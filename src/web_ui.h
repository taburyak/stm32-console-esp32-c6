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
.editor-wrap{flex:1;display:flex;flex-direction:column;min-height:420px}
#codeArea{flex:1;width:100%;background:#090d14;color:#e8f0fe;border:none;padding:12px;font-family:'Cascadia Code',Consolas,'Courier New',monospace;font-size:0.9rem;line-height:1.45;resize:none;outline:none;tab-size:4}
.img-viewer{flex:1;display:none;flex-direction:column;align-items:center;justify-content:center;gap:14px;padding:18px;background:#090d14;min-height:420px;overflow:auto}
.img-stage{padding:12px;border-radius:10px;border:1px dashed var(--border);background:repeating-conic-gradient(#121824 0% 25%, #0c111a 0% 50%) 50% / 20px 20px;display:flex;align-items:center;justify-content:center;max-width:100%}
#previewImg{max-width:100%;max-height:58vh;image-rendering:pixelated;border:1px solid var(--border);box-shadow:0 6px 24px rgba(0,0,0,0.6)}
.img-meta{font-family:Consolas,monospace;font-size:0.84rem;color:var(--accent)}
.status-bar{padding:6px 12px;background:var(--card);border-top:1px solid var(--border);font-size:0.78rem;color:var(--muted);display:flex;justify-content:space-between;align-items:center;gap:8px;flex-wrap:wrap}
.prog-wrap{height:4px;background:#090d14;width:100%;overflow:hidden}
.prog-bar{height:100%;width:0%;background:var(--green);transition:width 0.15s}
.help-box{background:#0e1522;border-top:1px solid var(--border);padding:10px 14px;font-size:0.78rem;line-height:1.45;color:#b8c7dc;display:none;max-height:190px;overflow-y:auto;font-family:Consolas,monospace}
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
    <div id="helpBox" class="help-box">
      <b style="color:var(--accent)">C-Script Built-in Functions:</b><br>
      • <b>Graphics:</b> clear_screen(rgb565); fill_rect(x,y,w,h,col); draw_pixel(x,y,col); draw_line(x0,y0,x1,y1,col); draw_rect(x,y,w,h,col); draw_circle(x,y,r,col); fill_circle(x,y,r,col); draw_text(x,y,"str",fg,bg,scale); draw_num(x,y,val,fg,bg,scale,digits); rgb(r,g,b);<br>
      • <b>Input:</b> k = input_poll(); input_pressed(KEY_A | KEY_B | KEY_X | KEY_START | KEY_EXIT | KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN); pad_lx(); pad_ly(); pad_rx(); pad_ry();<br>
      • <b>System & Audio:</b> frame_sync(16); delay_ms(ms); tick_ms(); beep(ms); tone(freq_hz, ms); rand_range(min, max); clamp(val, lo, hi); abs(v); min(a,b); max(a,b);<br>
      • <b>Persistence:</b> save_int("file.dat", val); val = load_int("file.dat", default_val); exit_app();
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

function loadTemplate(){
  if(isDirty && !confirm('Replace editor content with C-Script template?')) return;
  showPaneMode('editor');
  if(!curFile || !curFile.endsWith('.c')) curFile = joinPath(curDir === '/' ? '/games' : curDir, 'web_demo.c');
  document.getElementById('edFileTitle').textContent = '📝 ' + curFile;
  document.getElementById('codeArea').value =
`// Web Studio Live C-Script Demo
void main() {
    int x = 160, y = 120, vx = 3, vy = 2;
    clear_screen(COLOR_DARK_BG);
    while (1) {
        int k = input_poll();
        if (input_pressed(KEY_EXIT)) break;
        fill_circle(x, y, 10, COLOR_DARK_BG);
        x += vx + pad_lx() / 32;
        y += vy + pad_ly() / 32;
        if (x < 14 || x > 305) { vx = -vx; x = clamp(x, 14, 305); beep(15); }
        if (y < 28 || y > 225) { vy = -vy; y = clamp(y, 28, 225); beep(15); }
        draw_text(24, 6, "WEB STUDIO LIVE C-SCRIPT  [ESC: EXIT]", COLOR_CYAN, COLOR_DARK_BG, 1);
        fill_circle(x, y, 10, COLOR_NEON_GREEN);
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

loadDir('/');
pollStatus();
setInterval(pollStatus, 1500);
</script>
</body>
</html>)rawliteral";

#endif /* WEB_UI_H */
