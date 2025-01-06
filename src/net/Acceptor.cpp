#include "net/Acceptor.h"
#include "net/SocketOps.h"
#include "utils/Logger.h"
#include <string.h>

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
    : mainLoop_(loop), acceptFd_(SocketOps::createSocket(true)),
      acceptChannel_(new Channel(loop, acceptFd_))
{
     SocketOps::setReuseAddr(acceptFd_, true);                                /// 允许端口复用
     SocketOps::setReusePort(acceptFd_, true);                                /// 允许套接字复用
     SocketOps::bind(acceptFd_, listenAddr);                                  /// 绑定地址
     acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this)); /// 设置读回调函数
     acceptChannel_->enableReading();                                         /// 注册可读事件
     SocketOps::listen(acceptFd_, 128);                                       /// 开始监听
}

Acceptor::~Acceptor()
{
     acceptChannel_->remove();
     delete acceptChannel_;
}

void Acceptor::setNewConnectionCallback(std::function<void(int sockfd, const InetAddress &addr)> cb)
{
     newConnectionCallback_ = cb;
}

void Acceptor::handleRead()
{
     InetAddress addr;
     int cfd = SocketOps::accept(acceptFd_, &addr); // 该accept函数会阻塞等待新连接
     if (cfd < 0)
          LOG_ERROR("sockfd" + std::to_string(cfd) + " accept 失败: " + strerror(errno));
     else if (newConnectionCallback_)
          newConnectionCallback_(cfd, addr);
}