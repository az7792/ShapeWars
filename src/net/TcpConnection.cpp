#include "net/TcpConnection.h"
#include "net/SocketOps.h"
#include <cassert>

void TcpConnection::handleRead()
{
     int n = inputBuffer_.readFd(channel_->fd);
     if (n > 0)
     {
          if (readCallback_)
               readCallback_(this, inputBuffer_);
     }
     else if (n == 0)
     {
          handleClose();
     }
     else
     {
          handleError();
     }
}

void TcpConnection::handleWrite()
{
     if (writeCallback_)
          writeCallback_();
}

void TcpConnection::handleClose()
{
     if (closeCallback_)
          closeCallback_(this);
}

void TcpConnection::handleError()
{
     if (errorCallback_)
          errorCallback_(this);
}

TcpConnection::TcpConnection() : channel_(std::make_unique<Channel>(nullptr, -1))
{
     channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
     channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
     channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

void TcpConnection::start(EventLoop *loop, int fd, InetAddress localAddr, InetAddress peerAddr)
{
     assert(channel_->isInEpoll == false && channel_->fd == -1);
     channel_->fd = fd;
     channel_->fdClosed = false;
     channel_->setEventLoop(loop);

     channel_->enableReading();
     // channel_->enableWriting();

     this->localAddr = localAddr;
     this->peerAddr = peerAddr;
     abled_ = true;
     fdClosed_ = false;
}

void TcpConnection::end()
{
     close();
     abled_ = false;
     readCallback_ = nullptr;
     closeCallback_ = nullptr;
     writeCallback_ = nullptr;
     errorCallback_ = nullptr;
}

int TcpConnection::send(const std::string &buf)
{
     return SocketOps::send(channel_->fd, buf);
}

void TcpConnection::close()
{
     channel_->close();
}

void TcpConnection::setFdStatus(bool closed)
{
     channel_->fdClosed = closed;
}

void TcpConnection::setReadCallback(const std::function<void(TcpConnection *, Buffer &)> &cb)
{
     readCallback_ = cb;
}

void TcpConnection::setWriteCallback(const std::function<void()> &cb)
{
     writeCallback_ = cb;
}

void TcpConnection::setErrorCallback(const std::function<void(TcpConnection *)> &cb)
{
     errorCallback_ = cb;
}

void TcpConnection::setCloseCallback(const std::function<void(TcpConnection *)> &cb)
{
     closeCallback_ = cb;
}

bool TcpConnection::getAbled() const
{
     return abled_;
}
