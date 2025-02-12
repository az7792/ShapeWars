//多边形方块
class polygonEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.1;
          this.sides = 15;
          this.maxHP = 100;
          this.HP = 30;
     }
     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), "red", "blue");
          drawHealthBar(lerp(this.x, deltaTime), lerp(this.y, deltaTime)-0.1, this.HP, this.maxHP);
     }

     update(dataView, offset) {
          //更新所有需要插值的状态
          this.x[0] = this.x[1];
          this.y[0] = this.y[1];
          this.angle[0] = this.angle[1];

          let componentState = dataView.getBigUint64(offset.value, true);//组件状态
          offset.value += 8;

          if (componentState & COMP_POSITION) {//位置
               [this.x[1], this.y[1]] = readPosition(dataView, offset, 2);
          }

          if (componentState & COMP_ANGLE) {//角度
               this.angle[1] = readAngle(dataView, offset);
          }

          if (componentState & COMP_POLYGON) {//多边形
               [this.sides, this.r] = readPolygon(dataView, offset);
          }

          if (componentState & COMP_HP) {//血量
               [this.HP, this.maxHP] = readHP(dataView, offset);
          }
     }

     static create(dataView, offset) {
          let res = new polygonEntity();
          res.update(dataView, offset);
          //更新初始状态
          res.x[0] = res.x[1];
          res.y[0] = res.y[1];
          res.angle[0] = res.angle[1];
          return res;
     }
}

//玩家
class playerEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.05;
          this.sides = 16;//>=16显示圆形
          this.maxHP = 100;
          this.HP = 20;
     }
     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), "red", "blue");
          drawHealthBar(lerp(this.x, deltaTime), lerp(this.y, deltaTime) - 0.1, this.HP, this.maxHP);
     }

     update(dataView, offset) {
          //更新所有需要插值的状态
          this.x[0] = this.x[1];
          this.y[0] = this.y[1];
          this.angle[0] = this.angle[1];

          let componentState = dataView.getBigUint64(offset.value, true);//组件状态
          offset.value += 8;

          if (componentState & COMP_POSITION) {//位置
               [this.x[1], this.y[1]] = readPosition(dataView, offset, 2);
          }

          if (componentState & COMP_ANGLE) {//角度
               this.angle[1] = readAngle(dataView, offset);
          }

          if (componentState & COMP_POLYGON) {//多边形
               [this.sides, this.r] = readPolygon(dataView, offset);
          }

          if (componentState & COMP_HP) {//血量
               [this.HP, this.maxHP] = readHP(dataView, offset);
          }
     }

     static create(dataView, offset) {
          let res = new playerEntity();
          res.update(dataView, offset);
          //更新初始状态
          res.x[0] = res.x[1];
          res.y[0] = res.y[1];
          res.angle[0] = res.angle[1];
          return res;
     }
}