#pragma once
#include <string>
#include <vector>
#include "utils/Logger.h"
#include "utils/Buffer.h"
#include "TcpConnection.h"

class WebSocketFrame
{
public:
     bool fin;                       // FIN 位
     uint8_t opcode;                 // Opcode
     bool masked;                    // 是否启用 Mask
     uint64_t payloadLength;         // Payload 长度
     std::array<char, 4> maskingKey; // Masking Key（如果有）
     std::string payloadData;        // 实际数据

     bool ok = true; // 帧是否解析成功

     /**
      * 解析帧
      * @param conn 连接
      * @param buffer 缓冲区
      * @param isContinue 是否可以继续解析下一个包
      */
     WebSocketFrame(TcpConnection *conn, Buffer &buffer, bool &isContinue);

     /// 生成 WebSocket 帧字符串
     static std::string encode(bool fin, uint8_t opcode, bool masked, const std::string &payloadData);

private:
     /// 解析 WebSocket 帧字符串
     void decode(TcpConnection *conn, Buffer &buffer, bool &isContinue);

     bool tryRead(TcpConnection *conn, Buffer &buf, char *data, int len, int tryNum, bool &isContinue);

     /// 尝试从TcpConnection已经对于的buffer中读len个字节到指定位置，最多尝试n次
     bool tryRead(TcpConnection *conn, Buffer &buf, bool &isContinue, char *data, int len, int tryNum = 3);
};