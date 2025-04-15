// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

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

     updateRGBA(color, alpha) {
          alpha = Math.min(255, Math.max(0, alpha));

          if (color.length === 9) {//#RRGGBBAA
               return color.substring(0, 7) + alpha.toString(16).padStart(2, '0');
          } //RRGGBB
          return color + alpha.toString(16).padStart(2, '0');
     }


     showMe(deltaTime) {
          drawRegularPolygon(this.sides, lerp(this.x, deltaTime), lerp(this.y, deltaTime), this.r, lerp(this.angle, deltaTime), this.fillColor, this.strokeColor);
          if (this.HP < this.maxHP)
               drawHealthBar(lerp(this.x, deltaTime), lerp(this.y, deltaTime) - (this.sides >= 16 ? this.r : (this.r / Math.cos(Math.PI / this.sides))), this.HP, this.maxHP);
     }

     initDeadStatus() {
          this.deadPrevTime = (serverTime.curr - serverTime.avgFrameInterval) - (1 / this.speedRate - 1) * serverTime.avgFrameInterval;//变相增加从val[0]到val[1]的时间
          this.deadTime = serverTime.curr;
     }

     showDeadAnimation(currentTime) {
          let delta = (currentTime - this.deadTime) / this.deadAnimationDuration;

          //透明度
          let alpha = Math.floor(255 * Math.max(0, 1 - delta));//HACK：应该从原始透明的开始下降，而不是从1(255)开始下降
          let deadR = this.r * (1 + 0.3 * delta);//半径

          let rgbaFillColor = this.updateRGBA(this.fillColor, alpha);
          let rgbaStrokeColor = this.updateRGBA(this.strokeColor, alpha);
          currentTime -= serverTime.avgFrameInterval;//修正时间,当实体死亡时,客户端会在一个服务器tick后才知道实体死亡
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

          if (componentState & COMP_STYLE) {
               [this.fillColor, this.strokeColor] = readStyle(dataView, offset);
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
          this.Barrels = [];
          this.isOperator = false;
          this.score = 0;
     }

     showMe(deltaTime) {
          for (let i = 0; i < this.Barrels.length; i++)
               this.Barrels[i].showMe(deltaTime, lerp(this.x, deltaTime), lerp(this.y, deltaTime), lerp(this.angle, deltaTime));
          super.showMe(deltaTime);
          if (!this.isOperator)
               drawNameAndScore(lerp(this.x, deltaTime), lerp(this.y, deltaTime) + this.r, this.name,this.score);
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

          if (componentState & COMP_BARRELLIST) {
               let size = dataView.getUint8(offset.value, true);
               offset.value += 1;
               while (this.Barrels.length < size) {
                    this.Barrels.push(new Barrel());
               }
               for (let i = 0; i < size; i++) {
                    this.Barrels[i].update(dataView, offset);
               }
          }

          if (componentState & COMP_SCORE) {
               this.score = readScore(dataView, offset);
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

class Barrel {
     constructor() {
          this.widthL = 0.5;
          this.widthR = 0.5;
          this.length = [0.5, 0.5];
          this.offsetAngle = 0;
          this.offsetY = 0;
     }

     update(dataView, offset) {
          this.length[0] = this.length[1];

          this.widthL = dataView.getFloat32(offset.value, true);
          offset.value += 4;
          this.widthR = dataView.getFloat32(offset.value, true);
          offset.value += 4;
          this.length[1] = dataView.getFloat32(offset.value, true);
          offset.value += 4;
          this.offsetAngle = dataView.getFloat32(offset.value, true);
          offset.value += 4;
          this.offsetY = dataView.getFloat32(offset.value, true);
          offset.value += 4;
     }

     showMe(deltaTime, x, y, angle) {
          angle += this.offsetAngle;
          let points = [];
          const cosTheta = Math.cos(angle);
          const sinTheta = Math.sin(angle);

          // 计算靠近圆心的边的左右端点
          const left1X = x - sinTheta * (this.widthL / 2);
          const left1Y = y + cosTheta * (this.widthL / 2);
          const right1X = x + sinTheta * (this.widthL / 2);
          const right1Y = y - cosTheta * (this.widthL / 2);

          // 计算远离圆心的边的中心点坐标
          const tmpLength = lerp(this.length, deltaTime);
          const endX = x + cosTheta * tmpLength;
          const endY = y + sinTheta * tmpLength;

          // 计算远离圆心的边的左右端点
          const left2X = endX - sinTheta * (this.widthR / 2);
          const left2Y = endY + cosTheta * (this.widthR / 2);
          const right2X = endX + sinTheta * (this.widthR / 2);
          const right2Y = endY - cosTheta * (this.widthR / 2);

          const offsetx = -Math.sin(angle) * this.offsetY;
          const offsety = Math.cos(angle) * this.offsetY;

          points.push([left1X + offsetx, left1Y + offsety]);
          points.push([right1X + offsetx, right1Y + offsety]);
          points.push([right2X + offsetx, right2Y + offsety]);
          points.push([left2X + offsetx, left2Y + offsety]);
          drawPolygon(points, COLORS.barrelFillColor, COLORS.barrelStrokeColor);          
     }
}