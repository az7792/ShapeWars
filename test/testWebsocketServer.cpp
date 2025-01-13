#include <iostream>
#include <thread>
#include "box2d/box2d.h"
#include "utils/Time.h"
#include "utils/ThreadPool.h"
#include "utils/Logger.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "net/Channel.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "net/SocketOps.h"
#include "net/Acceptor.h"
#include "utils/ObjectPool.h"
#include "net/TcpServer.h"
#include "net/WebSocketServer.h"
#include "net/WebSocketFrame.h"
using namespace std;
// 测试内存泄漏：
// 先编译好 server 可执行文件
// 然后执行 valgrind --leak-check=full ./server
void onMessage(TcpConnection *conn, string &&msg)
{
     LOG_DEBUG(msg);
     conn->send(WebSocketFrame::encode(1, 0x1, 0, msg));
}

void onClose(const TcpConnection *conn)
{
     LOG_DEBUG(conn->peerAddr.getIpPort() + " disconnected");
}

void onError(const TcpConnection *conn)
{
     LOG_DEBUG("error");
}

void onOpen(const TcpConnection *conn)
{
     LOG_DEBUG(conn->peerAddr.getIpPort() + " connected");
}
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     WebSocketServer server(InetAddress("0.0.0.0", 7792));
     server.setOnMessage(onMessage);
     server.setOnClose(onClose);
     server.setOnError(onError);
     server.setOnOpen(onOpen);
     thread t([&]() { // 测试关闭
          this_thread::sleep_for(chrono::seconds(120));
          server.close();
     });
     server.run();
     if (t.joinable())
          t.join();
     return 0;
}