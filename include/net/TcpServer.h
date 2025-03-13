// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "utils/ObjectPool.h"
#include <vector>
#include <csignal>
#include <atomic>
#include <thread>

class TcpServer
{
     std::atomic<int> numSubLoop_;
     std::atomic<int> nowIndex_ = -1;

     InetAddress localAddr_;
     EventLoop acceptorLoop_; //[1]  //顺序不能交换，1->2->3->4的顺序初始化，c++按声明顺序初始化
     Acceptor acceptor_;      //[2]  //顺序不能交换，4->3->2->1的顺序析构，c++按声明顺序相反的顺序析构

     std::vector<EventLoop> subLoops_;                   // 子事件循环[3]
     std::vector<std::unique_ptr<std::thread>> threads_; // 子事件循环线程

     ObjectPool<TcpConnection> connectionPool_; // 连接池[4]

     std::function<void(TcpConnection *)> newConnectionCallback_, onCloseCallback_, onErrorCallback_;
     std::function<void(TcpConnection *, Buffer &)> onMessageCallback_;

     bool isRunning_;

     /// 获取下一个子事件循环的索引
     int nextIndex();

     /// 处理新连接
     void newConnection(int sockfd, const InetAddress &addr);

     /// 处理连接断开
     void handleClose(TcpConnection *tc);

public:
     /// @param addr 本地地址
     /// @param numSubLoop 子事件循环数量
     TcpServer(const InetAddress &addr, int numSubLoop = 2);
     ~TcpServer();

     void setNewConnectionCallback(std::function<void(TcpConnection *)> cb);
     void setOnCloseCallback(std::function<void(TcpConnection *)> cb);
     void setOnErrorCallback(std::function<void(TcpConnection *)> cb);
     void setOnMessageCallback(std::function<void(TcpConnection *, Buffer &)> cb);

     /// @brief 启动服务器
     void run();

     /// @brief 关闭服务器
     void close();

     /// 主动断开一个连接
     void closeConnection(TcpConnection *tc);
};