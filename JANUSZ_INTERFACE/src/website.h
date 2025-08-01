#ifndef WEBSITE_H
#define WEBSITE_H
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Radial Sliders</title>
  <style>
    html, body {
      margin: 0;
      background: black;
      overflow: hidden;
      touch-action: none;
      height: 100%;
    }

    canvas {
      display: block;
      touch-action: none;
    }

    #fullscreen-btn {
      position: fixed;
      top: 10px;
      right: 10px;
      background: rgba(255, 255, 255, 0.1);
      color: white;
      border: 1px solid white;
      border-radius: 8px;
      padding: 8px 12px;
      font-size: 14px;
      cursor: pointer;
      z-index: 10;
      transition: background 0.3s;
    }

    #fullscreen-btn:hover {
      background: rgba(255, 255, 255, 0.2);
    }
  </style>
</head>
<body>
<canvas id="canvas"></canvas>
<button id="fullscreen-btn">Go Fullscreen</button>

<script>
  const NUM_SLIDERS = 42;
  const canvas = document.getElementById('canvas');
  const ctx = canvas.getContext('2d');
  const fullscreenBtn = document.getElementById('fullscreen-btn');

  let sliders = [];
  let lastSliderValues = [];
  let center = { x: 0, y: 0 };
  let innerRadius = 50;
  let outerRadius = 0;
  let draggingIndex = -1;
  let lastSent = 0;

  function resize() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    center.x = canvas.width / 2;
    center.y = canvas.height / 2;
    outerRadius = Math.min(canvas.width, canvas.height) / 2 - 30;
    draw();
  }

  function setup() {
    for (let i = 0; i < NUM_SLIDERS; i++) {
      sliders[i] = 0.5;
      lastSliderValues[i] = -1;
    }
    resize();
  }

  function getHandlePos(i, v) {
    const angle = (2 * Math.PI * i) / NUM_SLIDERS;
    const radius = innerRadius + v * (outerRadius - innerRadius);
    return {
      x: center.x + radius * Math.cos(angle),
      y: center.y + radius * Math.sin(angle)
    };
  }

  function drawSpline(points) {
    ctx.beginPath();
    for (let i = 0; i < points.length; i++) {
      const p0 = points[(i - 1 + points.length) % points.length];
      const p1 = points[i];
      const p2 = points[(i + 1) % points.length];
      const p3 = points[(i + 2) % points.length];

      const cp1x = p1.x + (p2.x - p0.x) / 6;
      const cp1y = p1.y + (p2.y - p0.y) / 6;
      const cp2x = p2.x - (p3.x - p1.x) / 6;
      const cp2y = p2.y - (p3.y - p1.y) / 6;

      if (i === 0) ctx.moveTo(p1.x, p1.y);
      ctx.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, p2.x, p2.y);
    }
    ctx.closePath();
    ctx.strokeStyle = 'white';
    ctx.lineWidth = 2;
    ctx.stroke();
  }

  function drawHandles(points) {
    for (const p of points) {
      ctx.beginPath();
      ctx.arc(p.x, p.y, 6, 0, Math.PI * 2);
      ctx.fillStyle = 'white';
      ctx.fill();
    }
  }

  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const points = sliders.map((v, i) => getHandlePos(i, v));
    drawSpline(points);
    drawHandles(points);
  }

  function getNearestHandle(x, y) {
    const threshold = 20 * 20;
    for (let i = 0; i < NUM_SLIDERS; i++) {
      const p = getHandlePos(i, sliders[i]);
      const dx = p.x - x;
      const dy = p.y - y;
      if (dx * dx + dy * dy < threshold) return i;
    }
    return -1;
  }

  function updateHandle(index, x, y) {
    const angle = (2 * Math.PI * index) / NUM_SLIDERS;
    const dx = x - center.x;
    const dy = y - center.y;
    const rayX = Math.cos(angle);
    const rayY = Math.sin(angle);
    const projection = dx * rayX + dy * rayY;
    const r = Math.max(innerRadius, Math.min(outerRadius, projection));
    sliders[index] = (r - innerRadius) / (outerRadius - innerRadius);
    draw();
    sendSliderValues(); // trigger sending updated values
  }

  function getEventPos(evt) {
    const rect = canvas.getBoundingClientRect();
    if (evt.touches) {
      return {
        x: evt.touches[0].clientX - rect.left,
        y: evt.touches[0].clientY - rect.top
      };
    } else {
      return {
        x: evt.clientX - rect.left,
        y: evt.clientY - rect.top
      };
    }
  }

  function onStart(evt) {
    const { x, y } = getEventPos(evt);
    draggingIndex = getNearestHandle(x, y);
    if (draggingIndex !== -1) {
      updateHandle(draggingIndex, x, y);
      evt.preventDefault();
    }
  }

  function onMove(evt) {
    if (draggingIndex === -1) return;
    const { x, y } = getEventPos(evt);
    updateHandle(draggingIndex, x, y);
    evt.preventDefault();
  }

  function onEnd() {
    draggingIndex = -1;
  }

  canvas.addEventListener('mousedown', onStart);
  canvas.addEventListener('mousemove', onMove);
  canvas.addEventListener('mouseup', onEnd);
  canvas.addEventListener('mouseleave', onEnd);

  canvas.addEventListener('touchstart', onStart, { passive: false });
  canvas.addEventListener('touchmove', onMove, { passive: false });
  canvas.addEventListener('touchend', onEnd);

  window.addEventListener('resize', resize);

  fullscreenBtn.addEventListener('click', () => {
    const el = document.documentElement;
    if (!document.fullscreenElement) {
      if (el.requestFullscreen) el.requestFullscreen();
      else if (el.webkitRequestFullscreen) el.webkitRequestFullscreen();
      else if (el.msRequestFullscreen) el.msRequestFullscreen();
    } else {
      if (document.exitFullscreen) document.exitFullscreen();
      else if (document.webkitExitFullscreen) document.webkitExitFullscreen();
      else if (document.msExitFullscreen) document.msExitFullscreen();
    }
  });

  function sendSliderValues() {
    const now = Date.now();
    if (now - lastSent < 50) return; // throttle to max 20 fps
    lastSent = now;
    for (let i = 0; i < NUM_SLIDERS; i++) {
      const val = Math.round(sliders[i] * 100);
      if (val !== lastSliderValues[i]) {
        lastSliderValues[i] = val;
        fetch(`/slider?i=${i}&v=${val}`).catch(console.error);
      }
    }
  }

  setup();
</script>
</body>
</html>
)rawliteral";


#endif