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