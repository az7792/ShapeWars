//插值函数
function lerp(val, deltaTime) {
     return (val[0] + (val[1] - val[0]) * deltaTime);
}

/**
 * 线性预测函数
 * @param {*} val - 原始值val[0]和val[1]
 * @param {*} t1 - val[0]时的时间
 * @param {*} t2 - val[1]时的时间
 * @param {*} t3 - 当前时间
 * @returns 
 */
function predict(val, t1, t2, t3) {
     const delta = t2 - t1;

     if (delta <= 0) return val[1];

     const timeDiff = t3 - t1;


     return (val[1] - val[0]) * timeDiff / delta + val[0];
}

/**
 * 解压LZ4压缩包
 * @param {Uint8Array} compressedData - 压缩数据 
 * @param {Number} decompressedSize - 原始数据大小
 * @returns 
 */
function decompressLZ4(compressedData, decompressedSize) {
     // 分配内存
     const compressedPtr = Module._malloc(compressedData.length);
     const decompressedPtr = Module._malloc(decompressedSize);

     // 将压缩数据写入内存
     Module.HEAPU8.set(compressedData, compressedPtr);

     // 调用 LZ4 解压缩函数
     const resultSize = Module._LZ4_decompress_safe(
          compressedPtr,
          decompressedPtr,
          compressedData.length,
          decompressedSize
     );

     if (resultSize < 0) {
          throw new Error("LZ4 decompression failed");
     }

     // 读取解压后的数据
     const decompressedData = new Uint8Array(resultSize);
     decompressedData.set(//从 WebAssembly 内存拷贝到 JavaScript 的内存中
          new Uint8Array(Module.HEAPU8.buffer, decompressedPtr, resultSize)
     );

     // 释放内存
     Module._free(compressedPtr);
     Module._free(decompressedPtr);

     return decompressedData;
}

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
     x = (x + MAPINFO.width / 2) * MAPINFO.kScale;
     y = (MAPINFO.height / 2 - y) * MAPINFO.kScale;
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
     ltx = ltx - canvas.width * camera.fov / 2;
     lty = lty - canvas.height * camera.fov / 2;
     return { x: (x - ltx) / camera.fov, y: (y - lty) / camera.fov };
}

//屏幕坐标系 -> box2d物理坐标
function screenToBox2D(x, y) {
     //屏幕左上角在大地图的坐标
     let { x: ltx, y: lty } = box2DtoCanvas(camera.x, camera.y);
     ltx = ltx - canvas.width * camera.fov / 2;
     lty = lty - canvas.height * camera.fov / 2;

     x = ltx + x * camera.fov;
     y = lty + y * camera.fov;
     return canvasToBox2D(x, y);
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


//绘制性能指标
function drawPerformance() {
     // performanceMetrics = {
     //      isShow: true, //是否显示性能指标
     //      clientfps: 0, //客户端渲染帧率
     //      serverfps: 0, //服务器逻辑帧率
     //      ping: 0, //延迟 ms
     //      TPS: 0, //每秒事务数
     //      MSPT: 0, //平均单个事务处理时间 ms
     // }

     if (!performanceMetrics.isShow)
          return;
     ctx.save();
     let Fsize = 16;
     ctx.font = Fsize + 'px Arial';  // 字体大小和类型
     let x = canvas.width - 100;
     let y = 20;
     //绘制客户端渲染帧率
     ctx.fillStyle = performanceMetrics.cFPS <= 30 ? "#ff0000" : "#00ff00";
     ctx.fillText("FPS: " + performanceMetrics.cFPS, x, y);
     y += Fsize + 2;
     //绘制延迟
     if (performanceMetrics.isConnected) {
          ctx.fillStyle = performanceMetrics.ping >= 60 ? "#ff0000" : "#00ff00";
          ctx.fillText("Ping: " + performanceMetrics.ping + "ms", x, y);
     }
     else {
          ctx.fillStyle = "#ff0000";
          ctx.fillText("Ping: -- ms", x, y);
     }
     y += Fsize + 2;
     //绘制TPS
     ctx.fillStyle = performanceMetrics.TPS <= 20 ? "#ff0000" : "#00ff00";
     ctx.fillText("TPS: " + performanceMetrics.TPS, x, y);
     y += Fsize + 2;
     //TODO：绘制MSPT
     ctx.fillStyle = performanceMetrics.MSPT >= 50 ? "#ff0000" : "#00ff00";
     ctx.fillText("MSPT: " + performanceMetrics.MSPT + "ms", x, y);
     ctx.restore();
}

//绘制死亡界面(非复活状态)
function drawDeath() {
     if (playerStatus.isAlive)
          return;
     ctx.save();
     ctx.fillStyle = "rgba(0,0,0,0.5)";
     ctx.fillRect(0, 0, canvas.width, canvas.height);
     ctx.restore();
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
     ({ x, y } = box2DtoScreen(x, y));
     r = r * MAPINFO.kScale / camera.fov;

     //外接圆半径
     r = r / Math.cos(Math.PI / sides);

     angle = -angle;

     ctx.save();
     ctx.beginPath();

     if (sides >= 16) {
          // 边数较多时，直接画圆
          ctx.arc(x, y, r, 0, 2 * Math.PI);
     } else {

          // 计算每个顶点的角度增量
          const dAngle = (2 * Math.PI) / sides;

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
     }

     ctx.closePath();

     ctx.fillStyle = fillColor;
     ctx.fill();

     ctx.strokeStyle = strokeColor;
     ctx.lineWidth = 3 / camera.fov;
     ctx.stroke();
     ctx.restore();
}

// 绘制血条
function drawHealthBar(x, y, health, maxHealth) {
     // 变换到屏幕坐标系
     ({ x, y } = box2DtoScreen(x, y));

     let width = Math.min(100, maxHealth) / camera.fov; // 血条宽度
     let height = 8 / camera.fov; // 血条高度
     let barWidth = width * health / maxHealth; // 当前血量对应的宽度
     let radius = 3 / camera.fov; // 圆角半径

     ctx.save();

     let startX = x - width / 2;
     let startY = y - height / 2;

     // 画底色（暗绿色）
     ctx.fillStyle = "#004400";
     ctx.beginPath();
     drawRoundedRect(startX, startY, width, height, radius);
     ctx.fill();

     // 画血量（亮绿色）
     ctx.fillStyle = "#00ff00";
     ctx.beginPath();
     drawRoundedRect(startX, startY, barWidth, height, radius);
     ctx.fill();

     // 画边框（黑色）
     ctx.strokeStyle = "#000000";
     ctx.lineWidth = 2;
     ctx.beginPath();
     drawRoundedRect(startX, startY, width, height, radius);
     ctx.stroke();

     ctx.restore();
}

// 绘制圆角矩形
function drawRoundedRect(x, y, width, height, radius) {
     ctx.beginPath();
     ctx.moveTo(x + radius, y);
     ctx.lineTo(x + width - radius, y);
     ctx.quadraticCurveTo(x + width, y, x + width, y + radius);
     ctx.lineTo(x + width, y + height - radius);
     ctx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
     ctx.lineTo(x + radius, y + height);
     ctx.quadraticCurveTo(x, y + height, x, y + height - radius);
     ctx.lineTo(x, y + radius);
     ctx.quadraticCurveTo(x, y, x + radius, y);
     ctx.closePath();
}