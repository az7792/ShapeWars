class AdjustableInterval {
     constructor(callback, initialInterval) {
          this.callback = callback;
          this.interval = initialInterval;      // 当前生效的间隔
          this.pendingInterval = null;          // 待生效的新间隔
          this.timeoutId = null;
          this.nextRun = null;
     }

     start() {
          if (this.timeoutId !== null) return;
          this.nextRun = Date.now() + this.interval;
          this.schedule();
     }

     stop() {
          clearTimeout(this.timeoutId);
          this.timeoutId = null;
          this.pendingInterval = null;          // 清空待生效间隔
     }

     // 修改间隔（不取消当前定时器）
     setInterval(newInterval) {
          this.pendingInterval = newInterval;   // 存储待生效间隔
     }

     schedule() {
          const now = Date.now();
          let delay = this.nextRun - now;
          if (delay < 0) delay = 0;

          this.timeoutId = setTimeout(() => this.execute(), delay);
     }

     execute() {
          // 检查是否有待生效的间隔，若有则更新
          if (this.pendingInterval !== null) {
               this.interval = this.pendingInterval;
               this.pendingInterval = null;
          }

          this.callback();
          this.nextRun = Date.now() + this.interval; // 使用最新间隔
          this.schedule();
     }
}