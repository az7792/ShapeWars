// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "InetAddress.h"
namespace SocketOps
{
     /// 创建一个socket，默认为非阻塞
     int createSocket(bool isBlocking = false);

     /// 绑定到指定地址
     int bind(int fd, const InetAddress &addr);

     /// 监听
     int listen(int fd, int n = 128);

     /**
      * @brief 接收到的fd默认为非阻塞
      * 地址通过addr传出
      */
     int accept(int fd, InetAddress *addr);

     /// 连接到指定服务器
     int connect(int fd, const InetAddress &addr);

     /// 通过string发送
     ssize_t send(int fd, const std::string &mes);

     /// 通过char*发送
     ssize_t send(int fd, const void *mes, size_t len);

     ///  接收
     ssize_t recv(int fd, void *buffer, size_t len);

     /// 设置是否禁用 Nagle 算法。 true为关闭 false为开启
     void setTcpNoDelay(int fd, bool on);

     /// 设置是否允许地址重用
     void setReuseAddr(int fd, bool on);

     /// 设置是否允许端口重用
     void setReusePort(int fd, bool on);

     /// 设置是否启用 TCP 保活机制
     void setKeepAlive(int fd, bool on);

     /// 设置是否启用阻塞 I/O，true 为启用，false 为非阻塞
     void setBlocking(int fd, bool on);
} // namespace SocketOps