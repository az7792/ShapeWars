// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/lifeTimeSys.h"
#include "game/component/fwd.h"

void lifeTimeSys(ecs::EntityManager &em, uint32_t &tick)
{
     auto group = em.group<LifeTime>();
     for (auto entity : *group)
     {
          LifeTime *lifeTime = em.getComponent<LifeTime>(entity);
          if (lifeTime->startTick + lifeTime->durationTick < tick)
          {
               if (!em.hasComponent<DeleteFlag>(entity))
                    em.addComponent<DeleteFlag>(entity);
          }
     }
}