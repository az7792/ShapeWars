#include "net/Epoll.h"
#include "net/Channel.h"

Epoll::Epoll(EventLoop *loop) : epollFd_(epoll_create1(EPOLL_CLOEXEC)), // 调用 exec() 时，该文件描述符会被自动关闭
                                loop_(loop)
{
     if (epollFd_ < 0)
          LOG_FATAL("Epoll创建失败" + std::string(strerror(errno)));
     events_.resize(maxevents_);
}

Epoll::~Epoll()
{
     // 使用系统调用关闭epoll
     ::close(epollFd_);
}

std::vector<Channel *> Epoll::wait(int timeout)
{
     std::vector<Channel *> chs;
     while (true)
     {
          // events_.data()的使用长度最多不超过maxevents_
          int nfds = epoll_wait(epollFd_, events_.data(), maxevents_, timeout);
          if (nfds > 0)
          {
               chs.reserve(nfds); // 预留空间
               for (int i = 0; i < nfds; ++i)
               {
                    static_cast<Channel *>(events_[i].data.ptr)->revents = events_[i].events; // 更新实际发生的事件
                    chs.push_back(static_cast<Channel *>(events_[i].data.ptr));
               }
               if (maxevents_ == nfds)
               {
                    // 如果还有事件没处理完在EventLoop的下一次调用Epoll::wait时会返回
                    events_.resize(2 * maxevents_); // 借助vector进行扩容
               }
          }
          else if (nfds == 0)
          {
               LOG_DEBUG("epoll wait超时");
          }
          else
          {
               if (errno == EINTR)
                    continue; // 信号中断,重试
               else
                    LOG_ERROR("Epoll wait失败" + std::string(strerror(errno)));
          }
          break;
     }
     return chs;
}

void Epoll::updateChannel(int op, Channel *ch)
{
     epoll_event ev;
     ev.events = ch->events;
     ev.data.ptr = ch;

     std::lock_guard<std::mutex> lock(mutex_);
     if (epoll_ctl(epollFd_, op, ch->fd, &ev) == -1)
          LOG_ERROR("epoll_ctl失败:" + std::string(strerror(errno)));

     if (op == EPOLL_CTL_ADD)
     {
          channelMap_.emplace(ch->fd, ch);
          ch->isInEpoll = true;
     }
     else if (op == EPOLL_CTL_DEL)
     {
          channelMap_.erase(ch->fd);
          ch->isInEpoll = false;
     }
}

void Epoll::removeChannel(Channel *ch)
{
     if (ch->isInEpoll)
          updateChannel(EPOLL_CTL_DEL, ch);
}
