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
     x = Math.round((x + MAPINFO.width / 2) * MAPINFO.scale);
     y = Math.round((MAPINFO.height / 2 - y) * MAPINFO.scale);
     return { x, y };
}

//canvas大地图的像素坐标系 -> 角色box2d物理坐标
function canvasToBox2D(x, y) {
     x = x / MAPINFO.scale - MAPINFO.width / 2;
     y = MAPINFO.height / 2 - y / MAPINFO.scale;
     return { x, y };
}

//canvas大地图的像素坐标系 -> 小地图坐标系
function canvasToMiniMap(x, y) {
     x = Math.floor(x / MAPINFO.gridSize);
     y = Math.floor(y / MAPINFO.gridSize);
     return { x, y };
}