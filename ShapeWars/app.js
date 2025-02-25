/** @type {HTMLCanvasElement}*/
// for (let i = 0; i < 90; i++) {
//      miniMap.fillRect(0, i, 100, 1, [2 * i, 255 - 2 * i, 100 + i, 255]);
// }
// miniMap.fillRect(0, 0, 5, 95, [11, 24, 14, 255]);

miniMap.fillRect(0, 0, 100, 100, [205, 205, 205, 255])

//更新画布大小
function resizeCanvas() {
     const scale = window.localStorage.getItem("no_retina") ? 1 : window.devicePixelRatio;
     canvas.width = window.innerWidth * scale;
     canvas.height = window.innerHeight * scale;

     gameControls.style.top = (canvas.height - gameControls.offsetHeight) / 2 / scale + "px";
     gameControls.style.left = (canvas.width - gameControls.offsetWidth) / 2 / scale + "px";
}

// 监听窗口大小变化
window.onresize = () => {
     resizeCanvas();
}

//更新画布
let lastUpdataCFPSTime = Date.now();
let animationFrameCount = 0;
function update() {
     let deltaTime;
     let currTime = Date.now();

     //更新客户端帧率(服务器帧率在网络部分更新)
     animationFrameCount++;
     if (currTime - lastUpdataCFPSTime >= 1000) {
          performanceMetrics.cFPS = animationFrameCount;
          animationFrameCount = 0;
          lastUpdataCFPSTime = currTime;
     }

     //计算插值使用的时间变化
     if (serverTime.curr == serverTime.prev)
          deltaTime = 1;
     else
          deltaTime = Math.max(0, Math.min(1, (currTime - serverTime.curr) / (serverTime.curr - serverTime.prev)));
     // 插值摄像机位置
     camera.x = lerp(camera.lerpX, deltaTime);
     camera.y = lerp(camera.lerpY, deltaTime);

     //绘制画面
     drawBackground();//更新背景
     entityManager.update(deltaTime);//更新实体
     drawMiniMap();//更新小地图
     drawPerformance();//更新性能参数
     drawPlayerInfo();//绘制操作者的信息(在屏幕底部显示)
     drawDeath();//绘制玩家死亡画面

     requestAnimationFrame(update);
}


//Ping
setInterval(() => {
     let message = new Uint8Array(1);
     message[0] = 0x02;
     sendMessage(message);
     pingTime = Date.now();
}, 1000);

//启动
resizeCanvas();
update();