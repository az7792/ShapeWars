// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/InetAddress.h"

/**
 * @brief 用于监听新连接到来,并调用设置的回调函数
 * EventLoop不管理Acceptor的生命周期，析构EventLoop前需要先析构Acceptor
 * @warning 析构顺序: acceptor -> eventloop
 */
class Acceptor
{
     EventLoop *mainLoop_;                    // 所在主事件循环
     std::unique_ptr<Channel> acceptChannel_; // 用于监听的Channel
     std::function<void(int sockfd, const InetAddress &addr)> newConnectionCallback_;

     /// @brief 处理新连接到来事件
     void handleRead();

public:
     /// @param loop 所在主事件循环
     /// @param listenAddr 用于监听的地址
     Acceptor(EventLoop *loop, const InetAddress &listenAddr);
     ~Acceptor();

     /// @brief 设置新连接到来时的回调函数
     void setNewConnectionCallback(std::function<void(int sockfd, const InetAddress &addr)> cb);
};