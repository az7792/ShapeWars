// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/restoreHPSys.h"
#include "game/component/fwd.h"

void restoreHPSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     static std::vector<ecs::Entity> tmpEntities;

     // 为所有一定时间内未受伤的实体恢复生命值
     tmpEntities.clear();
     auto group = em.group<HP>(ecs::type_list<HealingOverTime>{}); // 有HP但是没有HealingOverTime的实体
     for (auto &entity : *group)
     {
          HP *hp = em.getComponent<HP>(entity);
          if (tick - hp->tick >= TPS * 5 && hp->hp < hp->maxHP) // 5s内未再次受伤并且hp低于最大值
          {
               tmpEntities.push_back(entity);
          }
     }
     for (auto entity : tmpEntities)
     {
          em.addComponent<HealingOverTime>(entity, static_cast<int16_t>(1 * TPS), tick, TPS * 5u);
     }

     // 处理拥有HealingOverTime的实体
     // 加血/扣血
     // 移除过期的HealingOverTime组件
     tmpEntities.clear();
     auto group2 = em.group<HP, HealingOverTime>();
     for (auto entity : *group2)
     {
          HP *hp = em.getComponent<HP>(entity);
          auto healingOverTime = em.getComponent<HealingOverTime>(entity);

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
          // 移除过期的HealingOverTime组件
          if (tick - healingOverTime->startTick > healingOverTime->durationTick)
          {
               tmpEntities.push_back(entity);
          }
     }
     for (auto entity : tmpEntities)
     {
          em.removeComponent<HealingOverTime>(entity);
     }
}