#include "net/WebSocketFrame.h"
#include <thread>
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
WebSocketFrame::WebSocketFrame(TcpConnection *conn, Buffer &buffer)
{
     decode(conn, buffer);
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

void WebSocketFrame::decode(TcpConnection *conn, Buffer &buffer)
{
     char twoBytes[2];                                    // 前两个字节
     bool readOk = tryRead(conn, buffer, twoBytes, 2, 3); // 尝试读取前两个字节,最多尝试3次
     if (!readOk)
     {
          ok = false;
          LOG_WARN("读取失败数据不足(2Bytes)");
          return;
     }
     // 读取第 1 字节
     fin = (twoBytes[0] & 0x80) != 0; // FIN 位
     opcode = twoBytes[0] & 0x0F;     // Opcode
     if (fin == 1 && opcode == 0x0)
     {
          ok = false;
          LOG_WARN("FIN 与 opcode不匹配");
          return;
     }
     // 读取第 2 字节
     masked = (twoBytes[1] & 0x80) != 0;             // Mask 位
     uint8_t initialPayloadLen = twoBytes[1] & 0x7F; // Payload 长度（前7位）

     // 处理扩展长度
     if (initialPayloadLen == 126)
     {
          readOk = tryRead(conn, buffer, twoBytes, 2, 3); // 尝试读取前两个字节,最多尝试3次
          if (!readOk)
          {
               ok = false;
               LOG_WARN("处理扩展长度1数据不足(2Bytes)");
               return;
          }
          payloadLength = (static_cast<uint8_t>(twoBytes[0]) << 8) |
                          static_cast<uint8_t>(twoBytes[1]);
     }
     else if (initialPayloadLen == 127)
     {
          char eightByte[8];
          readOk = tryRead(conn, buffer, eightByte, 8, 3); // 尝试读取前8个字节,最多尝试3次
          if (!readOk)
          {
               ok = false;
               LOG_WARN("处理扩展长度2数据不足(8Bytes)");
               return;
          }
          payloadLength = 0;
          for (int i = 0; i < 8; ++i)
          {
               payloadLength = (payloadLength << 8) | static_cast<uint8_t>(eightByte[i]);
          }
     }
     else
     {
          payloadLength = initialPayloadLen;
     }

     // 处理 Masking Key
     if (masked)
     {
          readOk = tryRead(conn, buffer, maskingKey.data(), 4, 3); // 尝试读取前4个字节,最多尝试3次
          if (!readOk)
          {
               ok = false;
               LOG_WARN("Masking Key数据不足(4Bytes)");
               return;
          }
     }

     // 提取 Payload 数据
     payloadData.resize(payloadLength);
     readOk = tryRead(conn, buffer, payloadData.data(), payloadLength, 3); // 尝试读取Payload数据,最多尝试3次
     if (!readOk)
     {
          ok = false;
          LOG_WARN("payloadData数据不足(" + std::to_string(payloadLength) + "Bytes)");
          return;
     }
     // 解码 Payload 数据（如果有 Masking Key）
     if (masked)
     {
          for (size_t i = 0; i < payloadData.size(); ++i)
          {
               payloadData[i] ^= maskingKey[i % 4];
          }
     }
}

bool WebSocketFrame::tryRead(TcpConnection *conn, Buffer &buf, char *data, int len, int tryNum)
{
     for (int i = 0; i < tryNum; ++i) // 最多尝试n次
     {
          conn->readFd();
          len -= buf.read(data, len);
          if (len == 0)
               return true;
          std::this_thread::sleep_for(std::chrono::milliseconds(10 * (i + 1))); // 指数退避，避免频繁轮询对系统资源造成影响
     }
     throw std::runtime_error("webSocket帧数据读取错误"); // 主要之后无法再区分websocket帧读取了多少
     return false;
}