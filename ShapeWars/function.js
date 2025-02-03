/**
 * 所用坐标系:
 * box2d:#            Y
 * canvas:*           Y   
 * *****************************************X X
 * *                  #                    *
 * *                  #                    *
 * *                  #                    *
 * *                  #                    *
 * *#######################################*X X
 * *                  #                    *
 * *                  #                    *
 * *                  #                    *
 * *                  #                    *
 * *****************************************
 * Y
 * Y
 */

//角色box2d物理坐标 -> canvas大地图的像素坐标系
function box2DtoCanvas(x, y) {
     x = Math.round((x + MAPINFO.width / 2) * MAPINFO.kScale);
     y = Math.round((MAPINFO.height / 2 - y) * MAPINFO.kScale);
     return { x, y };
}

//canvas大地图的像素坐标系 -> 角色box2d物理坐标
function canvasToBox2D(x, y) {
     x = x / MAPINFO.kScale - MAPINFO.width / 2;
     y = MAPINFO.height / 2 - y / MAPINFO.kScale;
     return { x, y };
}

//canvas大地图的像素坐标系 -> 小地图坐标系
function canvasToMiniMap(x, y) {
     x = Math.floor(x / MAPINFO.kGridSize);
     y = Math.floor(y / MAPINFO.kGridSize);
     return { x, y };
}

//box2d物理坐标 -> 屏幕坐标系
function box2DtoScreen(x, y) {
     ({ x: x, y: y } = box2DtoCanvas(x, y));

     //屏幕左上角在大地图的坐标
     let { x: ltx, y: lty } = box2DtoCanvas(camera.x, camera.y);
     ltx = Math.round(ltx - canvas.width * camera.fov / 2);
     lty = Math.round(lty - canvas.height * camera.fov / 2);
     return { x: (x - ltx) / camera.fov, y: (y - lty) / camera.fov };
}

//绘制背景
function drawBackground() {

     ctx.save();

     //地图外的背景
     ctx.fillStyle = COLORS.backgroundInvalid;
     ctx.fillRect(0, 0, canvas.width, canvas.height);

     //相机 box2d物理坐标转 -> canvas大地图的像素坐标
     let { x: cx, y: cy } = box2DtoCanvas(camera.x, camera.y);

     //屏幕左上角在大地图的坐标
     let ltx = Math.round(cx - canvas.width * camera.fov / 2);
     let lty = Math.round(cy - canvas.height * camera.fov / 2);

     //由于fov导致每个gridSize的像素实际在画布上显示为gridSize/fov
     let gridSize = MAPINFO.kGridSize / camera.fov;//这儿直接整除如果有小数会导致后面的计算失真

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
     let sx = Math.round((Math.floor(ltx / MAPINFO.kGridSize) * MAPINFO.kGridSize - ltx) / camera.fov);
     let sy = Math.round((Math.floor(lty / MAPINFO.kGridSize) * MAPINFO.kGridSize - lty) / camera.fov);


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
     ctx.arc(canvas.width / 2, canvas.height / 2, 20 / camera.fov, 0, Math.PI * 2);
     ctx.fillStyle = 'blue';
     ctx.fill();

     ctx.restore();
}

//绘制小地图
function drawMiniMap() {

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
     let { x: cx, y: cy } = box2DtoCanvas(camera.x, camera.y);
     ({ x: cx, y: cy } = canvasToMiniMap(cx, cy));

     //角色在小地图的红色圆形标识
     ctx.beginPath();
     ctx.arc(x + cx * 100 / miniMap.width, y + cy * 100 / miniMap.width, 2, 0, Math.PI * 2);
     ctx.fillStyle = 'red';
     ctx.fill();
}


/**
 * 绘制多边形
 * @param {[point]} points - 多边形顶点数组
 * @param {string} fillClose - 填充颜色
 * @param {string} strokeClose - 描边颜色
 */
function drawPolygon(points, fillColor, strokeColor) {
     for (let i = 0; i < points.length; i++) {
          points[i] = box2DtoScreen(points[i].x, points[i].y, canvas);
     }

     ctx.save();
     ctx.beginPath();

     ctx.moveTo(points[0].x, points[0].y);
     for (let i = 1; i < points.length; i++) {
          ctx.lineTo(points[i].x, points[i].y);
     }
     ctx.closePath();//自动闭合路径

     ctx.fillStyle = fillColor;
     ctx.fill();

     ctx.lineWidth = 3 / camera.fov;
     ctx.strokeStyle = strokeColor;
     ctx.stroke();

     ctx.restore();
}

/**
 * 绘制正多边形
 * @param {uint} sides - 边数
 * @param {f32} x - box2d x坐标
 * @param {f32} y - box2d y坐标
 * @param {f32} r - 半径
 * @param {f32}  angle - 旋转角度
 * @param {string} fillColor - 填充颜色
 * @param {string} strokeColor - 描边颜色
 * @returns 
 */
function drawRegularPolygon(sides, x, y, r, angle, fillColor, strokeColor) {
     // 确保边数至少为3
     if (sides < 3) return;

     //变换到屏幕坐标系
     ({ x, y } = box2DtoScreen(x, y, canvas));
     r = r * MAPINFO.kScale / camera.fov;

     angle = -angle;

     // 计算每个顶点的角度增量
     const dAngle = (2 * Math.PI) / sides;

     ctx.save();
     ctx.beginPath();

     // 第一个点
     let startX = x + r * Math.cos(angle);
     let startY = y + r * Math.sin(angle);
     ctx.moveTo(startX, startY);

     // 剩余顶点
     for (let i = 1; i < sides; i++) {
          // 当前顶点的角度
          let currentAngle = angle + i * dAngle;
          let currentX = x + r * Math.cos(currentAngle);
          let currentY = y + r * Math.sin(currentAngle);
          ctx.lineTo(currentX, currentY);
     }

     ctx.closePath();

     ctx.fillStyle = fillColor;
     ctx.fill();

     ctx.strokeStyle = strokeColor;
     ctx.lineWidth = 3 / camera.fov;
     ctx.stroke();
     ctx.restore();
}