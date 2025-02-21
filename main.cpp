#include "game/GameLoop.h"
#include "ecs/ComponentTypeID.h"
#include "game/component/fwd.h"
#include "utils/Logger.h"
#include <thread>
// valgrind --leak-check=full --show-leak-kinds=all ./server
int main()
{
     std::srand(std::time(0));
     /// 务必于协议表顺序一致，最好不要轻易调整顺序，哪怕这个字段永远用不到
     ecs::ComponentTypeID::registerComponent<Position>();       // 0
     ecs::ComponentTypeID::registerComponent<Velocity>();       // 1
     ecs::ComponentTypeID::registerComponent<Angle>();          // 2
     ecs::ComponentTypeID::registerComponent<RegularPolygon>(); // 3
     ecs::ComponentTypeID::registerComponent<HP>();             // 4
     ecs::ComponentTypeID::registerComponent<Type>();           // 5
     ecs::ComponentTypeID::registerComponent<GroupIndex>();     // 6
     ecs::ComponentTypeID::registerComponent<Name>();           // 7
     ecs::ComponentTypeID::registerComponent<Style>();          // 8

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