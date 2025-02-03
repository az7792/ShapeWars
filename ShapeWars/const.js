const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const scale = window.localStorage.getItem("no_retina") ? 1 : window.devicePixelRatio;
const miniMap = new MiniMap(MAPINFO.width * MAPINFO.kScale / MAPINFO.kGridSize, MAPINFO.height * MAPINFO.kScale / MAPINFO.kGridSize);
const entityManager = new EntityManager();
const socket = new WebSocket("ws://localhost:7792");
socket.binaryType = 'arraybuffer';
const playerInput = new PlayerInput();