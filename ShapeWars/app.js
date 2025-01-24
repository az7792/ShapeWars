/** @type {HTMLCanvasElement}*/
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const scale = window.localStorage.getItem("no_retina") ? 1 : window.devicePixelRatio;
const miniMap = new MiniMap(100, 100);

//--------test code start----------
let tmpx = -4.6;
let tmpy = -4.6;

for (let i = 0; i < 90; i++) {
     miniMap.fillRect(0, i, 100, 1, [2 * i, 255 - 2 * i, 100 + i, 255]);
}
miniMap.fillRect(0, 0, 5, 95, [11, 24, 14, 255]);

resizeCanvas();
update(tmpx, tmpy);

// 监听键盘按下事件
window.addEventListener('keydown', function (event) {
     // 获取按下的键值
     const key = event.key.toLowerCase(); // 小写化，确保可以兼容大小写

     switch (key) {
          case 'w':
               console.log('W 键被按下');
               tmpy += 0.01;
               break;
          case 'a':
               console.log('A 键被按下');
               tmpx -= 0.01;
               break;
          case 's':
               console.log('S 键被按下');
               tmpy -= 0.01;
               break;
          case 'd':
               console.log('D 键被按下');
               tmpx += 0.01;
               break;
          case 'l':
               console.log('L 键被按下');
               setInterval(() => {
                    tmpx += 0.01;
                    update(tmpx, tmpy);
               }, 20)
               break;
          default:
               break;
     }
     update(tmpx, tmpy);
});

//----------test code end----------

//更新画布大小
function resizeCanvas() {
     canvas.width = window.innerWidth * scale;
     canvas.height = window.innerHeight * scale;
}

// 监听窗口大小变化
window.onresize = () => {
     resizeCanvas();
     update(tmpx, tmpy);
}

//绘制背景
function drawBackground(palyerX, palyerY) {

     ctx.save();

     //地图外的背景
     ctx.fillStyle = COLORS.backgroundInvalid;
     ctx.fillRect(0, 0, canvas.width, canvas.height);

     //角色 box2d物理坐标转 -> canvas大地图的像素坐标
     ({ x: palyerX, y: palyerY } = Box2DtoCanvas(palyerX, palyerY));

     //屏幕左上角在大地图的坐标
     let ltx = Math.round(palyerX - canvas.width / 2);
     let lty = Math.round(palyerY - canvas.height / 2);

     /** 
      * 屏幕左上角在大地图的坐标的偏移量
      * A*********
      * *        *
      * *     B*********
      * *     *  *     *
      * *     *  *     *
      * **********     *
      *       *        *
      *       **********
      * B: (ltx,lty)，在实际绘制中为(0,0)点
      * A: B所在的(50*50)的方格的左上角坐标
      * (sx,sy)：A - B
      */
     let sx = Math.floor(ltx / 50) * 50 - ltx;
     let sy = Math.floor(lty / 50) * 50 - lty;

     //准换到小地图
     x = Math.floor((ltx) / 50);
     y = Math.floor((lty) / 50);
     let width = Math.ceil(canvas.width / 50) + 1;//避免右下边界无法被覆盖到
     let height = Math.ceil(canvas.height / 50) + 1;//避免右下边界无法被覆盖到

     ctx.imageSmoothingEnabled = false;//禁用图像插值
     ctx.drawImage(
          miniMap.canvas,
          x, y, width, height,
          sx, sy, width * 50, height * 50
     );

     ctx.lineWidth = 1;
     ctx.strokeStyle = COLORS.backgroundLine;
     ctx.beginPath();
     for (let i = sx; i <= sx + width * 50; i += 50) {//竖线
          ctx.moveTo(i, sy);
          ctx.lineTo(i, sy + height * 50);
     }
     for (let i = sy; i <= sy + height * 50; i += 50) {//横线
          ctx.moveTo(sx, i);
          ctx.lineTo(sx + width * 50, i);
     }
     ctx.stroke();

     //测试用圆
     ctx.beginPath();
     ctx.arc(canvas.width / 2, canvas.height / 2, 20, 0, Math.PI * 2);
     ctx.fillStyle = 'blue'; 
     ctx.fill(); 

     ctx.restore();
}

//绘制小地图
function drawMiniMap(palyerX, palyerY) {

     let x = canvas.width - miniMap.width - 30;
     let y = canvas.height - miniMap.height - 30;
     let width = miniMap.width;
     let height = miniMap.height;

     //地图
     ctx.drawImage(
          miniMap.canvas,
          0, 0, miniMap.width, miniMap.height,
          x, y, width, height
     );

     //边框
     ctx.lineWidth = 3;
     ctx.strokeStyle = COLORS.miniMapBorder;
     ctx.strokeRect(x, y, width, height);

     //角色 box2d物理坐标转 -> canvas小地图的像素坐标 -> canvas大地图的像素坐标
     palyerX += 5;
     palyerY = 5 - palyerY;
     palyerX = Math.floor(palyerX * 500 / 50);
     palyerY = Math.floor(palyerY * 500 / 50);

     //角色在小地图的红色圆形标识
     ctx.beginPath();
     ctx.arc(x + palyerX, y + palyerY, 2, 0, Math.PI * 2);
     ctx.fillStyle = 'red';
     ctx.fill();
}

//更新画布
function update(tmpx, tmpy) {
     drawBackground(tmpx, tmpy);
     drawMiniMap(tmpx, tmpy);
}