// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

const canvas = document.getElementById("canvas");
canvas.addEventListener("contextmenu", function (event) {//禁用浏览器右键菜单
     event.preventDefault();
});
const ctx = canvas.getContext("2d");
const miniMap = new MiniMap(MAPINFO.width * MAPINFO.kScale / MAPINFO.kGridSize, MAPINFO.height * MAPINFO.kScale / MAPINFO.kGridSize);
const entityManager = new EntityManager();
const socket = new WebSocket("ws://localhost:7792");
socket.binaryType = 'arraybuffer';
const playerInput = new PlayerInput();
const wsBuf = new Queue(10, [null, { value: 0 }, null]);//webSocket缓冲区

// 加点UI
const pointUI = new PointDistributionUI(pdAttributes, 'pd-attributeUI', 'pd-toggleBtn');


const CATEGORY_PLAYER = 0;
const CATEGORY_BLOCK = 1;
const CATEGORY_BULLET = 2;

const COMP_POSITION = BigInt(1 << 0); // 0b00000001 位置
const COMP_VELOCITY = BigInt(1 << 1); // 0b00000010 速度
const COMP_ANGLE = BigInt(1 << 2);    // 0b00000100 朝向
const COMP_POLYGON = BigInt(1 << 3);  // 0b00001000 正多边形
const COMP_HP = BigInt(1 << 4);       // 0b00010000 血量
const COMP_TYPE = BigInt(1 << 5);     // 0b00100000 类型
const COMP_GROUPINDEX = BigInt(1 << 6);   // 0b01000000 碰撞组索引
const COMP_NAME = BigInt(1 << 7);   // 0b10000000 名称
const COMP_STYLE = BigInt(1 << 8);  //0b100000000 填充样式
const COMP_BARRELLIST = BigInt(1 << 9); // 枪管列表
const COMP_SCORE = BigInt(1 << 10); // 得分