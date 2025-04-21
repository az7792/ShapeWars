// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/GameLoop.h"
#include "config/config.h"
#include "game/system/fwd.h" //所有系统
// #include "game/system/movementSys.h"
#include "game/factories.h"
#include "game/utils.h"
#include "lz4/lz4.h"
#include "game/tankFactory/tankFactory.h"
#include <cstring>
#include "game/tankFactory/tankFactory.h"

void GameLoop::modifyAttribute(ecs::Entity entity, uint8_t v)
{
     if (!em_.hasComponents<Score, Attribute>(entity))
          return;
     int8_t isUp = ((v & 0x80) >> 7) == 1 ? 1 : -1; // 0b10000000
     uint8_t attr = v & 0x7F;                       // 0b01111111

     Attribute *attribute = em_.getComponent<Attribute>(entity);
     Score *score = em_.getComponent<Score>(entity);

     // 无法更改
     if (isUp == 1 && (attribute->attr[attr] == 8 || score->score < 1000))
          return;
     if (isUp == -1 && (attribute->attr[attr] == 0 || score->score < 2000))
          return;

     // 可以更改
     std::string recvStr = {0x08, static_cast<char>(v)}; // 返回确认信息

     if (attr == 0 && em_.hasComponent<HealingOverTime>(entity)) // 回复速度
     {
          HealingOverTime *healingOverTime = em_.getComponent<HealingOverTime>(entity);
          if ((isUp == 1 && attribute->attr[attr] <= 4) ||
              (isUp == -1 && attribute->attr[attr] <= 5))
          {
               healingOverTime->cooldownTick -= isUp * TPS;
          }
          else
          {
               healingOverTime->healingTarget += isUp * 10;
          }
     }
     else if (attr == 1 && em_.hasComponent<HP>(entity)) // 最大生命值
     {
          HP *hp = em_.getComponent<HP>(entity);
          hp->maxHP += isUp * 100;
          if (isUp == 1)
          {
               hp->hp += 100;
          }
          else //-1
          {
               hp->hp = hp->hp > hp->maxHP ? hp->maxHP : hp->hp;
          }
          hp->tick = tick_;
     }
     else if (attr == 2 && em_.hasComponent<Attack>(entity)) // 身体碰撞伤害
     {
          Attack *attack = em_.getComponent<Attack>(entity);
          attack->damage += isUp;
     }
     else if (attr == 3 || attr == 4 || attr == 5 || attr == 6 || attr == 7) // 子弹伤害 | 子弹飞行时长 | 子弹血量 | 子弹速度 | 子弹密度
     {
          Children *children = em_.getComponent<Children>(entity);
          for (ecs::Entity &e : children->children)
          {
               if (!em_.hasComponent<BulletParams>(e))
               {
                    continue;
               }
               BulletParams *bulletParams = em_.getComponent<BulletParams>(e);
               if (attr == 3) // 子弹伤害
               {
                    bulletParams->attack += isUp * 2;
               }
               else if (attr == 4) // 子弹飞行时长
               {
                    bulletParams->lifetime += isUp * 2;
               }
               else if (attr == 5) // 子弹血量
               {
                    bulletParams->maxHP += isUp * 5;
               }
               else if (attr == 6) // 子弹速度
               {
                    bulletParams->speed += isUp * 0.8f;
               }
               else if (attr == 7) // 子弹密度
               {
                    bulletParams->density += isUp * 0.5f;
               }
          }
     }
     else if (attr == 8 && em_.hasComponent<Children>(entity)) // 子弹射速
     {
          Children *children = em_.getComponent<Children>(entity);
          for (ecs::Entity &e : children->children)
          {
               if (!em_.hasComponent<Barrel>(e))
               {
                    continue;
               }
               if (attribute->attr[attr] == 0 && isUp == 1)
                    em_.getComponent<Barrel>(e)->cooldown -= 3;
               else if (attribute->attr[attr] == 1 && isUp == -1)
                    em_.getComponent<Barrel>(e)->cooldown += 3;
               else if (attribute->attr[attr] == 1 && isUp == 1)
                    em_.getComponent<Barrel>(e)->cooldown -= 2;
               else if (attribute->attr[attr] == 2 && isUp == -1)
                    em_.getComponent<Barrel>(e)->cooldown += 2;
               else
                    em_.getComponent<Barrel>(e)->cooldown -= isUp;
          }
     }
     else if (attr == 9 && em_.hasComponent<Velocity>(entity)) // 角色移动速度
     {
          Velocity *vel = em_.getComponent<Velocity>(entity);
          vel->maxSpeed += isUp * 0.5;
     }

     attribute->attr[attr] += isUp;
     score->score -= isUp == 1 ? 1000 : 2000;
     score->tick = tick_;

     ws_.send(recvStr, *em_.getComponent<TcpConnection *>(entity)); // 返回确认
}

void GameLoop::inputSys()
{
     auto group = em_.group<Input, b2BodyId>();
     std::unique_lock<std::mutex> lock(playerMutex_);
     for (auto &entity : *group)
     {
          // 需要保证ecs与inputMap_是同步的
          atomicInput &aInput = input_[inputMap_[entity]];
          Input *input = em_.replaceComponent<Input>(entity, aInput.x.load(), aInput.y.load(), aInput.state.load());
          b2Vec2 pos = b2Body_GetPosition(*em_.getComponent<b2BodyId>(entity));
          em_.getComponent<Angle>(entity)->angle = atan2(input->y - pos.y, input->x - pos.x);
     }
}

void GameLoop::attributeSys()
{
     auto group = em_.group<Input, b2BodyId>();
     std::unique_lock<std::mutex> lock(playerMutex_);
     for (auto &entity : *group)
     {
          int index = inputMap_[entity];
          std::unique_lock<std::shared_mutex> lock2(inputMutex_[index]); // 写锁
          std::deque<uint8_t> &dq = attributeBuf_[index];
          while (!dq.empty())
          {
               uint8_t attr = dq.front();
               dq.pop_front();
               modifyAttribute(entity, attr);
          }
          assert(dq.empty() && dq.size() == 0);
     }
}

void GameLoop::upgradeSys()
{
     auto group = em_.group<Input, b2BodyId>();
     std::unique_lock<std::mutex> lock(playerMutex_);
     for (auto &entity : *group)
     {
          int index = inputMap_[entity];
          std::unique_lock<std::shared_mutex> lock2(inputMutex_[index]); // 写锁
          std::deque<uint8_t> &dq = upgradeBuf_[index];
          while (!dq.empty())
          {
               uint8_t id = dq.front();
               dq.pop_front();
               TankFactory::instence().upgradeTank(entity, tick_, id);
          }
          assert(dq.empty() && dq.size() == 0);
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
          // 玩家ID
          strAppend<uint32_t>(message, entity);
          // 玩家坦克ID
          strAppend<uint8_t>(message, em_.getComponent<TankID>(entity)->id);
          // 操作的玩家所属碰撞组
          strAppend<int32_t>(message, em_.getComponent<GroupIndex>(entity)->index);

          // 摄像机坐标
          b2Vec2 cameraPos = b2Body_GetPosition(camera->bodyId);
          strAppend<float>(message, cameraPos.x);
          strAppend<float>(message, cameraPos.y);
          // 摄像机是否是瞬移
          strAppend<uint8_t>(message, camera->isTeleport? 1 : 0);
          em_.getComponent<Camera>(entity)->isTeleport = false;

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
     std::lock_guard<std::mutex> lock2(playerMutex_);
     while (!createPlayerQueue_.empty())
     {
          auto it = playerMap_.find(std::get<0>(createPlayerQueue_.front()));
          if (it == playerMap_.end()) // 玩家未创建实体
          {
               TcpConnection *tcpConnection = std::get<0>(createPlayerQueue_.front());
               std::string name = std::get<1>(createPlayerQueue_.front());

               // 创建新玩家实体
               PlayerParams playerParams;
               playerParams.tcpConnection = tcpConnection;
               playerParams.groupIndex = groupIndex--;
               playerParams.name = name;

               ecs::Entity entity = ecs::nullEntity;

               if (name == "0")
                    entity = TankFactory::instence().createTank(tick_, 0, playerParams);
               else if (name == "1")
                    entity = TankFactory::instence().createTank(tick_, 1, playerParams);
               else
                    entity = TankFactory::instence().createTank(tick_, 2, playerParams);

               int inputIndex = freeInputsQueue_.front();
               freeInputsQueue_.pop_front();
               playerMap_.emplace(tcpConnection, std::make_pair(entity, true));
               inputMap_[entity] = inputIndex;
               LOG_INFO("创建一个角色：" + std::to_string(entity));
               ws_.send(std::string(1, 0x06), tcpConnection); // 通知客户端玩家已创建
               createPlayerQueue_.pop_front();
          }
          else if (it->second.second == 0) // 玩家已死亡
          {
               TcpConnection *tcpConnection = std::get<0>(createPlayerQueue_.front());
               std::string name = std::get<1>(createPlayerQueue_.front());
               ecs::Entity entity = it->second.first;
               it->second.first = createNewPlayer(entity, name);
               it->second.second = 1; // 标记玩家存活

               // 重置新实体输入
               inputMap_[it->second.first] = inputMap_[entity];
               inputMap_.erase(entity);

               ws_.send(std::string(1, 0x06), tcpConnection); // 通知客户端玩家已创建
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
          // TODO：加入断线重连
          std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
          std::lock_guard<std::mutex> lock2(playerMutex_);
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

               // 清除玩家的炮管
               std::vector<ecs::Entity> &barrelEntities = em_.getComponent<Children>(e)->children;
               for (auto be : barrelEntities)
               {
                    em_.destroyEntity(be);
               }
               // 清除玩家
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
               em_.removeComponent<HealingOverTime>(entity);
               em_.getComponent<ContactList>(entity)->list.clear();
               em_.removeComponent<DeleteFlag>(entity); // 需要在创建前移除，因为是先删再创建，因此在下一帧会被重复删第二次
               // 清除玩家的炮管
               std::vector<ecs::Entity> &barrelEntities = em_.getComponent<Children>(entity)->children;
               for (auto be : barrelEntities)
               {
                    em_.destroyEntity(be);
               }
               barrelEntities.clear();
               std::lock_guard<std::mutex> lock2(playerMutex_);
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

ecs::Entity GameLoop::createNewPlayer(ecs::Entity entity, std::string name)
{
     static PlayerParams playerParams;

     // 保留原有属性
     playerParams.tcpConnection = *em_.getComponent<TcpConnection *>(entity);
     playerParams.groupIndex = em_.getComponent<GroupIndex>(entity)->index;
     playerParams.name = name;
     playerParams.position = {0.f, 0.f}; // TODO : 随机位置
     Score score = *em_.addComponent<Score>(entity);
     score.score /= 4;
     score.tick = tick_;

     // 清除摄像机刚与旧玩家实体
     b2DestroyBody(em_.getComponent<Camera>(entity)->bodyId);
     em_.destroyEntity(entity);

     // 创建新坦克
     ecs::Entity newEntity = TankFactory::instence().createTank(tick_, 0, playerParams);
     em_.replaceComponent<Score>(newEntity, score);

     return newEntity;
}

void GameLoop::outputStandingsSys()
{
     static uint32_t lastTick = 0;
     static std::vector<ecs::Entity> entities;
     std::string message;
     if (tick_ - lastTick >= TPS) // 1s同步一次
     {
          entities.clear(), message.clear(), lastTick = tick_;

          auto group = em_.group<Input, b2BodyId>();
          for (auto &entity : *group)
          {
               entities.push_back(entity);
          }

          std::sort(entities.begin(), entities.end(), [this](ecs::Entity a, ecs::Entity b)
                    {
               assert(em_.hasComponent<Score>(a) && em_.hasComponent<Score>(b));
               return em_.getComponent<Score>(a)->score > em_.getComponent<Score>(b)->score; });

          size_t len = std::min(entities.size(), static_cast<size_t>(5));
          strAppend<uint8_t>(message, static_cast<uint8_t>(0x07));
          strAppend<uint8_t>(message, static_cast<uint8_t>(len));
          for (size_t i = 0; i < len; i++)
          {
               Name *name = em_.getComponent<Name>(entities[i]);
               strAppend<uint8_t>(message, static_cast<uint8_t>(name->name.size()));
               message += name->name;
               strAppend<int32_t>(message, em_.getComponent<Score>(entities[i])->score);
          }

          auto group2 = em_.group<TcpConnection *>();
          for (auto &e : *group2)
          {
               ws_.send(message, *em_.getComponent<TcpConnection *>(e));
          }
     }
}

void GameLoop::handleOnMessage(TcpConnection *conn, Buffer &buffer)
{
     char header = buffer.readValue<char>();
     switch (header)
     {
     case 0x00: // 创建角色
     {
          std::lock_guard<std::mutex> lock1(createPlayerQueueMutex_);
          std::unique_lock<std::mutex> lock2(playerMutex_);
          uint8_t nameLen = buffer.readValue<uint8_t>();
          std::string name = buffer.readAsString(nameLen);
          if (playerMap_.find(conn) == playerMap_.end() || playerMap_[conn].second == 0) // 玩家未创建实体或已死亡
               createPlayerQueue_.push_back(std::make_tuple(conn, name));
          break;
     }
     case 0x01: // 操作指令
     {
          std::unique_lock<std::mutex> lock(playerMutex_);
          if (playerMap_.find(conn) != playerMap_.end())
          {
               int inputIndex = inputMap_[playerMap_[conn].first];
               lock.unlock();
               Input input = {};
               input.x = buffer.readValue<float>();
               input.y = buffer.readValue<float>();
               if (std::isnan(input.x))
                    input.x = 0.f;
               if (std::isnan(input.y))
                    input.y = 0.f;
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
     case 0x03: // Pong
     {
          break;
     }
     case 0x04: // lz4压缩包
     {
          break;
     }
     case 0x05: // 属性升级包
     {
          std::unique_lock<std::mutex> lock(playerMutex_);
          if (playerMap_.find(conn) != playerMap_.end())
          {
               int index = inputMap_[playerMap_[conn].first];
               lock.unlock();
               std::unique_lock<std::shared_mutex> lock2(inputMutex_[index]); // 写锁
               attributeBuf_[index].push_back(buffer.readValue<uint8_t>());   // 缓存起来由ecs系统处理
          }
          break;
     }
     case 0x06: // 更改角色
     {
          std::unique_lock<std::mutex> lock(playerMutex_);
          if (playerMap_.find(conn) != playerMap_.end())
          {
               int index = inputMap_[playerMap_[conn].first];
               lock.unlock();
               std::unique_lock<std::shared_mutex> lock2(inputMutex_[index]); // 写锁
               upgradeBuf_[index].push_back(buffer.readValue<uint8_t>());     // 缓存起来由ecs系统处理
          }
          break;
     }
     default:
          break;
     }
}

void GameLoop::handleOnClose(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerMutex_);
     if (playerMap_.find(conn) != playerMap_.end()) // 玩家不一定创建了实体
          destroyPlayerQueue_.push_back(conn);
}

void GameLoop::handleOnError(TcpConnection *conn)
{
     std::lock_guard<std::mutex> lock1(destroyPlayerQueueMutex_);
     std::lock_guard<std::mutex> lock2(playerMutex_);
     if (playerMap_.find(conn) != playerMap_.end()) // 玩家不一定创建了实体
          destroyPlayerQueue_.push_back(conn);
}

void GameLoop::handleOnOpen(TcpConnection *conn)
{
     std::string message;
     message.push_back(0x00);           // 0x00 初始化地图
     strAppend<float>(message, 100.0f); // width
     strAppend<float>(message, 100.0f); // height
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

     // 初始化坦克工厂
     TankFactory::instence().init(&em_, &worldId_, "./entityDefs/tankdefs.json");

     // 创建墙
     createEntityBrderWall(em_, worldId_, 50.f, 50.f);

     // 注册系统
     em_.addSystem(std::bind(&GameLoop::inputSys, this))
         .addSystem(std::bind(&GameLoop::attributeSys, this))
         .addSystem(std::bind(&GameLoop::upgradeSys, this))
         .addSystem(std::bind(&lifeTimeSys, std::ref(em_), std::ref(tick_)))
         .addSystem(std::bind(&GameLoop::destroyEntitySys, this))
         .addSystem(std::bind(&GameLoop::createPlayerSys, this)) // 先删再创建能回收一部分实体标识符
         .addSystem(std::bind(&playerMovementSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&regularPolygonGenSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&fireSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&bulletAccelCtrlSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&physicsSys, std::ref(worldId_)))
         .addSystem(std::bind(&blockRotationCtrlSys, std::ref(em_)))
         .addSystem(std::bind(&blockRevolutionCtrlSys, std::ref(em_), std::ref(worldId_)))
         .addSystem(std::bind(&contactListSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&attackSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&restoreHPSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&cameraSys, std::ref(em_), std::ref(worldId_), std::ref(tick_)))
         .addSystem(std::bind(&GameLoop::delayDeleteShapesSys, this))
         .addSystem(std::bind(&GameLoop::outputSys, this))
         .addSystem(std::bind(&GameLoop::outputStandingsSys, this));
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
