#pragma once
#include <stdint.h>
#include <functional>

class EventLoop;

/// @brief 将sockfd 与 EventLoop 联系起来
class Channel
{
     std::function<void()> readCallback_, writeCallback_, errorCallback_; // 回调函数
     /**
      * @brief Channel对应的EventLoop
      * @warning 不要直接用Epoll，因为Epoll是由EventLoop管理的，外部只开放EventLoop的相关接口
      */
     EventLoop *loop_;
     /// @brief 更新当前订阅的事件状态
     void update();

public:
     int fd;                 // Channel对应的fd
     uint32_t events = 0;    // 订阅的事件
     uint32_t revents;       // 实际发生的事件
     bool isInEpoll = false; // 是否已经注册到Epoll中

     Channel(EventLoop *loop, int fd);
     ~Channel();

     void setReadCallback(const std::function<void()> &cb);
     void setWriteCallback(const std::function<void()> &cb);
     void setErrorCallback(const std::function<void()> &cb);

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