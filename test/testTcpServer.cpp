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
using namespace std;
// 测试内存泄漏：
// 先编译好 server 可执行文件
// 然后执行 valgrind --leak-check=full ./server
void onMessage(TcpConnection *conn, Buffer &buf)
{
     string msg = buf.readAllAsString();
     LOG_DEBUG(msg);
     conn->send(msg);
}

void onClose(TcpConnection *conn)
{
     LOG_DEBUG(conn->peerAddr.getIpPort() + " disconnected");
}

void onError(TcpConnection *conn)
{
     LOG_DEBUG("error");
}

void onConnection(TcpConnection *conn)
{
     LOG_DEBUG(conn->peerAddr.getIpPort() + " connected");
}
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     TcpServer server(InetAddress("127.0.0.1", 7792), 2);
     server.setNewConnectionCallback(onConnection);
     server.setOnMessageCallback(onMessage);
     server.setOnCloseCallback(onClose);
     server.setOnErrorCallback(onError);
     thread t([&]() { // 测试关闭
          this_thread::sleep_for(chrono::seconds(60));
           server.close();
     });
     server.run();
     if (t.joinable())
          t.join();
     return 0;
}