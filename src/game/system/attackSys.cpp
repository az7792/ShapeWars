#include "game/system/attackSys.h"
#include "game/component/fwd.h"

void attackSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

     // 形状开始接触
     for (int i = 0; i < contactEvents.beginCount; ++i)
     {
          b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
          // ShapesStartTouching(beginEvent->shapeIdA, beginEvent->shapeIdB);
          ecs::Entity entityA = *static_cast<ecs::Entity *>(b2Shape_GetUserData(beginEvent->shapeIdA));
          ecs::Entity entityB = *static_cast<ecs::Entity *>(b2Shape_GetUserData(beginEvent->shapeIdB));
          if (em.hasComponent<BorderWall>(entityA) || em.hasComponent<BorderWall>(entityB))
          {
               continue;
          }
          if (em.hasComponent<ContactList>(entityA))
          {
               em.getComponent<ContactList>(entityA)->list.push_back(beginEvent->shapeIdB);
          }
          if (em.hasComponent<ContactList>(entityB))
          {
               em.getComponent<ContactList>(entityB)->list.push_back(beginEvent->shapeIdA);
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
               if (em.hasComponent<ContactList>(entityA))
               {
                    auto &attacks = em.getComponent<ContactList>(entityA)->list;
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
               if (em.hasComponent<ContactList>(entityB))
               {
                    auto &attacks = em.getComponent<ContactList>(entityB)->list;
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
     auto group = em.group<Attack, ContactList>();
     for (auto entity : *group)
     {
          auto contactList = em.getComponent<ContactList>(entity);
          auto attack = em.getComponent<Attack>(entity);

          bool isBullet = (em.getComponent<Type>(entity)->id == CATEGORY_BULLET);
          BulletAttackNum *bulletAttackNum = (isBullet ? em.getComponent<BulletAttackNum>(entity) : nullptr);

          for (auto shapeId : contactList->list)
          {
               auto attackId = *static_cast<ecs::Entity *>(b2Shape_GetUserData(shapeId));
               if (em.hasComponent<HP>(attackId))
               {
                    auto hp = em.getComponent<HP>(attackId);
                    if (hp->hp > 0)
                    {
                         hp->hp = std::max(0, hp->hp - attack->damage / TPS);
                         hp->tick = tick;
                         if (isBullet) // 处理子弹攻击攻击判定次数
                         {
                              bulletAttackNum->num--;
                              if (bulletAttackNum->num <= 0)
                              {
                                   if (!em.hasComponent<DeleteFlag>(entity))
                                        em.addComponent<DeleteFlag>(entity);
                                   if (hp->hp <= 0 && (!em.hasComponent<DeleteFlag>(attackId)))
                                   {
                                        em.addComponent<DeleteFlag>(attackId);
                                   }
                                   break;
                              }
                         }
                    }
                    if (hp->hp <= 0 && (!em.hasComponent<DeleteFlag>(attackId)))
                    {
                         em.addComponent<DeleteFlag>(attackId);
                    }
               }
          }
     }

     auto group2 = em.group<HP, DamageOverTime>();
     for (auto entity : *group2)
     {
          auto hp = em.getComponent<HP>(entity);
          auto damageOverTime = em.getComponent<DamageOverTime>(entity);
          if (hp->hp > 0)
          {
               hp->hp = std::max(0, hp->hp - damageOverTime->damage / TPS);
               hp->tick = tick;
          }
          else if (!em.hasComponent<DeleteFlag>(entity))
          {
               em.addComponent<DeleteFlag>(entity);
          }
     }
}