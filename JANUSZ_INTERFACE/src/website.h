#ifndef WEBSITE_H
#define WEBSITE_H
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>JANUSZ</title>
  <style>
    html, body { margin:0; background:black; overflow:hidden; height:100%; font-family:sans-serif; color:white; }
    canvas { display:block; }
    #fullscreen-btn { position:fixed; right:10px; top:10px; background:rgba(255,255,255,0.1); color:white; border:1px solid white; border-radius:8px; padding:8px 12px; font-size:14px; cursor:pointer; z-index:10; }
    #status { position:fixed; top:10px; left:10px; font-size:14px; z-index:20; }
  </style>
</head>
<body>
<div id="status"></div>
<canvas id="canvas"></canvas>
<button id="fullscreen-btn">Go Fullscreen</button>

<script>
  const NUM_SLIDERS = 30;
  const canvas = document.getElementById('canvas');
  const ctx = canvas.getContext('2d');
  const statusEl = document.getElementById('status');

  let sliders = [];
  let center = { x:0, y:0 };
  let innerRadius = 50, outerRadius = 0;
  let draggingIndex = -1, lastSent = 0;

  let token = localStorage.getItem('token') || '';
  let timeLeft = 0;
  let isActive = false;

  // ---- Heartbeat + Release ----
  let heartbeatInterval = null;
  function startHeartbeat(){
    if (heartbeatInterval) clearInterval(heartbeatInterval);
    heartbeatInterval = setInterval(() => {
      if (token && isActive) {
        fetch("/heartbeat", { method:"POST", headers:{ "X-Token": token } }).catch(()=>{});
      }
    }, 5000);
  }
  function stopHeartbeat(){
    if (heartbeatInterval) { clearInterval(heartbeatInterval); heartbeatInterval = null; }
  }
  // Try to release the session immediately when the page/tab is closing.
  function sendReleaseBeacon(){
    if (!token) return;
    try {
      navigator.sendBeacon("/release?t="+encodeURIComponent(token), "");
    } catch (e) { /* ignore */ }
  }
  window.addEventListener('unload', sendReleaseBeacon);
  window.addEventListener('pagehide', sendReleaseBeacon); // iOS Safari

  // ---- Session polling ----
  let lastSessionPoll = 0;
async function ensureToken() {
  try {
    const r = await fetch("/session", { headers: { "X-Token": token || "" } });
    const j = await r.json();
    if (j.token) token = j.token;

    const prevActive = isActive;
    isActive = !!j.active;
    timeLeft = j.timeLeft || 0;
    localStorage.setItem("token", token);

    if (isActive) startHeartbeat();
    else stopHeartbeat();

// Force refresh if role changed
if (prevActive !== isActive) {
  if (isActive) {
    // became active
    loadSliders().then(draw);
  } else {
    // became inactive
    draggingIndex = -1;    // stop manipulating
    draw();                // force immediate grey UI
    loadSliders().then(draw); // refresh values but keep grey
  }
}
  } catch (e) {
    console.error("session error", e);
  }
}


  // ---- Sliders IO ----
  async function loadSliders() {
    try {
      const r = await fetch("/get_sliders", { headers: { "X-Token": token || "" } });
      if (!r.ok) return;
      const values = await r.json();
      for (let i=0;i<NUM_SLIDERS;i++) sliders[i] = values[i]/255;
      draw();
    } catch (e) { console.error(e); }
  }

  function resize(){
     canvas.width=innerWidth;
      canvas.height=innerHeight;
       center.x=canvas.width/2;
        center.y=canvas.height/2;
         outerRadius=Math.min(canvas.width,canvas.height)/2-30;
          draw(); }
  function setup(){
     for (let i=0;i<NUM_SLIDERS;i++)
      sliders[i]=0.5; resize();
     ensureToken().then(loadSliders); }

  function getHandlePos(i,v){const a=(2*Math.PI*i)/NUM_SLIDERS;const r=innerRadius+v*(outerRadius-innerRadius);return {x:center.x+r*Math.cos(a),y:center.y+r*Math.sin(a)};}
  function drawSpline(points){ctx.beginPath();for(let i=0;i<points.length;i++){const p0=points[(i-1+points.length)%points.length],p1=points[i],p2=points[(i+1)%points.length],p3=points[(i+2)%points.length];const cp1x=p1.x+(p2.x-p0.x)/6,cp1y=p1.y+(p2.y-p0.y)/6,cp2x=p2.x-(p3.x-p1.x)/6,cp2y=p2.y-(p3.y-p1.y)/6;if(i===0)ctx.moveTo(p1.x,p1.y);ctx.bezierCurveTo(cp1x,cp1y,cp2x,cp2y,p2.x,p2.y);}ctx.closePath();ctx.strokeStyle=isActive?'white':'darkgray';ctx.lineWidth=2;ctx.stroke();}
  function drawHandles(points){const colors=["red","orange","blue","yellow","magenta"];for(let i=0;i<points.length;i++){ctx.beginPath();ctx.arc(points[i].x,points[i].y,6,0,Math.PI*2);ctx.fillStyle=isActive?colors[Math.floor(i/6)]:"darkgray";ctx.fill();}}
  
  function draw(){
    ctx.clearRect(0,0,canvas.width,canvas.height);
    const points=sliders.map((v,i)=>getHandlePos(i,v));
    drawSpline(points);drawHandles(points);}

  function getNearestHandle(x,y){const th=20*20;for(let i=0;i<NUM_SLIDERS;i++){const p=getHandlePos(i,sliders[i]);const dx=p.x-x,dy=p.y-y;if(dx*dx+dy*dy<th)return i;}return -1;}
  function updateHandle(i,x,y){if(!isActive)return;const a=(2*Math.PI*i)/NUM_SLIDERS,dx=x-center.x,dy=y-center.y;const rx=Math.cos(a),ry=Math.sin(a);const proj=dx*rx+dy*ry;const r=Math.max(innerRadius,Math.min(outerRadius,proj));sliders[i]=(r-innerRadius)/(outerRadius-innerRadius);draw();sendSliderValues();}
  function getEventPos(e){const r=canvas.getBoundingClientRect();return e.touches?{x:e.touches[0].clientX-r.left,y:e.touches[0].clientY-r.top}:{x:e.clientX-r.left,y:e.clientY-r.top};}
  
  function onStart(e){
    if (!isActive) { e.preventDefault(); return; }
    
    const {x,y}=getEventPos(e);
    draggingIndex=getNearestHandle(x,y);
    if(draggingIndex!==-1){updateHandle(draggingIndex,x,y);
      e.preventDefault();}}
 
      function onMove(e){
        if (!isActive) { e.preventDefault(); return; }
        if(draggingIndex===-1)return;const {x,y}=getEventPos(e);updateHandle(draggingIndex,x,y);e.preventDefault();}
  function onEnd(){draggingIndex=-1;}

  canvas.addEventListener('mousedown',onStart);canvas.addEventListener('mousemove',onMove);canvas.addEventListener('mouseup',onEnd);canvas.addEventListener('mouseleave',onEnd);
  canvas.addEventListener('touchstart',onStart,{passive:false});canvas.addEventListener('touchmove',onMove,{passive:false});canvas.addEventListener('touchend',onEnd);
  window.addEventListener('resize',resize);

  function sendSliderValues(){
    if(!isActive) return;
    const now=Date.now();
    if(now-lastSent<50) return;
    lastSent=now;
    const values=sliders.map(v=>Math.round(v*255));
    fetch("/sliders",{method:"POST",headers:{"Content-Type":"application/json","X-Token":token||""},body:JSON.stringify(values)}).catch(()=>{});
  }

  // ---- Status + background refresh ----
  function updateStatus(){
    // Poll /session once per second while waiting to catch early handoff
    const now = Date.now();
    if (!isActive && now - lastSessionPoll >= 1000) {
      lastSessionPoll = now;
      ensureToken(); // will flip us to active immediately if server promoted us
    }

    if (isActive) {
      if (timeLeft > 0) {
        statusEl.textContent = "someone else wants to talk to me. you have " + timeLeft + " seconds left";
        timeLeft--;
      } else {
        statusEl.textContent = "";
      }
    } else {
      if (timeLeft > 0) {
        statusEl.textContent = "i am talking to somebody else now. try again in " + timeLeft + "s";
        timeLeft--;
      } else {
        statusEl.textContent = "";
      }
    }

    // Inactive users refresh slider values periodically (read-only view)
    if (!isActive && (now % 2000 < 100)) {
      loadSliders();
    }
  }
  setInterval(updateStatus, 1000);

  // Initial load
  setup();
</script>
</body>
</html>
)rawliteral";

#endif
