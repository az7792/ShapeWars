#pragma once
#include <string>
#include <vector>
#include "utils/Logger.h"
#include "utils/Buffer.h"
#include "TcpConnection.h"

class WebSocketFrame
{
public:
     bool fin = 1;                   // FIN 位
     uint8_t opcode;                 // Opcode
     bool masked;                    // 是否启用 Mask
     uint64_t payloadLength;         // Payload 长度
     std::array<char, 4> maskingKey; // Masking Key（如果有）

     /**
      * 帧头解析状态
      * 如果处理之前FIN==1 && handleStatue==6，则说明之前的帧解析完成
      * 0 : FIN & opcode(1字节)
      * 1 : MASK & payload len(1字节)
      * 2 : Extended payload length(2字节)
      * 3 : Extended payload length(8字节)
      * 4 : Masking key(4字节)
      * 5 : Payload data(n字节)
      * 6 : over(当前帧解析完成)
      * 7 : error(解析错误)
      */
     uint8_t handleStatue = 6;
     uint64_t needReadLen = 1; // 当前阶段还需读取的字节数

     char headerBuf[8];     // 帧头缓冲区
     Buffer payloadDataBuf; // 负载数据缓冲区
     int processedLen = 0;  // 已经处理完的数据长度(主要指之前的继续帧)

     WebSocketFrame();

     void start();

     /**
      *  生成 WebSocket 帧字符串
      *  @note 不支持数据包切片
      */
     static std::string encode(bool fin, uint8_t opcode, bool masked, const std::string &payloadData);

     /// 解析 WebSocket 帧
     void decode(TcpConnection *conn, Buffer &buffer);
private:

     // FIN & opcode(1字节)
     void handleStatue0(TcpConnection *conn, Buffer &buffer);
     // MASK & payload len(1字节)
     void handleStatue1(TcpConnection *conn, Buffer &buffer);
     // Extended payload length(2字节)
     void handleStatue2(TcpConnection *conn, Buffer &buffer);
     // Extended payload length(8字节)
     void handleStatue3(TcpConnection *conn, Buffer &buffer);
     // Masking key(4字节)
     void handleStatue4(TcpConnection *conn, Buffer &buffer);
     // Payload data(n字节)
     void handleStatue5(TcpConnection *conn, Buffer &buffer);
};