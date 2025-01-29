#pragma once
#include "WebSocketFrame.h"
#include "TcpServer.h"

class WebSocketServer
{
     // SHA-1
     std::vector<uint8_t> sha1(const std::string &message);
     // Base64编码
     std::string base64Encode(const std::vector<uint8_t> &input);

     void handleRead(TcpConnection *conn, Buffer &buffer); // 处理接受到的TCP消息
     void handleClose(TcpConnection *conn);                // 处理连接关闭
     void handleError(TcpConnection *conn);                // 处理错误

     std::function<void(TcpConnection *)> onOpen;                    // 连接打开
     std::function<void(TcpConnection *)> onClose;                   // 关闭时
     std::function<void(TcpConnection *)> onError;                   // 发生错误时
     std::function<void(TcpConnection *, std::string &&)> onMessage; // 收到消息时

     TcpServer tcpServer;
     bool isRunning;

     std::mutex tcpConnectedMutex;                     // 同步tcpConnected的锁
     std::unordered_set<TcpConnection *> tcpConnected; // 管理已经连接的websocket链接
     bool addConnect(TcpConnection *conn);             // 增加一个新连接
     void subConnect(TcpConnection *conn);             // 断开一个连接

public:
     WebSocketServer(const InetAddress &addr);
     ~WebSocketServer();
     /// @brief 发送消息到指定连接
     int send(const std::string &message, TcpConnection *conn);
     void run();
     void close();

public: // SETTERS
     void setOnOpen(std::function<void(TcpConnection *)> cb);
     void setOnClose(std::function<void(TcpConnection *)> cb);
     void setOnError(std::function<void(TcpConnection *)> cb);
     void setOnMessage(std::function<void(TcpConnection *, std::string &&)> cb);
};
