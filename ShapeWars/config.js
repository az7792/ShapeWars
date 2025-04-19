// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

//颜色常量
const COLORS = {
     miniMapBorder: "#cfcfcf",//小地图边框颜色
     backgroundValid: "#e3e3e3",//有效背景颜色
     backgroundInvalid: "#808080",//无效背景颜色
     backgroundLine: "#cfcfcf",//背景块线条颜色
     barrelFillColor: "#b7b7b7",//炮塔填充颜色
     barrelStrokeColor: "#727272",//炮塔边框颜色
     playerFillColor: ["#6984ae", "#d75f56", "#edb6bc", "#e8d179"],
     playerStrokeColor: ["#4c69a0", "#9e5044", "#c88194", "#c28f4f"],
     bulletFillColor: ["#6984ae", "#d75f56", "#edb6bc", "#e8d179"],
     bulletStrokeColor: ["#4c69a0", "#9e5044", "#c88194", "#c28f4f"],
};

// box2d 物理世界参数
MAPINFO = {
     width: 100, //地图宽度 (米)
     height: 100, //地图高度 (米)
     kScale: 50, //const //缩放比例 1米 = 50像素
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

//TODO：同步其他服务器数据
//       - TPS,MSPT
serverTime = {
     prev: 0,//前一帧
     curr: 0,//当前帧
     prevRecv: 0,//前一帧接收时间
     currRecv: 0,//当前帧接收时间
     historyFrameInterval: new Queue(120, 33),//历史帧间隔
     historyFrameIntervalRecv: new Queue(120, 33),//历史帧接收间隔
     avgFrameInterval: 33,//平均帧间隔
     deltaTime:1.1,
}

performanceMetrics = {
     isShow: true, //是否显示性能指标
     isConnected: false, //是否已连接到服务器
     cFPS: 0, //客户端渲染帧率
     ping: 0, //延迟 ms
     TPS: 0, //每秒事务数(服务器逻辑帧率)
     MSPT: 0, //平均单个事务处理时间 ms
}

playerStatus = {
     //当前玩家所属碰撞组
     nowGroupIndex: 0,
     nowEntityId: 0,
     nowTankID: -1,
     //当前玩家死活状态
     isAlive: false,
}

standings = [];//排名数据 [[name, score],...]

//Ping
pingTime = 0;


// 属性数据
let pdAttributes = [
     { id: 0, name: "回复速度", value: 0, max: 8, progressColor: '#FF6F61', btnColor: '#FF6F61' },
     { id: 1, name: "最大生命值", value: 0, max: 8, progressColor: '#6B5B95', btnColor: '#6B5B95' },
     { id: 2, name: "身体碰撞伤害", value: 0, max: 8, progressColor: '#88B04B', btnColor: '#88B04B' },
     { id: 3, name: "子弹伤害", value: 0, max: 8, progressColor: '#FFA500', btnColor: '#FFA500' },
     { id: 4, name: "子弹飞行时长", value: 0, max: 8, progressColor: '#92A8D1', btnColor: '#92A8D1' },
     { id: 5, name: "子弹血量", value: 0, max: 8, progressColor: '#955251', btnColor: '#955251' },
     { id: 6, name: "子弹速度", value: 0, max: 8, progressColor: '#B565A7', btnColor: '#B565A7' },
     { id: 7, name: "子弹密度", value: 0, max: 8, progressColor: '#009B77', btnColor: '#009B77' },
     { id: 8, name: "子弹射速", value: 0, max: 8, progressColor: '#DD4124', btnColor: '#DD4124' },
     { id: 9, name: "角色移动速度", value: 0, max: 8, progressColor: '#D65076', btnColor: '#D65076' }
];