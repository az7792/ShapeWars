// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/fireSys.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"

void fireSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto group = em.group<Input, b2BodyId>();
     for (auto entity : *group)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b01) // 左键
          {
               auto &barrelList = em.getComponent<Children>(entity)->children;
               for (auto barrelEntity : barrelList)
               {
                    auto *barrel = em.getComponent<Barrel>(barrelEntity);
                    auto *fireStatus = em.getComponent<FireStatus>(barrelEntity);
                    //由于加点系统，可能在子弹发射中途出现发射间隔小于冷却时间的情况，所以这里需要判断一下
                    if (tick - barrel->LastTick >= barrel->cooldown && fireStatus->status == static_cast<uint8_t>(0b00000000))
                         fireStatus->status = static_cast<uint8_t>(0b00000001);
               }
          }
     }

     auto group2 = em.group<Barrel>();
     for (auto entity : *group2)
     {
          auto *barrel = em.getComponent<Barrel>(entity);
          Parent *parent = em.getComponent<Parent>(entity);
          FireStatus *fireStatus = em.getComponent<FireStatus>(entity);
          float angle = em.getComponent<Angle>(parent->id)->angle + barrel->offsetAngle;

          if (fireStatus->status & 0b00000001) // 可以发设
          {
               BulletParams params;
               params.parentEntity = parent->id;
               params.angle = angle;
               createEntityBullet(em, worldId, tick, params);
               barrel->LastTick = tick;
               assert(fabs(barrel->nowLength - barrel->length) <= 1e-5);
               if (barrel->cooldown <= 2)
               {
                    fireStatus->status = static_cast<uint8_t>(0b00000100);
                    barrel->nowLength = (1.f - 0.1f) * barrel->length;
               }
               else
               {
                    barrel->nowLength -= 0.1f * barrel->length / (barrel->cooldown / 2);
                    fireStatus->status = static_cast<uint8_t>(0b00000010);
               }
          }
          else if (fireStatus->status & 0b00000010) // 发射中，第一段
          {
               if (tick - barrel->LastTick + 1 >= barrel->cooldown / 2)
               {
                    fireStatus->status = static_cast<uint8_t>(0b00000100);
                    barrel->nowLength = (1.f - 0.1f) * barrel->length;
               }
               else
                    barrel->nowLength -= 0.1f * barrel->length / (barrel->cooldown / 2);
          }
          else if (fireStatus->status & 0b00000100) // 发射中，第二段
          {
               if (tick - barrel->LastTick + 1 >= barrel->cooldown)
               {
                    fireStatus->status = static_cast<uint8_t>(0b00000000);
                    barrel->nowLength = barrel->length;
               }
               else
                    barrel->nowLength += 0.1f * barrel->length / (barrel->cooldown / 2);
          }
     }
}