// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

// 键盘事件：按下与抬起
document.addEventListener("keydown", (event) => {
     playerInput.setKeyStatus(event.key, true);
     if (event.key === 'e') {
          playerInput.autoFire = !playerInput.autoFire;
     }
});

document.addEventListener("keyup", (event) => {
     playerInput.setKeyStatus(event.key, false);
});

// 鼠标事件：按下与抬起
canvas.addEventListener("mousedown", (event) => {
     if (event.button === 0) {  // 0 是左键
          playerInput.setKeyStatus('left', true);
     } else if (event.button === 2) {  // 2 是右键
          playerInput.setKeyStatus('right', true);
     }
});

window.addEventListener("mouseup", (event) => {
     if (event.button === 0) {  // 0 是左键
          playerInput.setKeyStatus('left', false);
     } else if (event.button === 2) {  // 2 是右键
          playerInput.setKeyStatus('right', false);
     }
});

//鼠标移动
canvas.addEventListener("mousemove", (event) => {
     const rect = canvas.getBoundingClientRect();
     // 设备的像素比率
     const scaleX = canvas.width / rect.width;
     const scaleY = canvas.height / rect.height;

     //鼠标在 canvas 上的实际位置
     playerInput.mouseXInScreen = (event.clientX - rect.left) * scaleX;
     playerInput.mouseYInScreen = (event.clientY - rect.top) * scaleY;
});

window.addEventListener('blur', function () {
     playerInput.onBlur();
});