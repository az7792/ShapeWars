class BaseEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.1;
          this.sides = 3;
          this.maxHP = 100;
          this.HP = 100;
          this.fillColor = "red";
          this.strokeColor = "blue";
     }

     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), this.fillColor, this.strokeColor);
          drawHealthBar(lerp(this.x, deltaTime), lerp(this.y, deltaTime) - 0.1, this.HP, this.maxHP);
     }

     update(dataView, offset) {
          this.x[0] = this.x[1];
          this.y[0] = this.y[1];
          this.angle[0] = this.angle[1];

          let componentState = dataView.getBigUint64(offset.value, true);  // 组件状态
          offset.value += 8;

          if (componentState & COMP_POSITION) {
               [this.x[1], this.y[1]] = readPosition(dataView, offset, 2);
          }

          if (componentState & COMP_ANGLE) {
               this.angle[1] = readAngle(dataView, offset);
          }

          if (componentState & COMP_POLYGON) {
               [this.sides, this.r] = readPolygon(dataView, offset);
          }

          if (componentState & COMP_HP) {
               [this.HP, this.maxHP] = readHP(dataView, offset);
          }

          return componentState;
     }

     static create(dataView, offset) {
          let res = new this();
          res.update(dataView, offset);
          res.x[0] = res.x[1];
          res.y[0] = res.y[1];
          res.angle[0] = res.angle[1];
          return res;
     }
}

class PolygonEntity extends BaseEntity {
     constructor() {
          super();
          this.sides = 3;  // 默认边数
          this.r = 0.1;  // 默认半径
     }
}


class PlayerEntity extends BaseEntity {
     constructor() {
          super();  // 调用父类构造器
          this.sides = 16;  // 玩家是圆形，16边多边形
          this.r = 0.05;  // 默认半径
          this.maxHP = 100;
          this.HP = 100;
          this.groupIndex = 0;
     }

     update(dataView, offset) {
          let componentState = super.update(dataView, offset);
          if (componentState & COMP_GROUPINDEX) {
               this.groupIndex = readGroupIndex(dataView, offset);
               if (this.groupIndex == nowGroupIndex) {
                    this.fillColor = COLORS.bulletFillColor[0];
                    this.strokeColor = COLORS.bulletStrokeColor[0];
               } else {
                    this.fillColor = COLORS.bulletFillColor[1];
                    this.strokeColor = COLORS.bulletStrokeColor[1];
               }
          }
     }
}

class BulletEntity extends PolygonEntity {
     constructor() {
          super();
     }

     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), this.fillColor, this.strokeColor);
     }

     update(dataView, offset) {
          this.x[0] = this.x[1];
          this.y[0] = this.y[1];

          let componentState = dataView.getBigUint64(offset.value, true);
          offset.value += 8;

          if (componentState & COMP_POSITION) {
               [this.x[1], this.y[1]] = readPosition(dataView, offset, 2);
          }

          if (componentState & COMP_POLYGON) {
               [this.sides, this.r] = readPolygon(dataView, offset);
          }

          if (componentState & COMP_GROUPINDEX) {
               this.groupIndex = readGroupIndex(dataView, offset);
               if (this.groupIndex == nowGroupIndex) {
                    this.fillColor = COLORS.bulletFillColor[0];
                    this.strokeColor = COLORS.bulletStrokeColor[0];
               } else {
                    this.fillColor = COLORS.bulletFillColor[1];
                    this.strokeColor = COLORS.bulletStrokeColor[1];
               }
          }
     }
}
