#pragma once
#include "ecs/EntityManager.h"
#include "net/WebSocketServer.h"
#include "box2d/box2d.h"
#include <shared_mutex>

class GameLoop
{
private:
     // 处理网络接收的输入与ecs的同步
     void inputSys();
     // 处理ecs的输出与网络发送的同步
     void outputSys();
     // 处理延迟创建玩家实体
     void createPlayerSys();
     // 处理延迟销毁实体
     void destroyEntitySys();
     // 延迟删除形状映射
     void delayDeleteShapesSys();

private:
     struct atomicInput
     {
          std::atomic<float> x, y;
          std::atomic<uint64_t> state;
     };
     std::array<atomicInput, MAX_CONNECTED> input_; // 玩家输入
     std::deque<int> freeInputsQueue_;              // 空闲输入队列

private:
     ecs::EntityManager em_;                          // 实体管理器
     WebSocketServer ws_;                             // WebSocket服务器
     b2WorldId worldId_;                              // Box2D世界ID
     std::atomic<bool> isRunning_;                    // 游戏是否正在运行
     uint32_t tick_;                                  // 当前游戏tick
     std::chrono::steady_clock::time_point lastTime_; // 上次刷新的时间

     std::unordered_map<TcpConnection *, ecs::Entity> playerMap_; // 玩家连接到实体的映射
     std::unordered_map<ecs::Entity, int> inputMap_;              // 玩家输入映射
     std::mutex playerAndInputMapMutex_;                          // 玩家映射锁

     std::deque<TcpConnection *> createPlayerQueue_;  // 等待创建玩家的队列
     std::deque<TcpConnection *> destroyPlayerQueue_; // 等待销毁玩家的队列
     std::mutex createPlayerQueueMutex_;              // 创建玩家队列锁
     std::mutex destroyPlayerQueueMutex_;             // 销毁玩家队列锁

     void handleOnMessage(TcpConnection *conn, Buffer &buffer); // 处理WebSocket新消息事件
     void handleOnClose(TcpConnection *conn);                   // 处理WebSocket关闭事件
     void handleOnError(TcpConnection *conn);                   // 处理WebSocket错误事件
     void handleOnOpen(TcpConnection *conn);                    // 处理WebSocket打开事件

public:
     GameLoop();
     ~GameLoop();
     void run();
     void exit();
     bool isRunning() const;
};
