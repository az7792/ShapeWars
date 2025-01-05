#pragma once

class Epoll;
class Channel;

class EventLoop
{
     Epoll *epoll_; // 管理的Epoll
     bool running_; // loop是否正在运行

     /**
      * @brief 用于通知退出的Channel
      * running_为false时，loop可能阻塞在wait上
      * 通过注册一个用于通知的fd，在quit时向该fd写内容，让loop解除阻塞
      * @warning 该Channel通过epoll_释放
      */
     Channel *quitCh_;

public:
     EventLoop();
     ~EventLoop();

     /// @brief 开启事件循环
     void loop();

     /// @brief 退出事件循环
     void quit();

     /// @brief 更新当前channel订阅的事件状态
     void updateChannel(Channel *ch);
     /// @brief 将当前channel从epoll中注销
     void removeChannel(Channel *ch);
};