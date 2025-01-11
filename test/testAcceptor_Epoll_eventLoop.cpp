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
using namespace std;
void F(Channel *ch)
{
     char buf[1024] = {0};
     int n = SocketOps::recv(ch->fd, buf, 1024);
     if (n == 0)
     {
          cout << "tmp" << endl;
          ch->remove(); // 此时该channel已经是游离状态，会发生内存泄漏
          return;
     }
     SocketOps::send(ch->fd, buf, strlen(buf));
     printf("%s\n", buf);
}
// 测试内存泄漏：
// 先编译好 server 可执行文件
// 然后执行 valgrind --leak-check=full ./server
vector<Channel *> channels;
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);

     EventLoop eloop1;                    // 主事件循环
     EventLoop *eloop2 = new EventLoop(); // 从事件循环
     Acceptor *acceptor = new Acceptor(&eloop1, InetAddress("127.0.0.1", 7792));
     acceptor->setNewConnectionCallback([eloop = eloop2](int sockfd, const InetAddress &addr) { // 连接到来时
          cout << "new connection from " << addr.getIpPort() << endl;
          Channel *ch = new Channel(eloop, sockfd);
          channels.push_back(ch);
          ch->setReadCallback(std::bind(F, ch));
          ch->enableReading();
     });

     thread t([&]() { // 测试关闭
          this_thread::sleep_for(std::chrono::seconds(60));
          delete acceptor; // 先关闭acceptor
          eloop1.quit();
          eloop2->quit();
     });

     thread t2(&EventLoop::loop, eloop2);
     eloop1.loop();

     if (t.joinable())
          t.join();
     if (t2.joinable())
          t2.join();
     delete eloop2;
     for (auto ch : channels)
          delete ch;
     return 0;
}