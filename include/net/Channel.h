#pragma once
#include <stdint.h>
#include <functional>

class EventLoop;
class Acceptor;
class Epoll;
class TcpConnection;

/// @brief 将sockfd 与 EventLoop 联系起来
class Channel
{
     // friend
     friend Acceptor;
     friend EventLoop;
     friend Epoll;
     friend TcpConnection;

     std::function<void()> readCallback_, writeCallback_, errorCallback_; // 回调函数
     /**
      * @brief Channel对应的EventLoop
      * @warning 不要直接用Epoll，因为Epoll是由EventLoop管理的，外部只开放EventLoop的相关接口
      */
     EventLoop *loop_;
     /// @brief 更新当前订阅的事件状态
     void update();

     int fd; // Channel对应的fd
     /**
      * @brief fd是否已经关闭
      * 如果已经关闭则不需要再close，但需要从epoll树上移除，所有不能简单将fd设置为-1
      */
     bool fdClosed = true;
     uint32_t events = 0;    // 订阅的事件
     uint32_t revents;       // 实际发生的事件
     bool isInEpoll = false; // 是否已经注册到Epoll中
public:

     Channel(EventLoop *loop, int fd);
     ~Channel();

     /// 关闭Channel
     void close();

     void setReadCallback(const std::function<void()> &cb);
     void setWriteCallback(const std::function<void()> &cb);
     void setErrorCallback(const std::function<void()> &cb);
     void setEventLoop(EventLoop *loop);

     /// @brief 将当前channel从epoll中注销
     void remove();

     /// @brief 事件触发时处理已注册的回调函数
     void handleEvent();

     void enableReading();
     void enableWriting();
     void enableErroring();
     void disableReading();
     void disableWriting();
     void disableErroring();
     void disableAll();
};