// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <vector>
#include "utils/Logger.h"

class Channel;
class EventLoop;

class Epoll
{
     int epollFd_;                         // epollID
     EventLoop *loop_;                     // epoll所在的EventLoop
     std::mutex mutex_;                    // channelMap_与epoll树的互斥锁
     std::map<int, Channel *> channelMap_; // 所有的ChannelPtr
     std::vector<epoll_event> events_;     // 存放epoll_wait返回的事件
     int maxevents_ = 16;                  // epoll_wait返回的事件的最大长度

public:
     Epoll(EventLoop *loop);
     ~Epoll();

     /// @brief
     /// @param timeout 最长等待时间,-1为一直等待
     /// @return 返回有事件发生的Channel
     std::vector<Channel *> wait(int timeout = -1);

     /// @brief 更新channel
     /// @param op (更新)EPOLL_CTL_MOD 添加(EPOLL_CTL_ADD)
     void updateChannel(int op, Channel *ch);

     /// @brief 将channel移除
     void removeChannel(Channel *ch);
};