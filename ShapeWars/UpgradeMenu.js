class UpgradeMenu {
     constructor(tankConfigs, containerId, toggleBtnId) {
          this.tankConfigs = tankConfigs;
          this.container = document.getElementById(containerId);
          this.toggleBtn = document.getElementById(toggleBtnId);
          this.init();
     }

     init() {
          this.toggleBtn.addEventListener('click', () => this.toggleMenu());
          this.renderCanvases();
     }

     toggleMenu() {
          this.container.classList.toggle('active');
     }

     renderCanvases() {
          const grid = document.createElement('div');
          grid.className = 'upgrade-canvas-grid';
          const newHeight = Math.ceil(this.tankConfigs.length / 2) * 110;
          this.container.style.height = `${newHeight}px`;

          this.tankConfigs.forEach((p, index) => {
               const canvasContainer = document.createElement('div');
               canvasContainer.className = 'canvas-item';

               const canvas = document.createElement('canvas');
               canvas.width = 100;
               canvas.height = 100;

               canvas.addEventListener('click', () => this.sendValue(p));

               const ctx = canvas.getContext('2d');
               ctx.fillStyle = "rgba(227,227,227,1)";
               ctx.fillRect(0, 0, 100, 100);
               ctx.strokeStyle = "#727272";
               ctx.lineWidth = 5;
               ctx.strokeRect(0, 0, 100, 100);
               UpgradeMenu.drawTankImg(ctx, 50, 50, p);

               canvasContainer.appendChild(canvas);
               grid.appendChild(canvasContainer);
          });

          this.container.innerHTML = '';
          this.container.appendChild(grid);
     }

     // 重置方法
     reinit(id) {
          this.tankConfigs = tankdefs[id].upgrades;
          this.renderCanvases();
     }

     sendValue(p) {
          let message = new Uint8Array(2);
          message[0] = 0x06;
          message[1] = p & 0xFF;
          sendMessage(message);
     }

     /**
     * 绘制一个静态tank图形
     * @param {*} ctx - canvas上下文
     * @param {*} x - tank中心在canvas上的x坐标
     * @param {*} y - tank中心在canvas上的y坐标
     * @param {*} p - tank编号
     */
     static drawTankImg(ctx, x, y, p) {
          if (tankdefs === undefined) return;

          const scale = 40;
          const tankdef = tankdefs[p];
          ctx.save();
          ctx.lineJoin = "round";
          ctx.fillStyle = COLORS.barrelFillColor;
          ctx.strokeStyle = COLORS.barrelStrokeColor;
          ctx.lineWidth = 3;

          for (let i = 0; i < tankdef.barrels.length; i++) {
               const angle = tankdef.barrels[i].offsetAngle;
               const widthL = tankdef.barrels[i].widthL * scale;
               const widthR = tankdef.barrels[i].widthR * scale;
               const length = tankdef.barrels[i].length * scale;
               const offsetY = tankdef.barrels[i].offsetY * scale;

               let points = [];
               const cosTheta = Math.cos(angle);
               const sinTheta = Math.sin(angle);

               // 计算靠近圆心的边的左右端点
               const left1X = x - sinTheta * (widthL / 2);
               const left1Y = y + cosTheta * (widthL / 2);
               const right1X = x + sinTheta * (widthL / 2);
               const right1Y = y - cosTheta * (widthL / 2);

               // 计算远离圆心的边的中心点坐标
               const endX = x + cosTheta * length;
               const endY = y + sinTheta * length;

               // 计算远离圆心的边的左右端点
               const left2X = endX - sinTheta * (widthR / 2);
               const left2Y = endY + cosTheta * (widthR / 2);
               const right2X = endX + sinTheta * (widthR / 2);
               const right2Y = endY - cosTheta * (widthR / 2);

               const offsetx = -Math.sin(angle) * offsetY;
               const offsety = Math.cos(angle) * offsetY;

               points.push([left1X + offsetx, left1Y + offsety]);
               points.push([right1X + offsetx, right1Y + offsety]);
               points.push([right2X + offsetx, right2Y + offsety]);
               points.push([left2X + offsetx, left2Y + offsety]);


               ctx.beginPath();
               ctx.moveTo(points[0][0], points[0][1]);
               for (let i = 1; i < points.length; i++) {
                    ctx.lineTo(points[i][0], points[i][1]);
               }
               ctx.closePath();//自动闭合路径

               ctx.fill();
               ctx.stroke();
          }

          ctx.beginPath();
          const r = tankdef.radius * scale;
          ctx.arc(x, y, r, 0, 2 * Math.PI);
          ctx.closePath();
          ctx.fillStyle = COLORS.playerFillColor[0];
          ctx.strokeStyle = COLORS.playerStrokeColor[0];
          ctx.fill();
          ctx.stroke();

          ctx.restore();
     }
}