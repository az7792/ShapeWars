class ToggleSwitch {
     constructor(container, { onEnable = () => { }, onDisable = () => { }, defaultOn = false } = {}) {
          // 创建 DOM 元素
          this.label = document.createElement('label');
          this.label.className = 'switch';

          this.checkbox = document.createElement('input');
          this.checkbox.type = 'checkbox';
          this.checkbox.checked = defaultOn;

          this.slider = document.createElement('span');
          this.slider.className = 'slider';

          this.label.appendChild(this.checkbox);
          this.label.appendChild(this.slider);
          container.appendChild(this.label);

          // 回调
          this.onEnable = onEnable;
          this.onDisable = onDisable;

          // 监听事件
          this.checkbox.addEventListener('change', () => {
               if (this.checkbox.checked) {
                    this.onEnable();
               } else {
                    this.onDisable();
               }
          });

          // 初始化调用一次回调
          if (defaultOn) this.onEnable(); else this.onDisable();
     }

     isOn() {
          return this.checkbox.checked;
     }

     setOn(value) {
          const current = this.isOn();
          this.checkbox.checked = value;
          if (value !== current) {
               // 触发回调
               if (value) this.onEnable(); else this.onDisable();
          }
     }
}