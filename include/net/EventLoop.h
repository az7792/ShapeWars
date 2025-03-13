// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <memory>
#include <atomic>
class Epoll;
class Channel;

class EventLoop
{
     std::unique_ptr<Epoll> epoll_; // 管理的Epoll
     std::atomic<bool> running_;    // eventLoop是否正在运行

     /**
      * @brief 用于通知唤醒epoll的Channel
      * 通过注册一个用于通知的fd，在需要通知时向该fd写内容，让loop解除阻塞
      */
     std::unique_ptr<Channel> wakeupCh_;

public:
     EventLoop();
     ~EventLoop();

     /// @brief 开启事件循环
     void loop();

     /// @brief 通知epoll进行一次唤醒
     bool wakeup();

     /// @brief 退出事件循环
     void quit();

     /// @brief 更新当前channel订阅的事件状态
     void updateChannel(Channel *ch);
     /// @brief 将当前channel从epoll中注销
     void removeChannel(Channel *ch);
};