#pragma once
#include "Channel.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "utils/Buffer.h"

/**
 * @brief
 * @warning 必须start()后才能使用
 */
class TcpConnection
{
private:
     std::unique_ptr<Channel> channel_;

     Buffer inputBuffer_;
     // Buffer outputBuffer_;//TODO
     bool abled_ = false; // 该对象是否可用

     std::function<void(TcpConnection *, Buffer &)> readCallback_;
     std::function<void(TcpConnection *)> closeCallback_, errorCallback_;
     std::function<void()> writeCallback_; // 回调函数

     void handleRead();
     void handleClose();
     void handleWrite();
     void handleError();

public:
     InetAddress localAddr;
     InetAddress peerAddr;

public:
     TcpConnection();
     ~TcpConnection() = default;

     // 使用前初始化
     void start(EventLoop *loop, int fd, InetAddress localAddr, InetAddress peerAddr);
     // 结束使用，重置到初始前的状态
     void end();

     // 发送数据
     int send(const std::string &buf);

     /// @brief 尝试从fd读取数据到缓冲区
     int readFd();

     // 断开连接
     void close();

     // 设置fd状态
     void setFdStatus(bool closed);

public: // SETTERS
     void setReadCallback(const std::function<void(TcpConnection *, Buffer &)> &cb);
     void setCloseCallback(const std::function<void(TcpConnection *)> &cb);
     void setErrorCallback(const std::function<void(TcpConnection *)> &cb);
     void setWriteCallback(const std::function<void()> &cb);

public: // GETTERS
     bool getAbled() const;
};