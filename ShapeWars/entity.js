//三角形方块
class triangleEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.1;
          this.sides = 3;
     }
     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), "red", "blue");
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