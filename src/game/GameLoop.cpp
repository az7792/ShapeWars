#include "game/GameLoop.h"
#include "config/config.h"
#include "game/system/fwd.h" //所有系统
// #include "game/system/movementSys.h"
#include "game/factories.h"
#include "game/utils.h"
#include <cstring>

void GameLoop::inputSys()
{
     auto view = em_.getView<Input>();
     std::unique_lock<std::mutex> lock(playerAndInputMapMutex_);
     for (auto &entity : view)
     {
          // 需要保证ecs与inputMap_是同步的
          atomicInput &aInput = input_[inputMap_[entity]];
          em_.replaceComponent<Input>(entity, aInput.x.load(), aInput.y.load(), aInput.state.load());
     }
}

void GameLoop::outputSys()
{
     auto &group = em_.group<Camera, TcpConnection *>();
     for (auto &entity : group)
     {
          Camera *camera = em_.getComponent<Camera>(entity);
          std::string message;
          strAppend<uint8_t>(message, 0x01); // 更新实体数据
          // 操作的玩家ID
          strAppend<uint32_t>(message, ecs::entityToIndex(entity));

          // 摄像机坐标
          b2Vec2 cameraPos = b2Body_GetPosition(camera->bodyId);
          strAppend<float>(message, cameraPos.x);
          strAppend<float>(message, cameraPos.y);

          // 创建实体列表
          uint16_t len = camera->createEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->createEntities)
          {
               message += em_.getComponent<PackData>(e)->createData;
               em_.getComponent<PackData>(e)->isCreated = false;
          }

          // 删除实体列表
          len = camera->delEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->delEntities)
          {
               strAppend(message, ecs::entityToIndex(e));
          }

          // 进入视野实体列表
          len = camera->inEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->inEntities)
          {
               message += em_.getComponent<PackData>(e)->updateData;
               em_.getComponent<PackData>(e)->isUpdated = false;
          }

          // 清理摄像机
          camera->delEntities.clear();
          for (auto &v : camera->createEntities)
          {
               camera->inEntities.insert(v);
          }
          camera->createEntities.clear();

          ws_.send(message, *em_.getComponent<TcpConnection *>(entity));
     }
}

void GameLoop::createPlayerSys()
{
     std::lock_guard<std::mutex> lock1(createPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
     while (!createPlayerQueue_.empty())
     {
          ecs::Entity entity = createEntityTest(em_, worldId_, createPlayerQueue_.front());
          int inputIndex = freeInputsQueue_.front();
          freeInputsQueue_.pop_front();
          playerMap_[createPlayerQueue_.front()] = entity;
          inputMap_[entity] = inputIndex;
          createPlayerQueue_.pop_front();
          LOG_INFO("创建一个角色：" + std::to_string(entity));
     }
}

void GameLoop::destroyPlayerSys()
{
     std::vector<ecs::Entity> delEntitiesFromCamera;
     {
          std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
          std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
          while (!destroyPlayerQueue_.empty())
          {
               ecs::Entity e = playerMap_[destroyPlayerQueue_.front()];
               playerMap_.erase(destroyPlayerQueue_.front());
               destroyPlayerQueue_.pop_front();
               b2BodyId *bodyId = em_.getComponent<b2BodyId>(e);
               b2DestroyBody(*bodyId);                             // 清除玩家的刚体
               b2DestroyBody(em_.getComponent<Camera>(e)->bodyId); // 清除摄像机刚体
               em_.destroyEntity(e);
               freeInputsQueue_.push_back(inputMap_[e]);
               inputMap_.erase(e);
               delEntitiesFromCamera.push_back(e);
          }
     }

     // 处理 由于销毁刚体导致实体脱离摄像机
     for (auto e : delEntitiesFromCamera)
     {
          auto &group = em_.group<Camera, TcpConnection *>();
          for (auto entity : group)
          {
               Camera *camera = em_.getComponent<Camera>(entity);
               auto it = camera->inEntities.find(e);
               if (it != camera->inEntities.end())
               {
                    camera->inEntities.erase(it);
                    camera->delEntities.push_back(e);
               }
          }
     }
}

void GameLoop::handleOnMessage(TcpConnection *conn, std::string &&msg)
{
     int index = 0;
     char header = msg[index++];
     switch (header)
     {
     case 0x00: // 创建角色
     {
          std::lock_guard<std::mutex> lock1(createPlayerQueueMutex_);
          std::unique_lock<std::mutex> lock2(playerAndInputMapMutex_);
          if (playerMap_.find(conn) == playerMap_.end())
               createPlayerQueue_.push_back(conn);
          break;
     }
     case 0x01: // 操作指令
     {
          std::unique_lock<std::mutex> lock(playerAndInputMapMutex_);
          if (playerMap_.find(conn) != playerMap_.end())
          {
               int inputIndex = inputMap_[playerMap_[conn]];
               lock.unlock();
               Input input = {};
               std::memcpy(&input.x, msg.data() + index, sizeof(float));
               index += sizeof(float);
               std::memcpy(&input.y, msg.data() + index, sizeof(float));
               index += sizeof(float);
               std::memcpy(&input.state, msg.data() + index, sizeof(uint64_t));
               input_[inputIndex].x.store(input.x);
               input_[inputIndex].y.store(input.y);
               input_[inputIndex].state.store(input.state);
          }
          break;
     }
     case 0x02: // Ping
     {
          std::string message = {0x03}; // Pong
          ws_.send(message, conn);
          break;
     }
     default:
          break;
     }
}

void GameLoop::handleOnClose(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock(destroyPlayerQueueMutex_);
     destroyPlayerQueue_.push_back(conn);
}

void GameLoop::handleOnError(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock(destroyPlayerQueueMutex_);
     destroyPlayerQueue_.push_back(conn);
}

void GameLoop::handleOnOpen(TcpConnection *conn)
{
     std::string message;
     message.push_back(0x00);          // 0x00 初始化地图
     strAppend<float>(message, 10.0f); // width
     strAppend<float>(message, 10.0f); // height
     ws_.send(message, conn);
}

GameLoop::GameLoop() : em_(), ws_(InetAddress(LISTEN_IP, LISTEN_PORT)), isRunning_(false), tick_(0), lastTime_(std::chrono::steady_clock::now())
{
     // 初始化空闲输入队列和输入
     for (int i = 0; i < MAX_CONNECTED; ++i)
     {
          freeInputsQueue_.push_back(i);
          input_[i].x.store(0.0f);
          input_[i].y.store(0.0f);
          input_[i].state.store(0ull);
     }

     // 绑定websocket事件
     ws_.setOnMessage(std::bind(&GameLoop::handleOnMessage, this, std::placeholders::_1, std::placeholders::_2));
     ws_.setOnOpen(std::bind(&GameLoop::handleOnOpen, this, std::placeholders::_1));
     ws_.setOnClose(std::bind(&GameLoop::handleOnClose, this, std::placeholders::_1));
     ws_.setOnError(std::bind(&GameLoop::handleOnError, this, std::placeholders::_1));

     // 初始化box2d物理世界
     b2WorldDef worldDef = b2DefaultWorldDef();
     worldDef.gravity = {0.0f, 0.0f};
     worldId_ = b2CreateWorld(&worldDef);

     // 注册系统
     em_.addSystem(std::bind(&GameLoop::inputSys, this))
         .addSystem(std::bind(&GameLoop::destroyPlayerSys, this))
         .addSystem(std::bind(&GameLoop::createPlayerSys, this)) // 先删再创建能回收一部分实体标识符
         .addSystem(std::bind(&playerMovementSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&physicsSys, std::ref(worldId_)))
         .addSystem(std::bind(&attackSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&cameraSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&GameLoop::outputSys, this));
}

GameLoop::~GameLoop()
{
     b2DestroyWorld(worldId_);
}

void GameLoop::run()
{
     std::thread WebSocketServerThread(&WebSocketServer::run, std::ref(ws_)); // websocket服务器线程
     isRunning_.store(true);
     while (isRunning_.load())
     {
          // 休眠到目标时间点，需要提前计算
          // auto nextFrameTime = lastTime_ + std::chrono::milliseconds(2000);       // WARN: 这儿间隔是整数，可能有些问题
          auto nextFrameTime = lastTime_ + std::chrono::milliseconds(1000 / TPS); // WARN: 这儿间隔是整数，可能有些问题

          em_.updateSystems();
          tick_++;

          std::this_thread::sleep_until(nextFrameTime);
          lastTime_ = nextFrameTime;

          // // next frame
          // char ch;
          // std::cin >> ch;
          // std::cout << ch << std::endl;
     }
     ws_.close();
     if (WebSocketServerThread.joinable())
          WebSocketServerThread.join();
     LOG_DEBUG("GameLoop run() end");
}

void GameLoop::exit()
{
     isRunning_.store(false);
}

bool GameLoop::isRunning() const
{
     return isRunning_.load();
}
