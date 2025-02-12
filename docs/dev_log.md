## 2025年1月4日
- 初始化仓库并导入Box2d
- 增加 `Time` 类,主要用于时间转字符串
- 增加 [`ThreadPool`](https://github.com/az7792/ThreadPool) 类,主要用于异步执行任务
- 增加 `Logger`类，用于打印日志

## 2025年1月5日
- 增加`InetAddress`类，处理ip地址
- 增加`Channel`类，用于管理fd，与其注册的事件
- 增加`Epoll`类，管理Channel
- 增加`EventLoop`类，管理Epoll

## 2025年1月6日
- 增加`SocketOps`，封装了部分socket相关操作
- 更新[`ThreadPool`](https://github.com/az7792/ThreadPool),去掉了管理线程
- 增加`Acceptor`类，用于监听新连接
- 增加`ObjectPool`类，用于管理对象
- DONE: 使用池化技术优化管理连接，避免自己析构自己

## 2025年1月7日
- 增加`Buffer`类，用于管理缓冲区
- 增加测试目录，单独用于测试代码

## 2025年1月8日
- 优化net模块的资源管理，明确对象所属关系[关系图](https://www.processon.com/v/677e7f6ee07760164438b57e)
- `Buffer`增加读文件描述符到缓冲区的方法
- `EventLoop`的`running_`改为`atomic<bool>`，避免在退出loop时出现竞争条件
- 修复`Epoll`析构时未更改Channel的`isInEpoll`状态
- 修复`Channel`fd关闭后无法正常从epoll中移除的问题
     - 添加fdClosed表示fd状态，避免使用fd=-1导致无法正常从epoll中移除
- 完善`ObjectPool`，添加unordered_set以管理已分配的对象,防止意外丢失对象
- 增加`TcpConnection`类 与 `TcpServer`类

## 2025年1月10日
- 优化`Buffer`类，不再判断是否能读取到指定长度的数据，而是读取min(length, 缓冲区可读长度)长度的数据
- `TcpConnection`增加尝试从fd读取数据到buf的公共方法
- 增加`WebSocketFrame`类，用于编解码websocket数据帧(未测试,会与WebSocketServer一并测试)

## 2025年1月11日
- `TcpServer`增加closeConnection方法，用于主动关闭连接
- 增加`WebSocketServer`类，用于处理websocket连接
     - 使用[demo](https://github.com/az7792/ShapeWarsDemo)测试新网络

## 2025年1月13日
- 使用友元优化Channel的成员管理
- 测试的cmake脚本优化，先制作静态库，避免不必要的重复编译
- 增加`SparseSet`类，用于管理小范围非负整数集合
- 增加`SparseMap`类，用于管理小范围非负整数映射

## 2025年1月15日
- 增加`TypeID`类，用于分配唯一的类型ID

## 2025年1月18日
- `SparseSet`,`SpareMap`增加扩容功能
- 完成部分ecs

## 2025年1月19日
- ecs:增加删除实体
- 修复一些bug

## 2025年1月20日
- `ComponentPool`: insert和replace返回Component*
- 增加`Group`用于运行时维护符合某一组组件要求的实体组

## 2025年1月23日
- `Group`增加初始化方法
- 增加System相关方法

## 2025年1月24日
- 前端完成小地图、背景绘制功能与测试

## 2025年1月25日
- 增加显示视野缩放功能,**注意：未限制分辨率,4k看到的画面将是1080p的两倍**
- TODO：优化上述问题
- 增加绘制多边形(点集描述)与正多边形(坐标也外接圆半径描述)的方法
     - 参数均为box2d的坐标系

## 2025年1月27日
- 修复cmake构建无法递归查找源文件的问题
- 修复ecs跨翻译单元导致重复定义的问题

## 2025年1月31日
- ecs添加获取实体ID指针的方法
- TcpConnection修正启动时的断言

## 2025年2月3日
- 修复`EntityManager`中实体(ecs::Entity)列表resize时导致指针失效的问题
- 增加`EntitySet`，专门适用于的实体的稀疏集
- TcpConnection移除冗余的`fdClosed_`变量
- 完成前后端联调测试

## 2025年2月5日
- 后端增加对自定义Ping包的处理
- 前端修复插值失效的bug
- 组件ID采用手动注册的方式
- 实体类型组件Type的id改为uint8_t
- 增加bug列表
- 添加平滑阻尼函数SmoothDampVelocity
- 修改玩家移动的默认速度为0.5m/s
- 增加摄像机跟随玩家

## 2025年2月7日
- 修复前端玩家移动时画面抖动的问题
- 同步当前玩家ID给前端
- 将angle从regularPolygon独立出来
- 摄像机长宽+100px,避免物体在前端画面里突然消失
- 前端优化组件数据读取
- 血量组件从int改为int16_t
- 增加血条绘制
- 增加鼠标位置的同步

## 2025年2月9日
- 修正前端血量值读取顺序
- 后端打包数据分为创建包和更新包
- 加入攻击判定

## 2025年2月10日
- 修正前端血量值读取顺序
- 协议表调整实体类型，并增加碰撞组组件
- 前端调整实体类型读取
- 为bug列表添加编号
- 完成资源方块测试
     - 未添加删除逻辑
- 修复[BUG-004](https://github.com/az7792/ShapeWars/blob/master/docs/bugList.md)

## 2025年2月11日
- 为新连接禁用Nagle算法
- 处理同一时间同一连接到达多个包的情况
- 修复[BUG-001](https://github.com/az7792/ShapeWars/blob/master/docs/bugList.md),产生[BUG-005](https://github.com/az7792/ShapeWars/blob/master/docs/bugList.md)
- `AttackList`更名为`ContactList`

## 2025年2月12日
- 测试子弹
- 添加边界墙