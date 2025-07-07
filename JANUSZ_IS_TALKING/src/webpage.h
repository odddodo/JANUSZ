#ifndef webpage_h
#define webpage_h
#include <ESPAsyncWebServer.h>

// HTML page stored in flash memory
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>ESP32 Color Wheel</title>
  <style>
    html, body {
      margin: 0; 
      padding: 0; 
      overflow: hidden; 
      background: #000; 
      height: 100%;
      width: 100%;
      touch-action: none;
      -webkit-user-select: none; /* Disable text selection on mobile */
      user-select: none;
      position: relative;
    }
    canvas {
      position: absolute;
      top: 0; left: 0;
      width: 100vw;
      height: 100vh;
      touch-action: none;
      -webkit-touch-callout: none; /* Disable long press on iOS */
    }
  </style>
</head>
<body>
  <canvas id="wheel"></canvas>
  <canvas id="overlay"></canvas>

  <script>
    const wheelCanvas = document.getElementById("wheel");
    const wheelCtx = wheelCanvas.getContext("2d");
    const overlayCanvas = document.getElementById("overlay");
    const overlayCtx = overlayCanvas.getContext("2d");
    
    const rippleCount = 3;               // Number of saturation ripples
const phase = 1;                     // Phase in radians
const innerRadiusRatio = 0.2;

    let w, h, cx, cy, radius;
    let selected = { x: 0, y: 0, r: 255, g: 0, b: 0, s:0};
    let isTouching = false;

    function resizeCanvas() {
      w = wheelCanvas.width = overlayCanvas.width = window.innerWidth;
      h = wheelCanvas.height = overlayCanvas.height = window.innerHeight;
      cx = w / 2;
      cy = h / 2;
      radius = Math.min(cx, cy) * 0.95;
      drawWheel();
      drawDot();
    }
    function triangleWave(t, count, phase = 0) {
  const shiftedT = (t + phase / (2 * Math.PI / count)) % 1;
  return 1 - Math.abs((shiftedT * count % 1) * 2 - 1);
}
function drawWheel() {
  const img = wheelCtx.createImageData(w, h);
  const innerRadius = radius * innerRadiusRatio;

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const dx = x - cx;
      const dy = y - cy;
      const dist = Math.sqrt(dx * dx + dy * dy);

      if (dist >= innerRadius && dist <= radius) {
        const angle = Math.atan2(dy, dx);
        const hue = (angle * 180 / Math.PI + 360) % 360;

        const t = (dist - innerRadius) / (radius - innerRadius); // Normalized 0–1
        const value = 1 - t;
        const saturation = triangleWave(t, rippleCount, phase);

        const [r, g, b] = hsvToRgb(hue, saturation, value);
        const i = (y * w + x) * 4;
        img.data[i] = r;
        img.data[i + 1] = g;
        img.data[i + 2] = b;
        img.data[i + 3] = 255;
      }
    }
  }

  wheelCtx.putImageData(img, 0, 0);
}








    function drawDot() {
      overlayCtx.clearRect(0, 0, w, h);
      overlayCtx.beginPath();
      overlayCtx.arc(selected.x, selected.y, 10, 0, 2 * Math.PI);
      overlayCtx.strokeStyle = "#fff";
      overlayCtx.lineWidth = 3;
      overlayCtx.shadowColor = "rgba(255,255,255,0.7)";
      overlayCtx.shadowBlur = 8;
      overlayCtx.stroke();
    }

    function hsvToRgb(h, s, v) {
      let f = (n, k = (n + h / 60) % 6) =>
        v - v * s * Math.max(Math.min(k, 4 - k, 1), 0);
      return [f(5) * 255, f(3) * 255, f(1) * 255].map(x => Math.round(x));
    }

function handlePointer(e) {
  const rect = wheelCanvas.getBoundingClientRect();
  const x = e.clientX - rect.left;
  const y = e.clientY - rect.top;
  const dx = x - cx;
  const dy = y - cy;
  const dist = Math.sqrt(dx * dx + dy * dy);

  const innerRadius = radius * innerRadiusRatio;
  if (dist < innerRadius || dist > radius) return;

  const angle = Math.atan2(dy, dx);
  const hue = (angle * 180 / Math.PI + 360) % 360;

  const t = (dist - innerRadius) / (radius - innerRadius);
  const value = 1 - t;
  const saturation = triangleWave(t, rippleCount, phase);

  const [r, g, b] = hsvToRgb(hue, saturation, value);
selected = { x, y, r, g, b, s:saturation };

  drawDot();
  sendColorThrottled(r, g, b, saturation);
}



    // Throttle function: ensures fetch calls max once every delay ms
    function throttle(fn, delay) {
      let lastCall = 0;
      return (...args) => {
        const now = Date.now();
        if (now - lastCall >= delay) {
          lastCall = now;
          fn(...args);
        }
      };
    }

const sendColorThrottled = throttle((r, g, b, s) => {
  fetch(`/setcolor?r=${r}&g=${g}&b=${b}&s=${s}`).catch(() => {
    // Optional: ignore fetch errors
  });
}, 100);


    overlayCanvas.addEventListener("pointerdown", e => {
      isTouching = true;
      handlePointer(e);
    });

    overlayCanvas.addEventListener("pointermove", e => {
      if (isTouching) handlePointer(e);
    });

    overlayCanvas.addEventListener("pointerup", () => {
      isTouching = false;
    });

    overlayCanvas.addEventListener("pointercancel", () => {
      isTouching = false;
    });

    window.addEventListener("resize", resizeCanvas);
    resizeCanvas();
  </script>
</body>
</html>
)rawliteral";


#endif
