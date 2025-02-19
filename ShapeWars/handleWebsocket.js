// 连接打开时触发
socket.onopen = () => {
     console.log("WebSocket 连接已建立");
};

function parseMessage(dataView, offset) {
     let MessageType = dataView.getUint8(offset.value, true);
     offset.value++;
     switch (MessageType) {
          case 0x00: // 初始化地图数据
               MAPINFO.width = dataView.getFloat32(offset.value, true);
               offset.value += 4;
               MAPINFO.height = dataView.getFloat32(offset.value, true);
               offset.value += 4;
               miniMap.reset(MAPINFO.width * MAPINFO.kScale / MAPINFO.kGridSize, MAPINFO.height * MAPINFO.kScale / MAPINFO.kGridSize)
               break;
          case 0x01: // 更新实体
               //当前玩家所在碰撞组
               playerStatus.nowGroupIndex = readGroupIndex(dataView, offset);
               // 摄像机位置
               camera.lerpX[0] = camera.lerpX[1];
               camera.lerpY[0] = camera.lerpY[1];
               [camera.lerpX[1], camera.lerpY[1]] = readPosition(dataView, offset);
               //需要移出的实体列表
               let listLen = dataView.getUint16(offset.value, true);
               offset.value += 2;
               while (listLen--) {
                    entityManager.removeEntityByDataView(dataView, offset);
               }
               //需要删除的实体列表
               listLen = dataView.getUint16(offset.value, true);
               offset.value += 2;
               while (listLen--) {
                    entityManager.deleteEntityByDataView(dataView, offset);
               }
               //需要增加的实体列表
               listLen = dataView.getUint16(offset.value, true);
               offset.value += 2;
               while (listLen--) {
                    entityManager.addEntityByDataView(dataView, offset);
               }
               //需要更新的实体列表
               listLen = dataView.getUint16(offset.value, true);
               offset.value += 2;
               while (listLen--) {
                    entityManager.updateEntityByDataView(dataView, offset);
               }
               //更新时间
               serverTime.prev = serverTime.curr;
               serverTime.curr = Date.now();
               performanceMetrics.TPS = Math.round(1000 / (serverTime.curr - serverTime.prev));
               sendMessage(playerInput.packData());
               break;
          case 0x02://Ping
               break;
          case 0x03://Pong
               performanceMetrics.ping = Date.now() - pingTime;
               break;
          case 0x04://LZ4压缩包
               let compressedSize = dataView.getInt32(offset.value, true);
               offset.value += 4;
               let decompressedSize = dataView.getInt32(offset.value, true);
               offset.value += 4;
               const compressedData = new Uint8Array(dataView.buffer, offset.value, decompressedSize);
               offset.value += decompressedSize;
               let res = decompressLZ4(compressedData, compressedSize);
               parseMessage(new DataView(res.buffer), { value: 0 });
               break;
          case 0x05://玩家死亡消息
               playerStatus.isAlive = false;
               startButton.style.display = 'flex';
               playerInput.keyStatus = new Array(64).fill(false);
               break;
          case 0x06://玩家创建角色消息
               playerStatus.isAlive = true;
               startButton.style.display = 'none';               
               break;
     }
}

socket.onmessage = (event) => {
     //原始数据
     let offset = { value: 0 };
     const dataView = new DataView(event.data);
     parseMessage(dataView, offset);
};

// 连接关闭时触发
socket.onclose = () => {
     console.log("WebSocket 连接已关闭");
};

// 发生错误时触发
socket.onerror = (error) => {
     console.error("WebSocket 错误：", error);
};

//发送
function sendMessage(messageText) {
     if (socket.readyState === WebSocket.OPEN) {
          socket.send(messageText);
     } else {
          //alert("WebSocket 连接未打开");
     }
}