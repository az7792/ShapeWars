// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

/**
 * 定长数组
 * 超出长度直接舍弃队头
 */
class Queue {
     constructor(maxSize, initVal) {
          this.maxSize = maxSize; // 队列的最大长度
          this.queue = new Array(maxSize).fill(initVal);
          this.head = 0;
          this.tail = 0;
          this.length = 0;
     }

     empty() {
          return this.length === 0;
     }

     full() {
          return this.length === this.maxSize;
     }

     push(value) {
          if (this.full()) {
               // 如果队列已满,直接移除队头元素
               this.head = (this.head + 1) % this.maxSize;
               this.length--;
          }
          this.queue[this.tail] = value;
          this.tail = (this.tail + 1) % this.maxSize;
          this.length++;
     }

     pop() {
          if (this.empty()) {
               return null;
          }
          const value = this.queue[this.head];
          this.head = (this.head + 1) % this.maxSize;
          this.length--;
          return value;
     }

     at(index) {
          return this.queue[(this.head + index) % this.maxSize];
     }

     // 获取队列中的元素的迭代器
     *[Symbol.iterator]() {
          for (let i = 0; i < this.length; i++) {
               yield this.queue[(this.head + i) % this.maxSize];
          }
     }
}
