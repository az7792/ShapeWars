// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "net/SocketOps.h"
#include "utils/Logger.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>

int SocketOps::createSocket(bool isBlocking)
{
     if (isBlocking)
          return ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
     else
          return ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
}

int SocketOps::bind(int fd, const InetAddress &addr)
{
     return ::bind(fd, (sockaddr *)&addr.addr, sizeof(sockaddr_in));
}

int SocketOps::listen(int fd, int n)
{
     return ::listen(fd, n);
}

int SocketOps::accept(int fd, InetAddress *addr)
{
     socklen_t len = sizeof(sockaddr_in);
     if (addr != nullptr)
          return ::accept4(fd, (sockaddr *)&addr->addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
     else
          return ::accept4(fd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
}

int SocketOps::connect(int fd, const InetAddress &addr)
{
     return ::connect(fd, (sockaddr *)&addr.addr, sizeof(sockaddr_in));
}

ssize_t SocketOps::send(int fd, const std::string &mes)
{
     return send(fd, mes.c_str(), mes.size());
}

ssize_t SocketOps::send(int fd, const void *mes, size_t len)
{
     return ::send(fd, mes, len, MSG_NOSIGNAL);
}

ssize_t SocketOps::recv(int fd, void *buffer, size_t len)
{
     return ::recv(fd, buffer, len, 0);
}

void SocketOps::setTcpNoDelay(int fd, bool on)
{
     int opt = on ? 1 : 0;
     if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt)) < 0)
          LOG_WARN("sockfd" + std::to_string(fd) + " TCP_NODELAY 设置失败: " + strerror(errno));
}

void SocketOps::setReuseAddr(int fd, bool on)
{
     int opt = on ? 1 : 0;
     if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
          LOG_WARN("sockfd" + std::to_string(fd) + " SO_REUSEADDR 设置失败: " + strerror(errno));
}

void SocketOps::setReusePort(int fd, bool on)
{
     int opt = on ? 1 : 0;
     if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const char *)&opt, sizeof(opt)) < 0)
          LOG_WARN("sockfd" + std::to_string(fd) + " SO_REUSEPORT 设置失败: " + strerror(errno));
}

void SocketOps::setKeepAlive(int fd, bool on)
{
     int opt = on ? 1 : 0;
     if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&opt, sizeof(opt)) < 0)
          LOG_WARN("sockfd" + std::to_string(fd) + " SO_KEEPALIVE 设置失败: " + strerror(errno));
}

void SocketOps::setBlocking(int fd, bool on)
{
     int flags = fcntl(fd, F_GETFL, 0);
     if (flags == -1)
     {
          LOG_ERROR("获取文件状态标志失败");
          return;
     }

     if (on)
          flags &= ~O_NONBLOCK; // 阻塞
     else
          flags |= O_NONBLOCK; // 非阻塞

     if (fcntl(fd, F_SETFL, flags) == -1)
          LOG_ERROR("设置文件状态标志失败");
}