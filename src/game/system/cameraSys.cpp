#include "game/system/cameraSys.h"
#include "game/utils.h"
#include "box2d/box2d.h"
#include "game/component/fwd.h"
#include <map>

void cameraSys(ecs::EntityManager &em, b2WorldId &worldId)
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
               camera->delEntities.push_back(*entity);
               camera->inEntities.erase(*entity);
          }
     }

     // 处理摄像机的实体数据打包
     auto view = em.getView<Camera>();
     for (auto entity : view)
     {
          Camera *camera = em.getComponent<Camera>(entity);
          // 调整摄像机速度，平滑跟随玩家
          b2BodyId bodyId = camera->bodyId;
          b2Vec2 curr = b2Body_GetPosition(bodyId);
          b2Vec2 target = b2Body_GetPosition(*em.getComponent<b2BodyId>(entity));
          b2Vec2 currVelocity = b2Body_GetLinearVelocity(bodyId);
          b2Body_SetLinearVelocity(bodyId, SmoothDampVelocity(curr, target, currVelocity, 0.1f, 1.f));

          auto processEntity = [&](ecs::Entity targetEntity)
          {
               PackData *packData = em.getComponent<PackData>(targetEntity);
               if (packData->isPacked)
                    return;

               packData->data.clear();
               strAppend(packData->data, ecs::entityToIndex(targetEntity)); // 写入实体ID

               // TODO : 写入实体类型
               strAppend<uint8_t>(packData->data, 1); // test

               uint64_t componentState = 0;
               b2BodyId *bodyId = em.getComponent<b2BodyId>(targetEntity);
               strAppend(packData->data, componentState); // 占位
               if (em.hasComponent<Position>(targetEntity))
               {
                    componentState |= (1ull << 0);
                    b2Vec2 position = b2Body_GetPosition(*bodyId);
                    strAppend(packData->data, position.x);
                    strAppend(packData->data, position.y);
               }

               std::memcpy(packData->data.data() + 5, &componentState, 8); // 写入组件状态
               packData->isPacked = true;
          };

          // 处理 createEntities
          for (auto createEntity : camera->createEntities)
          {
               processEntity(createEntity);
          }

          // 处理 inEntities
          for (auto inEntity : camera->inEntities)
          {
               processEntity(inEntity);
          }

          // 删除单独处理
          // 因为对于每个摄像机而言删除的实体的打包不一致，对于当前摄像机是删除，但对于其他摄像机则是创建或者更新
     }
}