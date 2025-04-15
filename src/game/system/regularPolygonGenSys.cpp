// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"
#include "game/system/regularPolygonGenSys.h"

namespace
{
     BlockParams getSidesAndStyle()
     {
          //DONE:根据区域密度来生成方块
          /*
               均匀分布即可，玩家在一个地方清的差不多了会自发前往其他地区
          */
          BlockParams params;

          params.position.x = (std::rand() % 960) / 10 - 48.f;
          params.position.y = (std::rand() % 960) / 10 - 48.f;

          auto randNum = std::rand() % 100;

          // 40：40：15：5
          if (randNum < 40)
               params.polygon = {3, 3.f / 10}, params.score = 10, params.initialHP = params.maxHP = 50;
          else if (randNum < 80)
               params.polygon = {4, 4.f / 10}, params.score = 20, params.initialHP = params.maxHP = 100;
          else if (randNum < 95)
               params.polygon = {5, 6.f / 10}, params.score = 40, params.initialHP = params.maxHP = 150;
          else
               params.polygon = {6, 7.f / 10}, params.score = 50, params.initialHP = params.maxHP = 400;

          randNum = std::rand() % 100;
          if (randNum < 50)
               params.style = {BOLCK_FILL_GREEN, BOLCK_STROKE_GREEN}, params.score *= 1;
          else if (randNum < 80)
               params.style = {BOLCK_FILL_BLUE, BOLCK_STROKE_BLUE}, params.score *= 2;
          else if (randNum < 90)
               params.style = {BOLCK_FILL_PURPLE, BOLCK_STROKE_PURPLE}, params.score *= 3;
          else
               params.style = {BOLCK_FILL_ORANGE, BOLCK_STROKE_ORANGE}, params.score *= 4;

          if (params.polygon.sides >=4 && rand() % 100 == 0)
               params.polygon.radius = 1.5f, params.score *= 30, params.initialHP = params.maxHP *= 50;
          return params;
     }
}

void regularPolygonGenSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto view = em.getView<Input>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b10)
          {
               // auto [sides, style] = getSidesAndStyle();
               // createEntityBlock(em, worldId, tick, {sides, sides / 10.f}, style, input->x, input->y);
          }
     }

     if (shapeEntityMap.size() <= 500)
     {
          createEntityBlock(em, worldId, tick, getSidesAndStyle());
     }
}