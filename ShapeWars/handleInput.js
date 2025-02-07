// 键盘事件：按下与抬起
document.addEventListener("keydown", (event) => {
     playerInput.setKeyStatus(event.key, true);
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

canvas.addEventListener("mouseup", (event) => {
     if (event.button === 0) {  // 0 是左键
          playerInput.setKeyStatus('left', false);
     } else if (event.button === 2) {  // 2 是右键
          playerInput.setKeyStatus('right', false);
     }
});

//鼠标移动
canvas.addEventListener("mousemove", (event) => {
     playerInput.mouseXInScreen = event.clientX;
     playerInput.mouseYInScreen = event.clientY;
});

window.addEventListener('blur', function () {
     playerInput.onBlur();
});