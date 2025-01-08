#include "net/Channel.h"
#include "net/EventLoop.h"
#include <sys/epoll.h> //仅用于使用EPOLLIN，EPOLLOUT等宏
#include <unistd.h>

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd(fd) {}

Channel::~Channel()
{
     close();
}

void Channel::close()
{
     // 从epoll中注销
     disableAll();
     if (isInEpoll)
          this->remove();
     if (fd != -1)
          ::close(fd);
     loop_ = nullptr;
}

void Channel::setReadCallback(const std::function<void()> &cb)
{
     readCallback_ = cb;
}

void Channel::setWriteCallback(const std::function<void()> &cb)
{
     writeCallback_ = cb;
}

void Channel::setErrorCallback(const std::function<void()> &cb)
{
     errorCallback_ = cb;
}

void Channel::setEventLoop(EventLoop *loop)
{
     loop_ = loop;
}

void Channel::remove()
{
     if (loop_ != nullptr && fd != -1)
          loop_->removeChannel(this);
}

void Channel::update()
{
     if (loop_ != nullptr && fd != -1)
          loop_->updateChannel(this);
}

void Channel::handleEvent()
{
     if (loop_ == nullptr || fd == -1)
          return;
     // 实际发生了 且 注册了 且 设置了回调函数 才会最终调用回调函数
     if ((revents & EPOLLERR) && (events & EPOLLERR) && errorCallback_)
     {
          errorCallback_();
     }
     if ((revents & EPOLLIN) && (events & EPOLLIN) && readCallback_)
     {
          readCallback_();
     }
     if ((revents & EPOLLOUT) && (events & EPOLLOUT) && writeCallback_)
     {
          writeCallback_();
     }

     revents = 0;
}

void Channel::enableReading()
{
     events |= EPOLLIN;
     update();
}

void Channel::enableWriting()
{
     events |= EPOLLOUT;
     update();
}

void Channel::enableErroring()
{
     events |= EPOLLERR;
     update();
}

void Channel::disableReading()
{
     events &= ~EPOLLIN;
     update();
}

void Channel::disableWriting()
{
     events &= ~EPOLLOUT;
     update();
}

void Channel::disableErroring()
{
     events &= ~EPOLLERR;
     update();
}

void Channel::disableAll()
{
     events = 0;
     update();
}