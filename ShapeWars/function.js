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
function box2DtoScreen(x, y, canvas) {
     ({ x: x, y: y } = box2DtoCanvas(x, y));

     //屏幕左上角在大地图的坐标
     let { x: ltx, y: lty } = box2DtoCanvas(camera.x, camera.y);
     ltx = Math.round(ltx - canvas.width * camera.fov / 2);
     lty = Math.round(lty - canvas.height * camera.fov / 2);
     return { x: (x - ltx) / camera.fov, y: (y - lty) / camera.fov };
}