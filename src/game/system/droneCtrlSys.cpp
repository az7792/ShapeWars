// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/droneCtrlSys.h"
#include "game/component/fwd.h"

void droneCtrlSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto group = em.group<DroneFlag>();
     for (auto entity : *group)
     {
          Parent *parent = em.getComponent<Parent>(entity);

          if ((!em.entityIsValid(parent->id)) || (!em.hasComponent<b2BodyId>(parent->id))) // 如果父实体无效，则删除该实体
          {
               em.addComponent<DeleteFlag>(entity);
               continue;
          }

          Input *input = em.getComponent<Input>(parent->id);
          b2Vec2 pos = b2Body_GetPosition(*em.getComponent<b2BodyId>(entity));

          float angle = atan2(input->y - pos.y, input->x - pos.x);
          em.getComponent<Angle>(entity)->angle = angle;
          b2Vec2 vel = (b2Vec2){cosf(angle), sinf(angle)};
          vel = b2Normalize(vel) * 10.f;

          if (input->state & 0b10) // 按下右键
          {
               em.getComponent<Angle>(entity)->angle += M_PI ;
               b2Body_SetLinearVelocity(*em.getComponent<b2BodyId>(entity), -vel);
          }
          else if (input->state & 0b01) // 按下左键
          {
               b2Body_SetLinearVelocity(*em.getComponent<b2BodyId>(entity), vel);
          }
     }
}