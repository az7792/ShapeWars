#include "game/system/attackSys.h"
#include "game/component/fwd.h"

void attackSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     // 进行攻击
     auto group = em.group<Attack, ContactList>();
     for (auto entity : *group)
     {
          auto contactList = em.getComponent<ContactList>(entity);
          auto attack = em.getComponent<Attack>(entity);
          auto type = em.getComponent<Type>(entity)->id;
          bool needBreak = false;

          for (auto shapeId : contactList->list)
          {
               auto attackId = *static_cast<ecs::Entity *>(b2Shape_GetUserData(shapeId));
               if (!em.hasComponent<HP>(attackId))
                    continue;
               auto hp = em.getComponent<HP>(attackId);

               if (hp->hp <= 0) // 避免多次计算得分
                    continue;

               if (hp->hp > 0)
               {
                    hp->hp = std::max(0, hp->hp - attack->damage / TPS);
                    hp->tick = tick;

                    if (em.hasComponent<HealingOverTime>(attackId) && em.getComponent<HealingOverTime>(attackId)->healing > 0) // 打断回复
                    {
                         em.removeComponent<HealingOverTime>(attackId);
                    }

                    // 处理子弹的判断次数
                    if (type == CATEGORY_BULLET)
                    {
                         BulletAttackNum *bulletAttackNum = em.getComponent<BulletAttackNum>(entity);
                         bulletAttackNum->num--;
                         if (bulletAttackNum->num <= 0)
                         {
                              em.addComponent<DeleteFlag>(entity);
                              needBreak = true;
                         }
                    }
               }

               if (hp->hp <= 0)
               {
                    em.addComponent<DeleteFlag>(attackId);

                    // 处理积分
                    ecs::Entity scoreGeter = type == CATEGORY_BULLET ? em.getComponent<Parent>(entity)->id : entity;
                    if (em.entityIsValid(scoreGeter))
                    {
                         Score *score = em.getComponent<Score>(scoreGeter);
                         score->score += em.getComponent<Score>(attackId)->score / 2;
                         score->tick = tick;
                    }

                    assert(type != CATEGORY_BULLET || em.getComponent<Score>(entity)->score == 0);
               }

               if (needBreak)
                    break;
          } // end for contactList->list
     } // end for group
}