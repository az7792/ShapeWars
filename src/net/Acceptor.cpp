#include "net/Acceptor.h"
#include "net/SocketOps.h"
#include "utils/Logger.h"
#include <string.h>

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
    : mainLoop_(loop), acceptChannel_(std::make_unique<Channel>(loop, SocketOps::createSocket(true)))
{
     SocketOps::setReuseAddr(acceptChannel_->fd, true);                       /// 允许端口复用
     SocketOps::setReusePort(acceptChannel_->fd, true);                       /// 允许套接字复用
     SocketOps::bind(acceptChannel_->fd, listenAddr);                         /// 绑定地址
     acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this)); /// 设置读回调函数
     acceptChannel_->enableReading();                                         /// 注册可读事件
     SocketOps::listen(acceptChannel_->fd, 128);                              /// 开始监听
}

Acceptor::~Acceptor()
{
     acceptChannel_->remove();
}

void Acceptor::setNewConnectionCallback(std::function<void(int sockfd, const InetAddress &addr)> cb)
{
     newConnectionCallback_ = cb;
}

void Acceptor::handleRead()
{
     InetAddress addr;
     int cfd = SocketOps::accept(acceptChannel_->fd, &addr); // 该accept函数会阻塞等待新连接
     if (cfd < 0)
          LOG_ERROR("sockfd" + std::to_string(cfd) + " accept 失败: " + strerror(errno));
     else if (newConnectionCallback_)
          newConnectionCallback_(cfd, addr);
}