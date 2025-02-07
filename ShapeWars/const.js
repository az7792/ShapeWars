const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const scale = window.localStorage.getItem("no_retina") ? 1 : window.devicePixelRatio;
const miniMap = new MiniMap(MAPINFO.width * MAPINFO.kScale / MAPINFO.kGridSize, MAPINFO.height * MAPINFO.kScale / MAPINFO.kGridSize);
const entityManager = new EntityManager();
const socket = new WebSocket("ws://localhost:7792");
socket.binaryType = 'arraybuffer';
const playerInput = new PlayerInput();


const COMP_POSITION = BigInt(1 << 0); // 0b00000001 位置
const COMP_VELOCITY = BigInt(1 << 1); // 0b00000010 速度
const COMP_ANGLE = BigInt(1 << 2);    // 0b00000100 朝向
const COMP_POLYGON = BigInt(1 << 3);  // 0b00001000 正多边形
const COMP_HP = BigInt(1 << 4);       // 0b00010000 血量
const COMP_TYPEID = BigInt(1 << 5);   // 0b00100000 类型ID

let pingTime = 0;