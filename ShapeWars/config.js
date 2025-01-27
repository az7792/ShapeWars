//颜色常量
const COLORS = {
     miniMapBorder: "#cfcfcf",//小地图边框颜色
     backgroundValid: "#e3e3e3",//有效背景颜色
     backgroundInvalid: "#808080",//无效背景颜色
     backgroundLine: "#cfcfcf",//背景块线条颜色
};

// box2d 物理世界参数
MAPINFO = {
     width: 10, //地图宽度 (米)
     height: 10, //地图高度 (米)
     kScale: 500, //const //缩放比例 1米 = 500像素
     kGridSize: 50, //const //背景格子尺寸，同时对应小地图的一个像素 (像素)

     //通过scale可以将box2d的物理坐标转换为canvas的像素坐标
};

camera = {
     x: 0,//Box2D的坐标
     y: 0,//Box2D的坐标
     fov: 1,//缩放系数,越大可视范围越大，例如50px实际显示为 50/fov px
};