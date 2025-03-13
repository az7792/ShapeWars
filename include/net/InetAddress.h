// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <stdexcept>

/// @brief 主要用于字符串格式的ip转sockaddr_in
class InetAddress
{
public:
     // 包含端口(uint16_t)和ip(uint32_t)
     sockaddr_in addr;
     InetAddress(std::string ip = "127.0.0.1", uint16_t port = 0);
     InetAddress(const sockaddr_in &addr);

     /// @brief 获取 IP 点分十进制字符串
     std::string getIp() const;
     /// @brief 获取端口
     uint16_t getPort() const;
     /// @brief 获取 IP:端口
     std::string getIpPort() const;
};