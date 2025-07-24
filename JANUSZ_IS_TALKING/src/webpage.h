#ifndef WEBPAGE_H
#define WEBPAGE_H
#include <ESPAsyncWebServer.h>

#define NUMSLIDERS 42

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Radial Sliders</title>
  <style>
    html, body {
      margin: 0;
      padding: 0;
      width: 100%;
      height: 100%;
      overflow: hidden;
      background: #111;
      -webkit-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      touch-action: none;
    }

    canvas {
      position: fixed;
      top: 0;
      left: 0;
      width: 100vw;
      height: 100vh;
      background: #222;
      display: block;
      touch-action: none;
    }

    #fullscreenBtn, #saveBtn {
      position: fixed;
      right: 10px;
      z-index: 9999;
      padding: 8px 14px;
      font-size: 14px;
      border-radius: 4px;
      cursor: pointer;
      border: 1px solid white;
      color: white;
      background-color: rgba(255, 255, 255, 0.1);
    }

    #fullscreenBtn:hover, #saveBtn:hover {
      background-color: rgba(255, 255, 255, 0.3);
    }

    #fullscreenBtn {
      top: 10px;
    }

    #saveBtn {
      top: 50px;
      border-color: #0f0;
      color: #0f0;
      background-color: rgba(0, 255, 0, 0.1);
    }

    #saveBtn:hover {
      background-color: rgba(0, 255, 0, 0.3);
    }
  </style>
</head>
<body>
  <button id="fullscreenBtn">Enter Fullscreen</button>
  <button id="saveBtn">Save</button>
  <canvas id="canvas"></canvas>

  <script>
    const numSliders = 42;
    const values = new Array(numSliders).fill(128);
    const canvas = document.getElementById("canvas");
    const ctx = canvas.getContext("2d");
    const fullscreenBtn = document.getElementById("fullscreenBtn");
    const saveBtn = document.getElementById("saveBtn");

    let width, height, centerX, centerY, maxRadius, minRadius;
    const handleRadius = 10;
    let draggingIndex = null;

    // WebSocket
    const ws = new WebSocket("ws://" + location.host + "/ws");

    ws.onopen = () => console.log("WebSocket connected.");
    ws.onerror = (e) => console.error("WebSocket error:", e);

    ws.onmessage = function (event) {
      const data = event.data.split(',').map(v => parseInt(v));
      if (data.length === numSliders) {
        for (let i = 0; i < numSliders; i++) {
          values[i] = data[i];
        }
        draw();
      }
    };

    function sendValues(save = false) {
      const payload = values.join(",");
      if (ws.readyState === WebSocket.OPEN) {
        ws.send((save ? "SAVE:" : "") + payload);
      }
    }

    function resizeCanvas() {
      width = window.innerWidth;
      height = window.innerHeight;
      canvas.width = width;
      canvas.height = height;
      centerX = width / 2;
      centerY = height / 2;
      maxRadius = Math.min(width, height) * 0.45;
      minRadius = maxRadius * 0.15;
      draw();
    }

    function polarToCartesian(angle, radius) {
      return {
        x: centerX + radius * Math.cos(angle),
        y: centerY + radius * Math.sin(angle)
      };
    }

    function getHandleAt(x, y) {
      for (let i = 0; i < numSliders; i++) {
        const angle = (i / numSliders) * 2 * Math.PI;
        const valRadius = minRadius + (values[i] / 255) * (maxRadius - minRadius);
        const p = polarToCartesian(angle, valRadius);
        const dx = p.x - x;
        const dy = p.y - y;
        if (dx * dx + dy * dy < handleRadius * handleRadius * 2) {
          return i;
        }
      }
      return null;
    }

    function draw() {
      ctx.clearRect(0, 0, width, height);

      const points = values.map((val, i) => {
        const angle = (i / numSliders) * 2 * Math.PI;
        const radius = minRadius + (val / 255) * (maxRadius - minRadius);
        return polarToCartesian(angle, radius);
      });

      // Draw shape
      ctx.beginPath();
      for (let i = 0; i < points.length; i++) {
        const p0 = points[i];
        const p1 = points[(i + 1) % points.length];
        const cp = {
          x: (p0.x + p1.x) / 2,
          y: (p0.y + p1.y) / 2
        };
        if (i === 0) ctx.moveTo(p0.x, p0.y);
        ctx.quadraticCurveTo(p0.x, p0.y, cp.x, cp.y);
      }
      ctx.closePath();
      ctx.strokeStyle = "#ffffff";
      ctx.lineWidth = 2;
      ctx.stroke();

      // Draw handles
      for (const p of points) {
        ctx.beginPath();
        ctx.arc(p.x, p.y, handleRadius, 0, 2 * Math.PI);
        ctx.fillStyle = "#000";
        ctx.fill();
        ctx.strokeStyle = "#fff";
        ctx.lineWidth = 2;
        ctx.stroke();
      }
    }

    function updateValueFromPointer(x, y, index) {
      const dx = x - centerX;
      const dy = y - centerY;
      const dist = Math.sqrt(dx * dx + dy * dy);
      const clamped = Math.min(Math.max(dist, minRadius), maxRadius);
      const normalized = (clamped - minRadius) / (maxRadius - minRadius);
      values[index] = Math.round(normalized * 255);
      draw();
      sendValues();
    }

    // Interaction
    canvas.addEventListener("mousedown", (e) => {
      const rect = canvas.getBoundingClientRect();
      const x = e.clientX - rect.left;
      const y = e.clientY - rect.top;
      draggingIndex = getHandleAt(x, y);
    });

    canvas.addEventListener("mousemove", (e) => {
      if (draggingIndex !== null) {
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        updateValueFromPointer(x, y, draggingIndex);
      }
    });

    canvas.addEventListener("mouseup", () => { draggingIndex = null; });
    canvas.addEventListener("mouseleave", () => { draggingIndex = null; });

    canvas.addEventListener("touchstart", (e) => {
      const rect = canvas.getBoundingClientRect();
      const x = e.touches[0].clientX - rect.left;
      const y = e.touches[0].clientY - rect.top;
      draggingIndex = getHandleAt(x, y);
    }, { passive: false });

    canvas.addEventListener("touchmove", (e) => {
      if (draggingIndex !== null) {
        e.preventDefault();
        const rect = canvas.getBoundingClientRect();
        const x = e.touches[0].clientX - rect.left;
        const y = e.touches[0].clientY - rect.top;
        updateValueFromPointer(x, y, draggingIndex);
      }
    }, { passive: false });

    canvas.addEventListener("touchend", () => { draggingIndex = null; });

    window.addEventListener("resize", resizeCanvas);
    resizeCanvas();

    // Fullscreen toggle
    fullscreenBtn.addEventListener("click", () => {
      if (!document.fullscreenElement) {
        document.documentElement.requestFullscreen().then(() => {
          fullscreenBtn.innerText = "Exit Fullscreen";
        });
      } else {
        document.exitFullscreen().then(() => {
          fullscreenBtn.innerText = "Enter Fullscreen";
        });
      }
    });

    document.addEventListener("fullscreenchange", () => {
      fullscreenBtn.innerText = document.fullscreenElement ? "Exit Fullscreen" : "Enter Fullscreen";
    });

    // Save button logic
    saveBtn.addEventListener("click", () => {
      sendValues(true);
    });
  </script>
</body>
</html>
)rawliteral";


#endif
