#include "game/system/attackSys.h"
#include "game/component/fwd.h"

void attackSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

     // 形状开始接触
     for (int i = 0; i < contactEvents.beginCount; ++i)
     {
          b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
          // ShapesStartTouching(beginEvent->shapeIdA, beginEvent->shapeIdB);
          ecs::Entity entityA = *static_cast<ecs::Entity *>(b2Shape_GetUserData(beginEvent->shapeIdA));
          ecs::Entity entityB = *static_cast<ecs::Entity *>(b2Shape_GetUserData(beginEvent->shapeIdB));
          if (em.hasComponent<AttackList>(entityA))
          {
               em.getComponent<AttackList>(entityA)->attacks.push_back(beginEvent->shapeIdB);
          }
          if (em.hasComponent<AttackList>(entityB))
          {
               em.getComponent<AttackList>(entityB)->attacks.push_back(beginEvent->shapeIdA);
          }
     }

     // 形状停止接触
     for (int i = 0; i < contactEvents.endCount; ++i)
     {
          b2ContactEndTouchEvent *endEvent = contactEvents.endEvents + i;

          // 使用 b2Shape_IsValid，因为形状可能已被销毁
          if (b2Shape_IsValid(endEvent->shapeIdA))
          {
               ecs::Entity entityA = *static_cast<ecs::Entity *>(b2Shape_GetUserData(endEvent->shapeIdA));
               if (em.hasComponent<AttackList>(entityA))
               {
                    auto &attacks = em.getComponent<AttackList>(entityA)->attacks;
                    for (auto it = attacks.begin(); it != attacks.end(); ++it) // O(n) 这个n一般很小 0-5
                    {
                         if (b2StoreShapeId(*it) == b2StoreShapeId(endEvent->shapeIdB)) // 转为uint64_t进行比较
                         {
                              std::swap(*it, attacks.back());
                              attacks.pop_back();
                              break;
                         }
                    }
               }
          }
          if (b2Shape_IsValid(endEvent->shapeIdB))
          {
               ecs::Entity entityB = *static_cast<ecs::Entity *>(b2Shape_GetUserData(endEvent->shapeIdB));
               if (em.hasComponent<AttackList>(entityB))
               {
                    auto &attacks = em.getComponent<AttackList>(entityB)->attacks;
                    for (auto it = attacks.begin(); it != attacks.end(); ++it) // O(n) 这个n一般很小 0-5
                    {
                         if (b2StoreShapeId(*it) == b2StoreShapeId(endEvent->shapeIdA)) // 转为uint64_t进行比较
                         {
                              std::swap(*it, attacks.back());
                              attacks.pop_back();
                              break;
                         }
                    }
               }
          }
     }

     // 进行攻击
     auto group = em.group<Attack, AttackList>();
     for (auto entity : group)
     {
          auto attackList = em.getComponent<AttackList>(entity);
          auto attack = em.getComponent<Attack>(entity);

          for (auto attackShapeId : attackList->attacks)
          {
               auto attackId = *static_cast<ecs::Entity *>(b2Shape_GetUserData(attackShapeId));
               if (em.hasComponent<HP>(attackId))
               {
                    auto hp = em.getComponent<HP>(attackId);
                    hp->hp -= attack->damage / TPS;
                    hp->isDirty = true;
               }
          }
     }
}