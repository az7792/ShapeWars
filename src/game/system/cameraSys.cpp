// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/cameraSys.h"
#include "game/utils.h"
#include "box2d/box2d.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include <map>
#include <unordered_set>

namespace
{
     // 0
     void appendPosition0(std::string *data, b2Vec2 position, uint64_t &componentState)
     {
          componentState |= COMP_POSITION;
          strAppend(*data, position.x);
          strAppend(*data, position.y);
     }

     // 2
     void appendAngle2(std::string *data, Angle *angle, uint64_t &componentState)
     {
          componentState |= COMP_ANGLE;
          strAppend(*data, angle->angle);
     }

     // 3
     void appendRegularPolygon3(std::string *data, uint64_t &componentState, RegularPolygon *regularPolygon, bool isCreate = false)
     {
          if (isCreate)
          {
               componentState |= COMP_POLYGON;
               strAppend(*data, regularPolygon->sides);
               strAppend(*data, regularPolygon->radius);
          }
     }

     // 4
     void appendHP4(uint32_t tick, std::string *data, uint64_t &componentState, HP *hp, bool isCreate = false)
     {
          if (hp->tick == tick || isCreate) // 创建时无论是否变换都需要打包
          {
               componentState |= COMP_HP;
               strAppend(*data, hp->hp);
               strAppend(*data, hp->maxHP);
          }
     }

     // 6
     void appendGroupIndex6(std::string *data, uint64_t &componentState, GroupIndex *groupIndex, bool isCreate = false)
     {
          if (isCreate)
          {
               componentState |= COMP_GROUPINDEX;
               strAppend(*data, groupIndex->index);
          }
     }

     // 7
     void appendName7(std::string *data, uint64_t &componentState, Name *name, bool isCreate = false)
     {
          if (isCreate)
          {
               componentState |= COMP_NAME;
               strAppend<uint8_t>(*data, static_cast<uint8_t>(name->name.size()));
               *data += name->name;
          }
     }

     // 8
     void appendStyle8(std::string *data, uint64_t &componentState, Style *style, bool isCreate = false)
     {
          if (isCreate)
          {
               componentState |= COMP_STYLE;
               strAppend(*data, style->fillColor);
               strAppend(*data, style->strokeColor);
          }
     }

     // 9
     void appendBarrelList9(ecs::EntityManager &em, std::string *data, uint64_t &componentState, std::vector<ecs::Entity> *barrelList)
     {
          componentState |= COMP_BARRELLIST;
          strAppend<uint8_t>(*data, static_cast<uint8_t>(barrelList->size()));
          for (size_t i = 0; i < barrelList->size(); ++i)
          {
               Barrel *barrel = em.getComponent<Barrel>(barrelList->at(i));
               strAppend(*data, barrel->nowLength);
          }
     }

     // 10
     void appendScore10(uint32_t tick, std::string *data, uint64_t &componentState, Score *score, bool isCreate = false)
     {
          if (score->tick == tick || isCreate) // 创建时无论是否变换都需要打包
          {
               componentState |= COMP_SCORE;
               strAppend(*data, score->score);
          }
     }

     //11
     void appendTankID11(uint32_t tick, std::string *data, uint64_t &componentState, TankID *id, bool isCreate = false)
     {
          if (id->tick == tick || isCreate) // 创建时无论是否变换都需要打包
          {
               componentState |= COMP_TANKID;
               strAppend(*data, id->id);
          }
     }

     // 创建时无论是否需要更新都需要打包
     void processEntity(ecs::EntityManager &em, uint32_t tick, ecs::Entity targetEntity, bool isCreate = false)
     {
          PackData *packData = em.getComponent<PackData>(targetEntity);
          if (packData->isCreated && isCreate)
               return;
          if (packData->isUpdated && !isCreate)
               return;

          std::string *data = nullptr;
          bool *isPacked = nullptr;
          if (isCreate)
          {
               data = &packData->createData;
               isPacked = &packData->isCreated;
          }
          else
          {
               data = &packData->updateData;
               isPacked = &packData->isUpdated;
          }
          data->clear();

          strAppend(*data, ecs::entityToIndex(targetEntity)); // 写入实体ID

          // 写入实体类型
          Type *type = em.getComponent<Type>(targetEntity);
          strAppend<uint8_t>(*data, __builtin_ctz(type->id));

          uint64_t componentState = 0;
          b2BodyId *bodyId = em.getComponent<b2BodyId>(targetEntity);
          strAppend(*data, componentState); // 占位

          if (type->id == CATEGORY_PLAYER) // 处理玩家实体
          {
               appendPosition0(data, b2Body_GetPosition(*bodyId), componentState);
               appendAngle2(data, em.getComponent<Angle>(targetEntity), componentState);
               appendRegularPolygon3(data, componentState, em.getComponent<RegularPolygon>(targetEntity), isCreate);
               appendHP4(tick, data, componentState, em.getComponent<HP>(targetEntity), isCreate);
               appendGroupIndex6(data, componentState, em.getComponent<GroupIndex>(targetEntity), isCreate);
               appendName7(data, componentState, em.getComponent<Name>(targetEntity), isCreate);
               appendBarrelList9(em, data, componentState, &(em.getComponent<Children>(targetEntity)->children));
               appendScore10(tick, data, componentState, em.getComponent<Score>(targetEntity), isCreate);
               appendTankID11(tick, data, componentState, em.getComponent<TankID>(targetEntity), isCreate);
          }
          else if (type->id == CATEGORY_BLOCK) // 处理方块实体
          {
               appendPosition0(data, b2Body_GetPosition(*bodyId), componentState);
               appendAngle2(data, em.getComponent<Angle>(targetEntity), componentState);
               appendRegularPolygon3(data, componentState, em.getComponent<RegularPolygon>(targetEntity), isCreate);
               appendHP4(tick, data, componentState, em.getComponent<HP>(targetEntity), isCreate);
               appendStyle8(data, componentState, em.getComponent<Style>(targetEntity), isCreate);
          }
          else if (type->id == CATEGORY_BULLET) // 处理子弹实体
          {
               appendPosition0(data, b2Body_GetPosition(*bodyId), componentState);
               appendAngle2(data, em.getComponent<Angle>(targetEntity), componentState);
               appendRegularPolygon3(data, componentState, em.getComponent<RegularPolygon>(targetEntity), isCreate);
               appendGroupIndex6(data, componentState, em.getComponent<GroupIndex>(targetEntity), isCreate);
          }

          std::memcpy(data->data() + 5, &componentState, 8); // 写入组件状态
          *isPacked = true;
     };
}

void cameraSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     // 处理传感器事件
     b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);
     for (int i = 0; i < sensorEvents.beginCount; ++i)
     {
          b2SensorBeginTouchEvent *beginTouch = sensorEvents.beginEvents + i;
          Camera *camera = static_cast<Camera *>(b2Shape_GetUserData(beginTouch->sensorShapeId));
          ecs::Entity *entity = static_cast<ecs::Entity *>(b2Shape_GetUserData(beginTouch->visitorShapeId));
          // 处理开始事件
          camera->createEntities.push_back(*entity);
     }

     for (int i = 0; i < sensorEvents.endCount; ++i)
     {
          b2SensorEndTouchEvent *endTouch = sensorEvents.endEvents + i;
          if (b2Shape_IsValid(endTouch->visitorShapeId) && b2Shape_IsValid(endTouch->sensorShapeId)) // 由于移动
          {
               Camera *camera = static_cast<Camera *>(b2Shape_GetUserData(endTouch->sensorShapeId));
               ecs::Entity *entity = static_cast<ecs::Entity *>(b2Shape_GetUserData(endTouch->visitorShapeId));
               // 处理结束事件
               camera->removeEntities.push_back(*entity);
               camera->inEntities.erase(*entity);
          }
          else if (b2Shape_IsValid(endTouch->sensorShapeId)) // 由于刚体被删除
          {
               Camera *camera = static_cast<Camera *>(b2Shape_GetUserData(endTouch->sensorShapeId));
               uint64_t shapeId = b2StoreShapeId(endTouch->visitorShapeId);
               ecs::Entity entity = shapeEntityMap[shapeId];
               // 处理结束事件
               camera->delEntities.push_back(entity);
               camera->inEntities.erase(entity);
          }
     }

     // 处理摄像机的实体数据打包
     auto view = em.getView<Camera>();
     for (auto entity : view)
     {
          Camera *camera = em.getComponent<Camera>(entity);
          if (em.hasComponent<b2BodyId>(entity))
          {
               // 调整摄像机速度，平滑跟随玩家
               b2BodyId bodyId = camera->bodyId;
               b2Vec2 curr = b2Body_GetPosition(bodyId);
               b2Vec2 target = b2Body_GetPosition(*em.getComponent<b2BodyId>(entity));
               b2Vec2 currVelocity = b2Body_GetLinearVelocity(bodyId);
               // BUG: 006
               b2Body_SetLinearVelocity(bodyId, SmoothDampVelocity(curr, target, currVelocity, 0.1f, 50.f));
          }
          else
          {
               b2Body_SetLinearVelocity(camera->bodyId, b2Vec2_zero);
          }

          // 处理 createEntities
          for (auto createEntity : camera->createEntities)
          {
               processEntity(em, tick, createEntity, true);
          }

          // 处理 inEntities
          for (auto inEntity : camera->inEntities)
          {
               processEntity(em, tick, inEntity, false);
          }

          // 删除单独处理
          // 因为对于每个摄像机而言删除的实体的打包不一致，对于当前摄像机是删除，但对于其他摄像机则是创建或者更新
     }
}