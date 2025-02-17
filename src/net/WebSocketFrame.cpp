#include "net/WebSocketFrame.h"
#include <thread>
#include <cstring>
/*
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
*/
WebSocketFrame::WebSocketFrame() : payloadDataBuf(128) {}

void WebSocketFrame::start()
{
     fin = 1;
     masked = false;
     payloadLength = 0;

     handleStatue = 6;
     needReadLen = 1;

     payloadDataBuf.clear();
     processedLen = 0;
}

std::string WebSocketFrame::encode(bool fin, uint8_t opcode, bool masked, const std::string &payloadData)
{
     std::string frame;

     // 第 1 字节：FIN + RSV + Opcode
     uint8_t firstByte = (fin ? 0x80 : 0x00) | (opcode & 0x0F);
     frame.push_back(firstByte);

     // 第 2 字节：Mask 位 + Payload 长度
     uint8_t secondByte = masked ? 0x80 : 0x00;
     size_t payloadLength = payloadData.size();

     // 计算Payload实际存放位置
     if (payloadLength <= 125)
     {
          secondByte |= static_cast<uint8_t>(payloadLength);
          frame.push_back(secondByte);
     }
     else if (payloadLength <= 0xFFFF)
     {
          secondByte |= 126;
          frame.push_back(secondByte);
          frame.push_back((payloadLength >> 8) & 0xFF);
          frame.push_back(payloadLength & 0xFF);
     }
     else
     {
          secondByte |= 127;
          frame.push_back(secondByte);
          for (int i = 7; i >= 0; --i)
          {
               frame.push_back((payloadLength >> (i * 8)) & 0xFF);
          }
     }

     // Masking Key
     if (masked)
     {
          std::vector<char> maskingKey = {0x12, 0x34, 0x56, 0x78}; // Masking Key
          frame.insert(frame.end(), maskingKey.begin(), maskingKey.end());

          // 应用 Mask 到 Payload 数据
          std::string maskedPayload = payloadData;
          for (size_t i = 0; i < payloadLength; ++i)
          {
               maskedPayload[i] ^= maskingKey[i % 4];
          }
          frame += maskedPayload;
     }
     else
     {
          // 不加掩码，直接添加 Payload 数据
          frame += payloadData;
     }

     return frame;
}

// FIN & opcode(1字节)
void WebSocketFrame::handleStatue0(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 0; // FIN & opcode(1字节)
     needReadLen -= buffer.read(headerBuf, needReadLen);
     if (needReadLen == 0)
     {
          fin = (headerBuf[0] & 0x80) != 0; // FIN 位
          opcode = headerBuf[0] & 0x0F;     // Opcode

          if ((fin == 1 && opcode == 0x0) || (fin == 0 && opcode != 0x0))
          {
               handleStatue = 7; // error
               LOG_WARN("FIN 与 opcode不匹配");
               return;
          }

          needReadLen = 1;
          handleStatue1(conn, buffer);
     }
}

// MASK & payload len(1字节)
void WebSocketFrame::handleStatue1(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 1; // MASK & payload len(1字节)
     needReadLen -= buffer.read(headerBuf, needReadLen);
     if (needReadLen == 0)
     {
          masked = (headerBuf[0] & 0x80) != 0; // Mask 位
          payloadLength = headerBuf[0] & 0x7F; // Payload 长度（前7位）
          if (payloadLength < 126)
          {
               if (masked)
               {
                    needReadLen = 4;
                    handleStatue4(conn, buffer);
               }
               else
               {
                    needReadLen = payloadLength;
                    handleStatue5(conn, buffer);
               }
          }
          else if (payloadLength == 126)
          {
               needReadLen = 2;
               handleStatue2(conn, buffer);
          }
          else if (payloadLength == 127)
          {
               needReadLen = 8;
               handleStatue3(conn, buffer);
          }
     }
}

// Extended payload length(2字节)
void WebSocketFrame::handleStatue2(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 2; // Extended payload length(2字节)
     needReadLen -= buffer.read(headerBuf, needReadLen);
     if (needReadLen == 0)
     {
          // 大端->小端
          payloadLength = (static_cast<uint16_t>(headerBuf[0]) << 8) |
                          static_cast<uint16_t>(headerBuf[1]);
          if (masked)
          {
               needReadLen = 4;
               handleStatue4(conn, buffer);
          }
          else
          {
               needReadLen = payloadLength;
               handleStatue5(conn, buffer);
          }
     }
}

// Extended payload length(8字节)
void WebSocketFrame::handleStatue3(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 3; // Extended payload length(8字节)
     needReadLen -= buffer.read(headerBuf, needReadLen);
     if (needReadLen == 0)
     {
          payloadLength = 0;
          for (int i = 0; i < 8; ++i) // 大端->小端
          {
               payloadLength = (payloadLength << 8) | static_cast<uint64_t>(headerBuf[i]);
          }
          if (masked)
          {
               needReadLen = 4;
               handleStatue4(conn, buffer);
          }
          else
          {
               needReadLen = payloadLength;
               handleStatue5(conn, buffer);
          }
     }
}

// Masking key(4字节)
void WebSocketFrame::handleStatue4(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 4; // Masking key(4字节)
     needReadLen -= buffer.read(headerBuf, needReadLen);
     if (needReadLen == 0)
     {
          std::memcpy(maskingKey.data(), headerBuf, 4);
          needReadLen = payloadLength;
          handleStatue5(conn, buffer);
     }
}

// Payload data(n字节)
void WebSocketFrame::handleStatue5(TcpConnection *conn, Buffer &buffer)
{
     handleStatue = 5; // Payload data(n字节)
     needReadLen -= buffer.read(payloadDataBuf, needReadLen);
     if (needReadLen == 0)
     {
          if (masked)
          {
               for (size_t i = 0; i < payloadLength; ++i)
               {
                    *payloadDataBuf[processedLen + i] ^= maskingKey[i % 4];
               }
          }
          processedLen += payloadLength;
          handleStatue = 6; // 处理完毕，准备处理下一帧
     }
}

void WebSocketFrame::decode(TcpConnection *conn, Buffer &buffer)
{
     assert(handleStatue != 7);
     if (fin == 1 && handleStatue == 6) // 之前的帧解析完成
     {
          start();
          handleStatue0(conn, buffer);
     }
     else if (handleStatue == 0)
          handleStatue0(conn, buffer);
     else if (handleStatue == 1)
          handleStatue1(conn, buffer);
     else if (handleStatue == 2)
          handleStatue2(conn, buffer);
     else if (handleStatue == 3)
          handleStatue3(conn, buffer);
     else if (handleStatue == 4)
          handleStatue4(conn, buffer);
     else if (handleStatue == 5)
          handleStatue5(conn, buffer);
}