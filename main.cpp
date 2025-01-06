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
using namespace std;
void F(int fd)
{
     char buf[1024] = {0};
     int n = SocketOps::recv(fd, buf, 1024);
     SocketOps::send(fd, buf, strlen(buf));
     printf("%s\n", buf);
}
// 测试内存泄漏：
// 先编译好 server 可执行文件
// 然后执行 valgrind --leak-check=full ./server
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     // 1. 创建监听套接字
     int listenFd = SocketOps::createSocket(true);

     // 2. 绑定地址到套接字
     SocketOps::bind(listenFd, InetAddress("127.0.0.1", 7792));

     // 3. 开始监听套接字
     SocketOps::listen(listenFd, 8);

     cout << "Server listening on port 7792..." << endl;
     // 阻塞接受客户端连接
     int clientFd = SocketOps::accept(listenFd, nullptr);

     cout << "New client connected!" << endl;

     EventLoop eloop;

     Channel *ch = new Channel(&eloop, clientFd);
     ch->setReadCallback(std::bind(F, ch->fd));
     ch->enableReading();

     thread t([&]()
              {
          this_thread::sleep_for(std::chrono::seconds(5));
          //eloop.quit();
           });
     eloop.loop();

     close(listenFd);
     if (t.joinable())
          t.join();
     return 0;
}