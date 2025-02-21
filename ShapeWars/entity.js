class BaseEntity {
     constructor() {
          this.x = [0, 0];
          this.y = [0, 0];
          this.angle = [0, 0];
          this.r = 0.1;
          this.sides = 3;
          this.maxHP = 100;
          this.HP = 100;
          this.fillColor = "#ff0000";
          this.strokeColor = "#0000ff";

          this.deadPrevTime = 0;//死亡前的时间
          this.deadTime = 0;//死亡时的时间
          this.deadAnimationDuration = 200;//ms
          this.speedRate = 0.3;//控制速度为死亡时速度的倍数
     }
     hexToRGBA(hex, alpha) {
          hex = hex.replace('#', '');

          let r = parseInt(hex.substring(0, 2), 16);
          let g = parseInt(hex.substring(2, 4), 16);
          let b = parseInt(hex.substring(4, 6), 16);

          // 返回 RGBA 格式的颜色
          return `rgba(${r}, ${g}, ${b}, ${alpha})`;
     }

     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), this.fillColor, this.strokeColor);
          if (this.HP < this.maxHP)
               drawHealthBar(lerp(this.x, deltaTime), lerp(this.y, deltaTime) - (this.sides >= 16 ? this.r : (this.r / Math.cos(Math.PI / this.sides))), this.HP, this.maxHP);
     }

     initDeadStatus() {
          this.deadPrevTime = serverTime.prev - (1 / this.speedRate - 1) * (serverTime.curr - serverTime.prev);//变相增加从val[0]到val[1]的时间
          this.deadTime = serverTime.curr;
     }

     showDeadAnimation(currentTime) {
          let delta = (currentTime - this.deadTime) / this.deadAnimationDuration;

          let alpha = Math.max(0, 1 - delta);//透明度
          let deadR = this.r * (1 + 0.3 * delta);//半径

          let rgbaFillColor = this.hexToRGBA(this.fillColor, alpha);
          let rgbaStrokeColor = this.hexToRGBA(this.strokeColor, alpha);
          currentTime -= (serverTime.curr - serverTime.prev);//修正时间,当实体死亡时,客户端会在一个服务器tick后才知道实体死亡
          drawRegularPolygon(this.sides, predict(this.x, this.deadPrevTime, this.deadTime, currentTime), predict(this.y, this.deadPrevTime, this.deadTime, currentTime), deadR, this.angle[1], rgbaFillColor, rgbaStrokeColor);
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
               if (this.angle[1] - this.angle[0] > Math.PI)
                    this.angle[0] += Math.PI * 2;
               else if (this.angle[0] - this.angle[1] > Math.PI)
                    this.angle[0] -= Math.PI * 2;
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
          this.name = "";
          this.deadAnimationDuration = 500;
     }

     showMe(deltaTime) {
          super.showMe(deltaTime);
          drawName(lerp(this.x, deltaTime), lerp(this.y, deltaTime) + this.r, this.name);
     }


     update(dataView, offset) {
          let componentState = super.update(dataView, offset);
          if (componentState & COMP_GROUPINDEX) {
               this.groupIndex = readGroupIndex(dataView, offset);
               if (this.groupIndex == playerStatus.nowGroupIndex) {
                    this.fillColor = COLORS.bulletFillColor[0];
                    this.strokeColor = COLORS.bulletStrokeColor[0];
               } else {
                    this.fillColor = COLORS.bulletFillColor[1];
                    this.strokeColor = COLORS.bulletStrokeColor[1];
               }
          }
          if (componentState & COMP_NAME) {
               this.name = readName(dataView, offset);
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
               if (this.groupIndex == playerStatus.nowGroupIndex) {
                    this.fillColor = COLORS.bulletFillColor[0];
                    this.strokeColor = COLORS.bulletStrokeColor[0];
               } else {
                    this.fillColor = COLORS.bulletFillColor[1];
                    this.strokeColor = COLORS.bulletStrokeColor[1];
               }
          }
     }
}
