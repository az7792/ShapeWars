/** @type {HTMLCanvasElement}*/


//--------test code start----------
camera.x = 0;
camera.y = 0;

for (let i = 0; i < 90; i++) {
     miniMap.fillRect(0, i, 100, 1, [2 * i, 255 - 2 * i, 100 + i, 255]);
}
miniMap.fillRect(0, 0, 5, 95, [11, 24, 14, 255]);

resizeCanvas();
update();

// // 监听键盘按下事件
// window.addEventListener('keydown', function (event) {
//      // 获取按下的键值
//      const key = event.key.toLowerCase(); // 小写化，确保可以兼容大小写

//      switch (key) {
//           case 'w':
//                console.log('W 键被按下');
//                camera.y += 0.01;
//                update();
//                break;
//           case 'a':
//                console.log('A 键被按下');
//                camera.x -= 0.01;
//                update();
//                break;
//           case 's':
//                console.log('S 键被按下');
//                camera.y -= 0.01;
//                update();
//                break;
//           case 'd':
//                console.log('D 键被按下');
//                camera.x += 0.01;
//                update();
//                break;
//           case 'l':
//                console.log('L 键被按下');
//                setInterval(() => {
//                     camera.x += 0.01;
//                     update();
//                }, 20)
//                break;
//           case 'i':
//                console.log('I 键被按下');
//                camera.fov += 0.1;
//                update();
//                break;
//           case 'k':
//                console.log('K 键被按下');
//                camera.fov -= 0.1;
//                camera.fov = Math.max(0.1, camera.fov);
//                update();
//                break;
//           case 'm':
//                console.log('M 键被按下');
//                let points = [
//                     { x: 0, y: 0 },
//                     { x: 0.1, y: 0.1 },
//                     { x: 0.1, y: 0.2 },
//                     { x: 0, y: 0.2 }];
//                drawPolygon(points, 'blue', 'red');
//                drawRegularPolygon(3, 0.35, 0.13, 0.2, Math.PI * (-0) / 180, 'blue', 'yellow');
//                break;
//           default:
//                break;
//      }
// });

//----------test code end----------

//更新画布大小
function resizeCanvas() {
     canvas.width = window.innerWidth * scale;
     canvas.height = window.innerHeight * scale;
}

// 监听窗口大小变化
window.onresize = () => {
     resizeCanvas();
     update();
}

//更新画布
function update() {
     drawBackground();//更新背景
     entityManager.update(Date.now());//更新实体
     drawMiniMap();//更新小地图
     requestAnimationFrame(update);
}


// //Ping
// setInterval(() => {
//      let message = new Uint8Array(1);
//      message[0] = 0x02;
//      sendMessage(message);
//      pingTime = Date.now();
// }, 1000);