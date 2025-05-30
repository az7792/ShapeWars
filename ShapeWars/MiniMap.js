// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

// 存放小地图和背景地图数据
class MiniMap {
     constructor(width, height) {
          this.width = width;
          this.height = height;

          this.canvas = document.createElement('canvas');
          this.canvas.width = width;
          this.canvas.height = height;

          this.ctx = this.canvas.getContext('2d');
          this.fillRect(0, 0, width, height, COLORS.backgroundValid);
     }

     // 填充矩形区域
     fillRect(x, y, w, h, rgba) {
          if (Array.isArray(rgba)) {
               this.ctx.fillStyle = `rgba(${rgba[0]}, ${rgba[1]}, ${rgba[2]}, ${rgba[3]})`;
          } else if (typeof rgba === 'string') {
               this.ctx.fillStyle = rgba;
          }

          this.ctx.fillRect(x, y, w, h);
     }

     // 获取指定位置的 RGBA 值
     getPixel(x, y) {
          const imageData = this.ctx.getImageData(x, y, 1, 1);
          const data = imageData.data;  // 返回一个数组 [R, G, B, A]
          return data;
     }

     reset(width, height) {
          this.width = width;
          this.height = height;
          
          this.canvas.width = width;
          this.canvas.height = height;

          this.ctx = this.canvas.getContext('2d');
          this.fillRect(0, 0, width, height, COLORS.backgroundValid);

          //test start
          // for (let i = 0; i < 90; i++) {
          //      miniMap.fillRect(0, i, 100, 1, [2 * i, 255 - 2 * i, 100 + i, 255]);
          // }
          // miniMap.fillRect(0, 0, 5, 95, [11, 24, 14, 255]);
          //test end
     }
}