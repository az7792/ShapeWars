// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "net/EventLoop.h"
#include "net/Epoll.h"
#include "net/Channel.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/eventfd.h>

EventLoop::EventLoop() : epoll_(std::make_unique<Epoll>(this)), running_(false)
{
     int fd = eventfd(0, EFD_NONBLOCK); // 用于通知epoll
     if (fd < 0)
          LOG_ERROR("事件循环用于通知退出的fd初始失败" + std::string(strerror(errno)));
     wakeupCh_ = std::make_unique<Channel>(this, fd);

     wakeupCh_->setReadCallback([eventFd = fd]() { // 清空缓冲区
          uint64_t val;
          ::read(eventFd, &val, sizeof(val));
     });
     wakeupCh_->enableReading();
}

EventLoop::~EventLoop()
{
     LOG_INFO("事件循环退出");
}

void EventLoop::loop()
{
     running_ = true;
     LOG_INFO("开启事件循环");
     while (running_)
     {
          // TODO 边沿触发
          std::vector<Channel *> chs = epoll_->wait();
          for (auto &ch : chs)
               ch->handleEvent(); // TODO 用线程池执行
     }
}

bool EventLoop::wakeup()
{
     uint64_t one = 1;
     return write(wakeupCh_->fd, &one, sizeof(one)); // 向wakeupCh_->fd写入一个值，唤醒阻塞的epoll_wait
}

void EventLoop::quit()
{
     running_ = false;
     if (!wakeup())
     {
          LOG_ERROR("通知失败: " + std::string(strerror(errno)));
     }
}

void EventLoop::updateChannel(Channel *ch)
{
     if (ch->isInEpoll)
          epoll_->updateChannel(EPOLL_CTL_MOD, ch); // 如果已经在Epoll里了就修改
     else
          epoll_->updateChannel(EPOLL_CTL_ADD, ch); // 否则就添加到Epoll中
}

void EventLoop::removeChannel(Channel *ch)
{
     epoll_->removeChannel(ch);
}
