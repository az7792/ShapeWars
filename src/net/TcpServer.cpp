#include "net/TcpServer.h"

int TcpServer::nextIndex()
{
     ++nowIndex_;
     if (nowIndex_ == numSubLoop_)
          nowIndex_ = 0;
     return nowIndex_;
}
void TcpServer::newConnection(int sockfd, const InetAddress &addr)
{
     int nextLoopIndex = nextIndex(); // 子eventloop
     TcpConnection *newTcpConnection = connectionPool_.get();
     newTcpConnection->start(&subLoops_[nextLoopIndex], sockfd, localAddr_, addr);
     newTcpConnection->setReadCallback(onMessageCallback_);
     newTcpConnection->setCloseCallback(std::bind(&TcpServer::handleClose, this, std::placeholders::_1));
     newTcpConnection->setErrorCallback(onErrorCallback_);
     if (newConnectionCallback_)
          newConnectionCallback_(newTcpConnection);
}

void TcpServer::handleClose(TcpConnection *tc)
{
     tc->setFdStatus(true); // 设置fd已经关闭
     tc->end();
     connectionPool_.release(tc);
     if (onCloseCallback_)
          onCloseCallback_(tc);
}

TcpServer::TcpServer(const InetAddress &addr, int numSubLoop)
    : numSubLoop_(numSubLoop), localAddr_(addr),
      acceptorLoop_(), acceptor_(&acceptorLoop_, localAddr_),
      subLoops_(numSubLoop_),
      connectionPool_([]() -> TcpConnection *
                      { return new TcpConnection(); }, 128, 10),
      isRunning_(false)
{
     // signal(SIGPIPE, SIG_IGN); // 忽略 SIGPIPE 信号
     acceptor_.setNewConnectionCallback(
         std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
     close();
}

void TcpServer::setNewConnectionCallback(std::function<void(TcpConnection *)> cb)
{
     newConnectionCallback_ = cb;
}

void TcpServer::setOnCloseCallback(std::function<void(TcpConnection *)> cb)
{
     onCloseCallback_ = cb;
}

void TcpServer::setOnErrorCallback(std::function<void(TcpConnection *)> cb)
{
     onErrorCallback_ = cb;
}

void TcpServer::setOnMessageCallback(std::function<void(TcpConnection *, Buffer &)> cb)
{
     onMessageCallback_ = cb;
}

void TcpServer::run()
{
     for (auto &v : subLoops_)
     {
          threads_.emplace_back(std::make_unique<std::thread>(&EventLoop::loop, &v));
     }
     LOG_INFO("subLoop启动成功");
     // 最后开启mainLoop,mainLoop运行在当前线程，先开启会阻塞在wait上，导致之后的subLoop无法开启
     isRunning_ = true;
     acceptorLoop_.loop();
}

void TcpServer::close()
{
     if (!isRunning_)
          return;
     acceptorLoop_.quit();
     for (auto &v : subLoops_)
          v.quit();
     for (auto &v : threads_)
          if (v->joinable())
               v->join();
     isRunning_ = false;
}
