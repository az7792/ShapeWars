//三角形方块
class triangleEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.1;
          this.sides = 3;
          this.deltaTime = 0;
     }
     showMe(currTime) {
          if (serverTime.curr == serverTime.prev)
               this.deltaTime = 1;
          else
               this.deltaTime = Math.max(0, Math.min(1, (currTime - serverTime.prev) / (serverTime.curr - serverTime.prev)));
          drawRegularPolygon(this.sides, this.lerp(this.x), this.lerp(this.y), this.r, this.lerp(this.angle), "red", "blue");
     }

     lerp(val) {
          return (val[0] + (val[1] - val[0]) * this.deltaTime);
     }

     update(dataView, offset) {
          //更新所有需要插值的状态
          this.x[0] = this.x[1];
          this.y[0] = this.y[1];
          this.angle[0] = this.angle[1];

          let componentState = dataView.getBigUint64(offset.value, true);//组件状态
          offset.value += 8;
          let bitIsOne = Number(componentState >> BigInt(0)) & 1;//位置
          if (bitIsOne) {
               this.x[1] = dataView.getFloat32(offset.value, true);
               offset.value += 4;
               this.y[1] = dataView.getFloat32(offset.value, true);
               offset.value += 4;
          }
          bitIsOne = Number(componentState >> BigInt(2)) & 1;//半径
          if (bitIsOne) {
               this.r = dataView.getFloat32(offset.value, true);
               offset.value += 4;
          }
          bitIsOne = Number(componentState >> BigInt(3)) & 1;//角度
          if (bitIsOne) {
               this.angle[1] = dataView.getFloat32(offset.value, true);
               offset.value += 4;
          }
     }

     static create(dataView, offset) {
          let res = new triangleEntity();
          res.update(dataView, offset);
          return res;
     }
}