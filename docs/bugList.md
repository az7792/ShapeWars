- [x] `001`: 前端如果异步发送消息，会造成巨大延迟
     - 由于前端可能同一时间发送多个包，而server端一次读取多个包(从内核态读到用户态)但只处理一个包(从用户态buf中读出来处理)导致包一直堆积
- [x] `002`: 由于摄像机跟随导致插值抖动
- [x] `003`: 斜向移动还有轻微抖动
- [x] `004`：前端发送生成实体会造成严重卡顿
     - 由于绘制血条时ctx栈重复save(push)导致的
- [ ] `005`: 001修复后没有巨大延迟,但ping值还是会有波动,**但部署在服务器上并不会产生波动**
     - 表现为ping值在[0 - TPS/1000]之间波动
- [x] `006`: server端TPS设置太低(<25)会导致摄像机接近停止的目标时产生振荡
- [x] `007`: 前端意外关闭会同时触发close和error导致重复delete玩家实体
- [x] `008`: 重生时摄像机跟随过慢
- [x] `009`: 公转系统由于Box2D的限制，速度较低时导致某一帧过后速度直接变为0
     - 表现为实体运动会一顿一顿，太低甚至直接停止运动
     - 可能需要调整世界比例1m:500px -> 1m:50px
- [x] `010`: 炮管发射间隔<=2tick会导致炮管动画与预期不符
- [x] `011`: 前端有概率小地图背景加载不出来
- [ ] `012`：炮管旋转会有抖动，可能是外推导致的
- [ ] `013`: 某些情况下tankdefs.json与实际呈现的内容不一致