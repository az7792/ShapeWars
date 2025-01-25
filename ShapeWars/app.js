/** @type {HTMLCanvasElement}*/
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const scale = window.localStorage.getItem("no_retina") ? 1 : window.devicePixelRatio;
const miniMap = new MiniMap(MAPINFO.width * MAPINFO.scale / MAPINFO.gridSize, MAPINFO.height * MAPINFO.scale / MAPINFO.gridSize);

//--------test code start----------
let tmpx = -5;
let tmpy = -5;

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
          case 'i':
               console.log('I 键被按下');
               MAPINFO.fov += 0.1;
               break;
          case 'k':
               console.log('K 键被按下');
               MAPINFO.fov -= 0.1;
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
     ({ x: palyerX, y: palyerY } = box2DtoCanvas(palyerX, palyerY));

     //屏幕左上角在大地图的坐标
     let ltx = Math.round(palyerX - canvas.width * MAPINFO.fov / 2);
     let lty = Math.round(palyerY - canvas.height * MAPINFO.fov / 2);

     //由于fov导致每个gridSize的像素实际在画布上显示为gridSize/fov
     let gridSize = MAPINFO.gridSize / MAPINFO.fov;//这儿直接整除如果有小数会导致后面的计算失真

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
     let sx = Math.round((Math.floor(ltx / MAPINFO.gridSize) * MAPINFO.gridSize - ltx) / MAPINFO.fov);
     let sy = Math.round((Math.floor(lty / MAPINFO.gridSize) * MAPINFO.gridSize - lty) / MAPINFO.fov);


     //准换到小地图
     let { x, y } = canvasToMiniMap(ltx, lty);
     let width = Math.ceil(canvas.width / gridSize) + 1;//多算一格，避免右下边界无法被覆盖到
     let height = Math.ceil(canvas.height / gridSize) + 1;//多算一格，避免右下边界无法被覆盖到

     ctx.imageSmoothingEnabled = false;//禁用图像插值
     ctx.drawImage(
          miniMap.canvas,
          x, y, width, height,
          sx, sy,
          width * gridSize,
          height * gridSize
     );

     console.log(palyerX, palyerY, ltx, lty, sx, sy, x, y, width, height);


     ctx.lineWidth = 1;
     ctx.strokeStyle = COLORS.backgroundLine;
     ctx.beginPath();
     for (let i = sx; i <= sx + width * gridSize; i += gridSize) {//竖线
          ctx.moveTo(i, sy);
          ctx.lineTo(i, sy + height * gridSize);
     }
     for (let i = sy; i <= sy + height * gridSize; i += gridSize) {//横线
          ctx.moveTo(sx, i);
          ctx.lineTo(sx + width * gridSize, i);
     }
     ctx.stroke();

     //测试用圆
     ctx.beginPath();
     ctx.arc(canvas.width / 2, canvas.height / 2, 20 / MAPINFO.fov, 0, Math.PI * 2);
     ctx.fillStyle = 'blue';
     ctx.fill();

     ctx.restore();
}

//绘制小地图
function drawMiniMap(palyerX, palyerY) {

     let width = 100;
     let height = 100 * miniMap.height / miniMap.width;
     let x = canvas.width - width - 30;
     let y = canvas.height - height - 30;

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
     ({ x: palyerX, y: palyerY } = box2DtoCanvas(palyerX, palyerY));
     ({ x: palyerX, y: palyerY } = canvasToMiniMap(palyerX, palyerY));

     //角色在小地图的红色圆形标识
     ctx.beginPath();
     ctx.arc(x + palyerX * 100 / miniMap.width, y + palyerY * 100 / miniMap.width, 2, 0, Math.PI * 2);
     ctx.fillStyle = 'red';
     ctx.fill();
}

//更新画布
function update(tmpx, tmpy) {
     drawBackground(tmpx, tmpy);
     drawMiniMap(tmpx, tmpy);
}