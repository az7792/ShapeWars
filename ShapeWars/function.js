//角色box2d物理坐标 -> canvas大地图的像素坐标系
function Box2DtoCanvas(x, y) {
     x += 5;
     y = 5 - y;
     x = Math.round(x * 500);
     y = Math.round(y * 500);
     return { x, y };
}