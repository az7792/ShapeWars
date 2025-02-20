#include "game/GameLoop.h"
#include "config/config.h"
#include "game/system/fwd.h" //所有系统
// #include "game/system/movementSys.h"
#include "game/factories.h"
#include "game/utils.h"
#include "lz4/lz4.h"
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
     static std::string message = ""; // 原始数据
     static std::string dstStr = "";  // 压缩后的数据

     auto group = em_.group<Camera, TcpConnection *>();
     for (auto &entity : *group)
     {
          Camera *camera = em_.getComponent<Camera>(entity);
          message.clear();
          strAppend<uint8_t>(message, 0x01); // 更新实体数据
          // 操作的玩家所属碰撞组
          strAppend<int32_t>(message, em_.getComponent<GroupIndex>(entity)->index);

          // 摄像机坐标
          b2Vec2 cameraPos = b2Body_GetPosition(camera->bodyId);
          strAppend<float>(message, cameraPos.x);
          strAppend<float>(message, cameraPos.y);

          // 移出实体列表
          uint16_t len = camera->removeEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->removeEntities)
          {
               strAppend(message, ecs::entityToIndex(e));
          }

          // 删除实体列表
          len = camera->delEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->delEntities)
          {
               strAppend(message, ecs::entityToIndex(e));
          }

          // 创建实体列表
          len = camera->createEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->createEntities)
          {
               message += em_.getComponent<PackData>(e)->createData;
               em_.getComponent<PackData>(e)->isCreated = false;
          }

          // 已经在视野内实体列表
          len = camera->inEntities.size();
          strAppend<uint16_t>(message, len);
          for (auto e : camera->inEntities)
          {
               message += em_.getComponent<PackData>(e)->updateData;
               em_.getComponent<PackData>(e)->isUpdated = false;
          }

          // 清理摄像机
          camera->removeEntities.clear();
          camera->delEntities.clear();
          for (auto &v : camera->createEntities)
          {
               camera->inEntities.insert(v);
          }
          camera->createEntities.clear();

          // 发送数据
          if (message.size() >= 2500)
          {
               // LZ4压缩
               int srcSize = message.size();
               int maxDstSize = LZ4_compressBound(srcSize);             // 最坏情况下的压缩大小
               if (static_cast<size_t>(maxDstSize + 9) > dstStr.size()) // 9 = uint8 + int32 + int32 (head + srcSize + dstSize)
               {
                    dstStr.resize(maxDstSize + 9);
               }
               int dstSize = LZ4_compress_default(message.data(), dstStr.data() + 9, srcSize, maxDstSize);
               if (dstSize <= 0) // 压缩失败
               {
                    ws_.send(message, *em_.getComponent<TcpConnection *>(entity));
                    continue;
               }
               dstStr.resize(9 + dstSize);
               dstStr[0] = 0x04; // 0x04 LZ4压缩
               std::memcpy(dstStr.data() + 1, &srcSize, sizeof(int32_t));
               std::memcpy(dstStr.data() + 5, &dstSize, sizeof(int32_t));
               ws_.send(dstStr, *em_.getComponent<TcpConnection *>(entity));
          }
          else
          {
               ws_.send(message, *em_.getComponent<TcpConnection *>(entity));
          }
     }
}

void GameLoop::createPlayerSys()
{
     static int32_t groupIndex = -1;
     std::lock_guard<std::mutex> lock1(createPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
     while (!createPlayerQueue_.empty())
     {
          auto it = playerMap_.find(createPlayerQueue_.front());
          if (it == playerMap_.end()) // 玩家未创建实体
          {
               ecs::Entity entity = createEntityPlayer(em_, worldId_, tick_, createPlayerQueue_.front(), {groupIndex--});
               int inputIndex = freeInputsQueue_.front();
               freeInputsQueue_.pop_front();
               playerMap_.emplace(createPlayerQueue_.front(), std::make_pair(entity, true));
               inputMap_[entity] = inputIndex;
               LOG_INFO("创建一个角色：" + std::to_string(entity));
               ws_.send(std::string(1, 0x06), createPlayerQueue_.front()); // 通知客户端玩家已创建
               createPlayerQueue_.pop_front();
          }
          else if (it->second.second == 0) // 玩家已死亡
          {
               ecs::Entity entity = it->second.first;
               createPlayBody(entity);
               it->second.second = 1;                                      // 标记玩家存活
               ws_.send(std::string(1, 0x06), createPlayerQueue_.front()); // 通知客户端玩家已创建
               createPlayerQueue_.pop_front();
          }
          else
          {
               assert(false); // 路径不可达
          }
     }
}

void GameLoop::destroyEntitySys()
{
     { // 处理因为网络断开导致的玩家删除
          std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
          std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
          while (!destroyPlayerQueue_.empty())
          {
               ecs::Entity e = playerMap_[destroyPlayerQueue_.front()].first;
               playerMap_.erase(destroyPlayerQueue_.front());
               destroyPlayerQueue_.pop_front();
               if (em_.hasComponent<b2BodyId>(e)) // 可能在玩家死亡后掉线
               {
                    b2BodyId *bodyId = em_.getComponent<b2BodyId>(e);
                    b2DestroyBody(*bodyId); // 清除玩家的刚体
               }
               b2DestroyBody(em_.getComponent<Camera>(e)->bodyId); // 清除摄像机刚体
               em_.destroyEntity(e);
               freeInputsQueue_.push_back(inputMap_[e]);
               inputMap_.erase(e);
          }
     }
     // 处理其他实体
     auto view = em_.getView<DeleteFlag>();
     for (auto &entity : view)
     {
          auto type = em_.getComponent<Type>(entity);
          if (type->id == CATEGORY_PLAYER) // 处理玩家实体
          {
               b2BodyId *bodyId = em_.getComponent<b2BodyId>(entity);
               deleteBody(*bodyId);
               em_.removeComponent<b2BodyId>(entity);
               em_.getComponent<ContactList>(entity)->list.clear();
               em_.removeComponent<DeleteFlag>(entity); // 需要在创建前移除，因为是先删再创建，因此在下一帧会被重复删第二次
               std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
               playerMap_[*em_.getComponent<TcpConnection *>(entity)].second = 0; // 标记玩家为死亡

               ws_.send(std::string(1, 0x05), *em_.getComponent<TcpConnection *>(entity)); // 通知客户端玩家已死亡
          }
          else if (type->id == CATEGORY_BLOCK || type->id == CATEGORY_BULLET) // 处理方块实体 与 子弹实体
          {
               b2BodyId *bodyId = em_.getComponent<b2BodyId>(entity);
               deleteBody(*bodyId);
               em_.destroyEntity(entity);
          }
     }
}

void GameLoop::delayDeleteShapesSys()
{
     for (auto shapeId : willDeleteShapes)
     {
          shapeEntityMap.erase(b2StoreShapeId(shapeId));
     }
     willDeleteShapes.clear();
}

void GameLoop::deleteBody(b2BodyId bodyId)
{
     static std::vector<b2ShapeId> shapeIdCache;
     int shapeNum = b2Body_GetShapeCount(bodyId);             // 获取刚体上的形状数量
     if (static_cast<size_t>(shapeNum) > shapeIdCache.size()) // resize
     {
          shapeIdCache.resize(shapeNum);
     }
     b2Body_GetShapes(bodyId, shapeIdCache.data(), shapeNum); // 获取刚体上的形状
     for (int i = 0; i < shapeNum; ++i)                       // 添加到待删除列表
     {
          willDeleteShapes.emplace_back(shapeIdCache[i]);
     }
     b2DestroyBody(bodyId);
}

void GameLoop::createPlayBody(ecs::Entity entity)
{
     // em_.replaceComponent<Position>(entity);
     // em_.replaceComponent<Velocity>(entity);
     // em_.replaceComponent<HP>(entity, static_cast<int16_t>(100), static_cast<int16_t>(100));
     em_.getComponent<HP>(entity)->hp = em_.getComponent<HP>(entity)->maxHP;
     em_.getComponent<HP>(entity)->tick = tick_;
     // em_.replaceComponent<Attack>(entity, static_cast<int16_t>(2 * TPS));
     // em_.replaceComponent<ContactList>(entity);
     // em_.replaceComponent<PackData>(entity, "", "");
     // em_.replaceComponent<Input>(entity, 0.f, 0.f, 0ull);
     // em_.replaceComponent<TcpConnection *>(entity, tcpConnection);
     // em_.replaceComponent<Type>(entity, static_cast<uint8_t>(CATEGORY_PLAYER));
     // em_.getComponent<GroupIndex>(entity)->isDirty = true;
     em_.replaceComponent<RegularPolygon>(entity, static_cast<uint8_t>(64), 0.05f); //>=16为圆形
     // em_.replaceComponent<Camera>(entity, 0.f, 0.f, 1.f);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.position = {0.f, 0.f};
     bodyDef.userData = static_cast<void *>(em_.getEntityPtr(entity));
     b2BodyId bodyId = b2CreateBody(worldId_, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_PLAYER;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_BORDER_WALL;
     shapeDef.filter.groupIndex = em_.getComponent<GroupIndex>(entity)->index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = em_.getComponent<RegularPolygon>(entity)->radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = entity;

     em_.addComponent<b2BodyId>(entity, bodyId);
}

void GameLoop::handleOnMessage(TcpConnection *conn, Buffer &buffer)
{
     char header = buffer.readValue<char>();
     switch (header)
     {
     case 0x00: // 创建角色
     {
          std::lock_guard<std::mutex> lock1(createPlayerQueueMutex_);
          std::unique_lock<std::mutex> lock2(playerAndInputMapMutex_);
          if (playerMap_.find(conn) == playerMap_.end() || playerMap_[conn].second == 0) // 玩家未创建实体或已死亡
               createPlayerQueue_.push_back(conn);
          break;
     }
     case 0x01: // 操作指令
     {
          std::unique_lock<std::mutex> lock(playerAndInputMapMutex_);
          if (playerMap_.find(conn) != playerMap_.end())
          {
               int inputIndex = inputMap_[playerMap_[conn].first];
               lock.unlock();
               Input input = {};
               input.x = buffer.readValue<float>();
               input.y = buffer.readValue<float>();
               input.state = buffer.readValue<uint64_t>();
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
     std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
     if (playerMap_.find(conn) != playerMap_.end()) // 玩家不一定创建了实体
          destroyPlayerQueue_.push_back(conn);
}

void GameLoop::handleOnError(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerAndInputMapMutex_);
     if (playerMap_.find(conn) != playerMap_.end()) // 玩家不一定创建了实体
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

     // 创建墙
     createEntityBrderWall(em_, worldId_, 5.f, 5.f);

     // 注册系统
     em_.addSystem(std::bind(&GameLoop::inputSys, this))
         .addSystem(std::bind(&GameLoop::destroyEntitySys, this))
         .addSystem(std::bind(&GameLoop::createPlayerSys, this)) // 先删再创建能回收一部分实体标识符
         .addSystem(std::bind(&playerMovementSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&TestRegularPolygonSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&TestFireSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&physicsSys, std::ref(worldId_)))
         .addSystem(std::bind(&attackSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&cameraSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&GameLoop::delayDeleteShapesSys, this))
         .addSystem(std::bind(&GameLoop::outputSys, this));
}

GameLoop::~GameLoop()
{
     b2DestroyWorld(worldId_);
}

void GameLoop::run()
{
     int ct = 0;
     std::thread WebSocketServerThread(&WebSocketServer::run, std::ref(ws_)); // websocket服务器线程
     isRunning_.store(true);
     while (isRunning_.load())
     {
          // 休眠到目标时间点，需要提前计算
          auto nextFrameTime = lastTime_ + std::chrono::milliseconds(1000 / TPS); // HACK: 这儿间隔是整数，无法整除时会导致 设定的TPS 与 实际逻辑帧率不符

          em_.updateSystems();
          tick_++;

          std::this_thread::sleep_until(nextFrameTime);
          lastTime_ = nextFrameTime;

          // // next frame
          // char ch;
          // std::cin >> ch;
          // std::cout << ch << std::endl;
          ct++;
          if (ct % (10 * TPS) == 0)
          {
               std::cout << shapeEntityMap.size() << std::endl;
          }
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
