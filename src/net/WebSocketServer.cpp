#include "net/WebSocketServer.h"
#include "config/config.h"

std::vector<uint8_t> WebSocketServer::sha1(const std::string &message)
{
     // 将一个32位的数循环左移n位
     std::function<uint32_t(uint32_t value, size_t bits)> leftRotate = [](uint32_t value, size_t bits) -> uint32_t
     {
          return (value << bits) | (value >> (32 - bits));
     };
     // 预处理
     std::vector<uint8_t> paddedMessage(message.begin(), message.end());
     paddedMessage.push_back(0x80); // 添加 0x80
     while ((paddedMessage.size() + 8) % 64 != 0)
     {
          paddedMessage.push_back(0x00);
     }

     // 添加消息长度（位）
     uint64_t messageLengthBits = static_cast<uint64_t>(message.size()) * 8;
     for (int i = 7; i >= 0; --i)
     {
          paddedMessage.push_back((messageLengthBits >> (i * 8)) & 0xFF);
     }

     uint32_t h0 = 0x67452301;
     uint32_t h1 = 0xEFCDAB89;
     uint32_t h2 = 0x98BADCFE;
     uint32_t h3 = 0x10325476;
     uint32_t h4 = 0xC3D2E1F0;
     // 处理每个块
     for (size_t i = 0; i < paddedMessage.size(); i += 64)
     {
          // 处理w[i]
          uint32_t w[80] = {0};
          for (int j = 0; j < 16; ++j)
          {
               w[j] = (paddedMessage[i + j * 4] << 24) |
                      (paddedMessage[i + j * 4 + 1] << 16) |
                      (paddedMessage[i + j * 4 + 2] << 8) |
                      (paddedMessage[i + j * 4 + 3]);
          }
          for (int j = 16; j < 80; ++j)
          {
               w[j] = leftRotate(w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16], 1);
          }

          // 初始化哈希值
          uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
          // 处理80次
          for (int j = 0; j < 80; ++j)
          {
               uint32_t f, k;
               if (j < 20)
               {
                    f = d ^ (b & (c ^ d));
                    k = 0x5A827999;
               }
               else if (j < 40)
               {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
               }
               else if (j < 60)
               {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
               }
               else
               {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
               }
               uint32_t temp = leftRotate(a, 5) + f + e + k + w[j];
               e = d;
               d = c;
               c = leftRotate(b, 30);
               b = a;
               a = temp;
          }
          // 累加
          h0 += a;
          h1 += b;
          h2 += c;
          h3 += d;
          h4 += e;
     }
     // 输出最终哈希值
     std::vector<uint8_t> digest(20);
     uint32_t h[] = {h0, h1, h2, h3, h4};
     for (int i = 0; i < 5; ++i)
     {
          digest[i * 4] = (h[i] >> 24) & 0xFF;
          digest[i * 4 + 1] = (h[i] >> 16) & 0xFF;
          digest[i * 4 + 2] = (h[i] >> 8) & 0xFF;
          digest[i * 4 + 3] = h[i] & 0xFF;
     }
     return digest;
}

std::string WebSocketServer::base64Encode(const std::vector<uint8_t> &input)
{
     std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

     std::string encoded;
     int num = input.size() % 3; // 空余的字节数
     if (num == 1)               // 最后一组有8位(占用12位)，因此需要补两个'='(12位) ⌈8 / 6⌉ * 6 = 12
          num = 2;
     else if (num == 2) // 最后一组有16位(占用18位)，因此需要补一个'='(6位) ⌈16 / 6⌉ * 6 = 18
          num = 1;
     size_t i = 0;
     while (i < input.size())
     {
          // 每3个字节按6位分成4组，放在triple的低24位，不够的补充0
          uint32_t triple = 0;
          for (int j = 0; j < 3; ++j)
          {
               triple <<= 8;
               if (i < input.size())
               {
                    triple |= input[i++];
               }
          }
          for (int j = 1; j <= 4; ++j)
          {
               encoded.push_back(base64Chars[(triple >> (24 - j * 6)) & 0x3F]);
          }
     }
     for (int i = 0; i < num; ++i)
          encoded[encoded.size() - i - 1] = '=';
     return encoded;
}

void WebSocketServer::handleRead(TcpConnection *conn, Buffer &buffer)
{

     if (tcpConnected.find(conn) == tcpConnected.end()) // 可能是握手包
     {
          // 已经建立TCP连接，需要进行websocket握手
          std::string origin = buffer.readAllAsString();
          // 解析握手包
          size_t keyPos = origin.find("Sec-WebSocket-Key: ");
          if (keyPos == std::string::npos)
          {
               // 握手失败，关闭连接
               LOG_WARN(conn->peerAddr.getIpPort() + "握手失败,不是握手包");
               tcpServer.closeConnection(conn);
               return;
          }

          // 提取 Sec-WebSocket-Key
          size_t keyStart = keyPos + 19;
          size_t keyEnd = origin.find("\r\n", keyStart);
          std::string secWebSocketKey = origin.substr(keyStart, keyEnd - keyStart);

          // 生成 Sec-WebSocket-Accept
          std::string acceptKey = base64Encode(sha1(secWebSocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"));

          // 构造握手响应
          std::string response =
              "HTTP/1.1 101 Switching Protocols\r\n"
              "Upgrade: websocket\r\n"
              "Connection: Upgrade\r\n"
              "Sec-WebSocket-Accept: " +
              acceptKey + "\r\n\r\n";

          // 发送握手响应
          if (conn->send(response) < 0)
          {
               LOG_WARN(conn->peerAddr.getIpPort() + "握手失败");
               tcpServer.closeConnection(conn);
               return;
          }

          if (addConnect(conn))
          {
               // 标记连接建立完成
               LOG_INFO(conn->peerAddr.getIpPort() + "WebSocket已连接");
               if (onOpen)
                    onOpen(conn);
          }
     }
     else
     {
          bool isContinue = true;
          while (isContinue && buffer.readableBytes() > 0)
          {
               // static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
               // auto nowTime = std::chrono::steady_clock::now();
               // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime).count();
               // lastTime = nowTime;
               // std::ostringstream oss;
               // oss << std::this_thread::get_id();
               // LOG_DEBUG("readContinue:" + std::to_string(duration) + "ms On thread:" + oss.str());

               WebSocketFrame frame(conn, buffer, isContinue);
               // TODO: 重做半包处理
               // 已经连接webSocket连接
               if (!frame.ok)
               {
                    LOG_ERROR("webSocket帧解析错误" + conn->peerAddr.getIpPort() + "已断开连接");
                    tcpServer.closeConnection(conn);
                    subConnect(conn);
                    return;
               }
               if (frame.opcode == 0x8) // 关闭帧
               {
                    LOG_INFO("[ websocket ]" + conn->peerAddr.getIpPort() + "已关闭连接");
                    tcpServer.closeConnection(conn);
                    subConnect(conn);
                    return;
               }
               if (frame.opcode == 0x9) // Ping帧
               {
                    std::string Pong = WebSocketFrame::encode(1, 0xA, 0, ""); // 返回Pong帧
                    conn->send(Pong);
                    return;
               }
               if (onMessage)
               {
                    onMessage(conn, std::move(frame.payloadData));
               }
          }
     }
}

void WebSocketServer::handleClose(TcpConnection *conn)
{
     subConnect(conn);
     if (onClose)
          onClose(conn);
}

void WebSocketServer::handleError(TcpConnection *conn)
{
     subConnect(conn);
     if (onError)
          onError(conn);
}

bool WebSocketServer::addConnect(TcpConnection *conn)
{
     if (conn == nullptr)
          return 0;
     std::lock_guard<std::mutex> lock(tcpConnectedMutex);
     if (tcpConnected.size() >= MAX_CONNECTED)
     {
          // 不接受连接，主动断开
          tcpServer.closeConnection(conn);
          return 0;
     }
     else if (tcpConnected.find(conn) == tcpConnected.end())
     {
          tcpConnected.emplace(conn);
          return 1;
     }
     return 0;
}

void WebSocketServer::subConnect(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock(tcpConnectedMutex);
     if (conn != nullptr && tcpConnected.find(conn) != tcpConnected.end())
     {
          tcpConnected.erase(conn);
     }
}

int WebSocketServer::send(const std::string &message, TcpConnection *conn)
{
     int sendNum = conn->send(WebSocketFrame::encode(1, 0x2, 0, message));
     if (sendNum <= 0)
     {
          LOG_WARN("[ websocket ]发送失败," + conn->peerAddr.getIpPort() + "已断开连接");
          handleClose(conn);
     }
     return sendNum;
}

WebSocketServer::WebSocketServer(const InetAddress &addr) : tcpServer(addr, 2), isRunning(false)
{
     // server.setNewConnectionCallback();
     tcpServer.setOnMessageCallback(std::bind(&WebSocketServer::handleRead, this, std::placeholders::_1, std::placeholders::_2));
     tcpServer.setOnCloseCallback(std::bind(&WebSocketServer::handleClose, this, std::placeholders::_1));
     tcpServer.setOnErrorCallback(std::bind(&WebSocketServer::handleError, this, std::placeholders::_1));
}

WebSocketServer::~WebSocketServer()
{
     close();
}

void WebSocketServer::setOnOpen(std::function<void(TcpConnection *)> cb)
{
     onOpen = cb;
}

void WebSocketServer::setOnClose(std::function<void(TcpConnection *)> cb)
{
     onClose = cb;
}

void WebSocketServer::setOnMessage(std::function<void(TcpConnection *, std::string &&)> cb)
{
     onMessage = cb;
}

void WebSocketServer::setOnError(std::function<void(TcpConnection *)> cb)
{
     onError = cb;
}

void WebSocketServer::run()
{
     if (isRunning)
          return;
     isRunning = true;
     tcpServer.run();
}

void WebSocketServer::close()
{
     if (!isRunning)
          return;
     isRunning = false;
     tcpServer.close();
}