// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/bulletAccelCtrlSys.h"
#include "game/component/fwd.h"

void bulletAccelCtrlSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto group = em.group<BulletAccelerationFlag>();
     for (auto entity : *group)
     {
          Parent *parent = em.getComponent<Parent>(entity);
          if (!em.entityIsValid(parent->id))
               continue;
          Attribute *attribute = em.getComponent<Attribute>(parent->id);
          b2BodyId *bodyId = em.getComponent<b2BodyId>(entity);
          Angle *angle = em.getComponent<Angle>(entity);
          float mass = b2Body_GetMass(*bodyId);
          float forceVal = (attribute->attr[6] * 0.3) * mass;

          b2Vec2 force = (b2Vec2){cosf(angle->angle), sinf(angle->angle)};
          force = b2Normalize(force) * forceVal;
          b2Body_ApplyForce(*bodyId, force, b2Vec2_zero, true);
     }
}