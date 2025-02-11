/** @type {HTMLCanvasElement}*/
for (let i = 0; i < 90; i++) {
     miniMap.fillRect(0, i, 100, 1, [2 * i, 255 - 2 * i, 100 + i, 255]);
}
miniMap.fillRect(0, 0, 5, 95, [11, 24, 14, 255]);

resizeCanvas();
update();

//更新画布大小
function resizeCanvas() {
     canvas.width = window.innerWidth * scale;
     canvas.height = window.innerHeight * scale;
}

// 监听窗口大小变化
window.onresize = () => {
     resizeCanvas();
}

//更新画布
function update() {
     let deltaTime;
     if (serverTime.curr == serverTime.prev)
          deltaTime = 1;
     else
          deltaTime = Math.max(0, Math.min(1, (Date.now() - serverTime.curr) / (serverTime.curr - serverTime.prev)));
     camera.x = lerp(camera.lerpX, deltaTime);
     camera.y = lerp(camera.lerpY, deltaTime);
     drawBackground();//更新背景
     entityManager.update(deltaTime);//更新实体
     drawMiniMap();//更新小地图
     requestAnimationFrame(update);
}


 //Ping
 setInterval(() => {
      let message = new Uint8Array(1);
      message[0] = 0x02;
      sendMessage(message);
      pingTime = Date.now();
 }, 1000);