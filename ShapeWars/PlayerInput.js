class PlayerInput {
     static keyMapping = {
          'left': 0,    // 左键
          'right': 1,   // 右键
          'w': 2,       // W
          'a': 3,       // A
          's': 4,       // S
          'd': 5        // D
     };
     constructor() {
          this.mouseX = 0.0;
          this.mouseY = 0.0;
          this.keyStatus = new Array(64).fill(false);
     }

     onBlur() // 窗口失焦时
     {
          this.keyStatus.fill(false);
     }

     setKeyStatus(key, status) {
          let n = PlayerInput.keyMapping[key];
          if (n !== undefined)
               this.keyStatus[n] = status;
     }

     packData() {
          let message = new Uint8Array(1 + 4 + 4 + 8);
          let offset = 0;
          message[offset++] = 0x01;  //操作指令
          // 打包鼠标位置（x 和 y）
          let view = new DataView(message.buffer);
          view.setFloat32(offset, this.mouseX, true);  // 小端字节序
          offset += 4;
          view.setFloat32(offset, this.mouseY, true);
          offset += 4;
          //打包按键
          let keyStatusBigInt = BigInt(0);  // 初始化为 0
          for (let i = 0; i < Object.keys(PlayerInput.keyMapping).length; i++) {
               if (this.keyStatus[i]) {
                    keyStatusBigInt |= (1n << BigInt(i));  // 设置相应的位为 1
               }
          }
          view.setBigUint64(offset, keyStatusBigInt, true);
          return message;
     }
}