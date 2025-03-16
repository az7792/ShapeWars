// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

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
let lastCFPSTime = lastUpdataCFPSTime;
let animationFrameCount = 0;

let currTime = Date.now();
//用于统计统计逻辑帧加载的帧间隔
let serverTimePrev_ = 0;
let serverTimeCurr_ = 0;
let avg = 33;//实际用于插值间隔的值
function update() {
     let Len = 1;//MAYBE : 动态调整缓冲区大小
     if (serverTime.deltaTime >= 1) {          
          if (wsBuf.length >= Len) {
               popBuf();
               let offset = (serverTime.deltaTime - 1) * avg; // 由于渲染帧间隔往往不为服务器帧间隔的倍数，因此插值进度不会刚到为1，而是>1
               avg = serverTime.avgFrameInterval;
               serverTime.curr = currTime - offset;
               avg -= mapValue(Math.max(0, wsBuf.length - Len), 0, wsBuf.maxSize, 0, 10);//防止缓冲区堆积
               avg = Math.max(5, avg);
               //console.log(wsBuf.length,offset,avg);
               //统计逻辑帧加载的帧间隔(蓝线)
               serverTimePrev_ = serverTimeCurr_;
               serverTimeCurr_ = serverTime.curr;
               if (serverTimePrev_ === 0)
                    serverTimePrev_ = serverTimeCurr_ - 33;
               let frameInterval = serverTimeCurr_ - serverTimePrev_;
               serverTime.historyFrameInterval.push(frameInterval);
          } else {
               //console.log("no enough data");
          }
     }
     currTime = Date.now();
     serverTime.deltaTime = Math.max(0, Math.min(2, (currTime - serverTime.curr) / avg));   


     //更新客户端帧率(服务器帧率在网络部分更新)
     animationFrameCount++;
     if (currTime - lastUpdataCFPSTime >= 1000) {
          performanceMetrics.cFPS = animationFrameCount;
          animationFrameCount = 0;
          lastUpdataCFPSTime = currTime;
     }
     lastCFPSTime = currTime;


     // 插值摄像机位置
     camera.x = lerp(camera.lerpX, serverTime.deltaTime);
     camera.y = lerp(camera.lerpY, serverTime.deltaTime);

     //绘制画面
     drawBackground();//更新背景
     entityManager.update(serverTime.deltaTime);//更新实体(插值)
     drawMiniMap();//更新小地图
     drawPerformance();//更新性能参数
     drawPlayerInfo();//绘制操作者的信息(在屏幕底部显示)
     drawStandings(); //绘制排名
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