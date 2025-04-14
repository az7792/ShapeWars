class PointDistributionUI {
     constructor(attributes, containerId, toggleBtnId) {
          this.attributes = attributes;
          this.container = document.getElementById(containerId);
          this.toggleBtn = document.getElementById(toggleBtnId);
          this.init();
     }

     // 重置所有属性值
     reset() {
          this.attributes.forEach((attr, index) => {
               attr.value = 0;

               // 更新进度条块
               const blocks = this.container.querySelectorAll(`.pd-attribute-item:nth-child(${index + 1}) .pd-progress-block`);
               blocks.forEach((block, i) => block.classList.remove('active'));

               // 更新按钮状态
               const buttons = this.container.querySelectorAll(`.pd-attribute-item:nth-child(${index + 1}) .pd-btn`);
               buttons[0].disabled = true;
               buttons[1].disabled = false;
          });
     }

     init() {
          this.toggleBtn.addEventListener('click', () => this.toggleUI());
          this.renderAttributes();
     }

     toggleUI() {
          this.container.classList.toggle('active');
     }

     renderAttributes() {
          this.attributes.forEach((attr, index) => {
               this.container.appendChild(this.createAttributeElement(attr, index));
          });
     }

     createAttributeElement(attr, index) {
          const container = document.createElement('div');
          container.className = 'pd-attribute-item';

          // 创建进度条块
          const progressBlocks = Array.from({ length: attr.max }, (_, i) => {
               const block = document.createElement('div');
               block.className = `pd-progress-block${i < attr.value ? ' active' : ''}`;
               block.style.setProperty('--pd-progress-color', attr.progressColor);
               return block;
          });

          container.innerHTML = `
          <button class="pd-btn decrement" onclick="pointUI.sendValue(-1,${index})" disabled>-</button>
          <div class="pd-progress-container">
               ${progressBlocks.map(b => b.outerHTML).join('')}
               <span class="pd-attribute-name">${attr.name}</span>
          </div>
          <button class="pd-btn increment" onclick="pointUI.sendValue(1,${index})">+</button>
          `;

          // 设置按钮颜色
          const buttons = container.querySelectorAll('.pd-btn');
          buttons.forEach(button => {
               button.style.setProperty('--pd-btn-color', attr.btnColor);
          });

          return container;
     }

     sendValue(delta, index) {
          let message = new Uint8Array(2);
          message[0] = 0x05;
          message[1] = (delta == 1 ? (index | 0x80) : (index & 0x7F));
          sendMessage(message);
     }

     updateValue(delta, index) {
          const attr = this.attributes[index];
          const newValue = Math.min(Math.max(attr.value + delta, 0), attr.max);
          if (newValue === attr.value) return;

          attr.value = newValue;

          // 更新进度条块
          const blocks = this.container.querySelectorAll(`.pd-attribute-item:nth-child(${index + 1}) .pd-progress-block`);
          blocks.forEach((block, i) => block.classList.toggle('active', i < newValue));

          // 更新按钮状态
          const buttons = this.container.querySelectorAll(`.pd-attribute-item:nth-child(${index + 1}) .pd-btn`);
          buttons[0].disabled = newValue === 0;  // -按钮
          buttons[1].disabled = newValue === attr.max;  // +按钮
     }
}