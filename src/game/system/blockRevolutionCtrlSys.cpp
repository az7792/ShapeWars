// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/blockRevolutionCtrlSys.h"
#include "game/component/fwd.h"
#include "box2d/box2d.h"

void blockRevolutionCtrlSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     auto group = em.group<BlockRevolutionCtrl, b2BodyId>();
     for (auto &entity : *group)
     {
          auto *ctrl = em.getComponent<BlockRevolutionCtrl>(entity);
          auto *bodyId = em.getComponent<b2BodyId>(entity);
          b2Vec2 vel = b2Body_GetLinearVelocity(*bodyId);

          // 预期速度方向
          float tmp = 2 * M_PI / (ctrl->revolutionTime * TPS);
          ctrl->velocityAngle += ctrl->isClockwise ? tmp : -tmp;
          // 修正到[0,2PI)
          if (ctrl->velocityAngle >= 2 * M_PI)
               ctrl->velocityAngle -= 2 * M_PI;
          if (ctrl->velocityAngle < 0)
               ctrl->velocityAngle += 2 * M_PI;
          float tmpcosf = cosf(ctrl->velocityAngle);
          float tmpsinf = sinf(ctrl->velocityAngle);
          // 预期速度
          b2Vec2 velTarget = b2Vec2{ctrl->targetSpeed * tmpcosf, ctrl->targetSpeed * tmpsinf};

          // 最终速度
          vel.x = (1.f - ctrl->rate) * vel.x + ctrl->rate * velTarget.x;
          vel.y = (1.f - ctrl->rate) * vel.y + ctrl->rate * velTarget.y;

          b2Body_SetLinearVelocity(*bodyId, vel);
     }
}