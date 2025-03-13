// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "net/InetAddress.h"

InetAddress::InetAddress(std::string ip, uint16_t port)
{
     addr = {0};
     addr.sin_family = AF_INET;
     addr.sin_port = htons(port);
     addr.sin_addr.s_addr = inet_addr(ip.c_str());
     if (addr.sin_addr.s_addr == INADDR_NONE)
     {
          throw std::invalid_argument("无效IP地址:" + ip);
     }
}

InetAddress::InetAddress(const sockaddr_in &addr) : addr(addr) {}

std::string InetAddress::getIp() const { return inet_ntoa(addr.sin_addr); }

uint16_t InetAddress::getPort() const { return ntohs(addr.sin_port); }

std::string InetAddress::getIpPort() const { return getIp() + ":" + std::to_string(getPort()); }
