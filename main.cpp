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
using namespace std;
void F(int fd)
{
     char buf[1024] = {0};
     int n = read(fd, buf, 1024);
     if (n <= 0)
          ;
     printf("%s\n", buf);
}
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     // 1. 创建监听套接字
     int listenFd = socket(AF_INET, SOCK_STREAM, 0);

     // 2. 设置地址和端口
     struct sockaddr_in addr;
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_port = htons(7792);       // 设置端口号
     addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡

     // 3. 绑定地址到套接字
     bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));

     // 4. 开始监听套接字
     listen(listenFd, 5);

     cout << "Server listening on port 7792..." << endl;
     // 阻塞接受客户端连接
     int clientFd = accept(listenFd, nullptr, nullptr);

     cout << "New client connected!" << endl;

     EventLoop eloop;

     Channel *ch = new Channel(&eloop, clientFd);
     ch->setReadCallback(std::bind(F, ch->fd));
     ch->enableReading();

     thread t([&]()
              {
          this_thread::sleep_for(std::chrono::seconds(5));
          eloop.quit(); });
     eloop.loop();

     close(listenFd);
     if (t.joinable())
          t.join();
     return 0;
}