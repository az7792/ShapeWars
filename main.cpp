#include "game/GameLoop.h"
#include "utils/Logger.h"
#include <thread>
// valgrind --leak-check=full --show-leak-kinds=all ./server
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     GameLoop gameLoop;
     // TODO: 指令系统
     std::thread t(&GameLoop::run, &gameLoop);

     std::string command;
     while (true)
     {
          std::cin >> command;
          if (command == "exit")
          {
               gameLoop.exit();
               break;
          }
          std::cout << "command: " << command << std::endl;
     }
     if (t.joinable())
          t.join();
     if (gameLoop.isRunning())
          gameLoop.exit();
     LOG_INFO("GameLoop stopped");
     return 0;
}