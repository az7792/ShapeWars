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
     lerpX: [0, 0],//每帧插值更新x坐标
     lerpY: [0, 0],//每帧插值更新x坐标
};

serverTime = {
     prev: 0,//前一帧
     curr: 0,//当前帧
}

performanceMetrics = {
     clientfps: 0, //客户端渲染帧率
     serverfps: 0, //服务器逻辑帧率
     ping: 0, //延迟 ms
     TPS: 0, //每秒事务数
     MSPT: 0, //平均单个事务处理时间 ms
}

//当前玩家ID
nowPlayerID = 0;