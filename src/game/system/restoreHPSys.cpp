// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/restoreHPSys.h"
#include "game/component/fwd.h"

void restoreHPSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     // 处理拥有HealingOverTime的实体
     // 加血/扣血
     auto group = em.group<HP, HealingOverTime>();
     for (auto entity : *group)
     {
          HP *hp = em.getComponent<HP>(entity);
          if (hp->hp == hp->maxHP)
               continue;
          auto healingOverTime = em.getComponent<HealingOverTime>(entity);

          // 对于一定时间内未受伤的实体恢复生命值
          if (healingOverTime->healing == 0 && tick >= healingOverTime->startTick + healingOverTime->cooldownTick)
          {
               healingOverTime->healing = healingOverTime->healingTarget;
               healingOverTime->startTick = tick;
          }

          if (healingOverTime->healing < 0 && hp->hp >= 0) // 扣血
          {
               hp->hp = std::max(0, hp->hp + healingOverTime->healing / TPS);
               hp->tick = tick;
          }
          else if (healingOverTime->healing > 0 && hp->hp < hp->maxHP) // 加血
          {
               hp->hp = std::min(hp->hp + healingOverTime->healing / TPS, static_cast<int>(hp->maxHP));
               hp->tick = tick;
          }

          if (hp->hp <= 0 && !em.hasComponent<DeleteFlag>(entity))
          {
               em.addComponent<DeleteFlag>(entity);
          }
          // 重置HealingOverTime组件
          if (tick >= healingOverTime->startTick + healingOverTime->durationTick)
          {
               healingOverTime->healing = 0;
               healingOverTime->startTick = tick;
          }
     }
}