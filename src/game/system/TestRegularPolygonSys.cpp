#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"
#include "game/system/TestRegularPolygon.h"

namespace
{
     std::pair<uint8_t, Style> getSidesAndStyle()
     {
          auto randNum = std::rand() % 100;
          uint8_t sides;
          Style style;

          // 40：40：15：5
          if (randNum < 40)
               sides = 3;
          else if (randNum < 80)
               sides = 4;
          else if (randNum < 95)
               sides = 5;
          else
               sides = 6;

          randNum = std::rand() % 100;
          if (randNum < 50)
               style = {BOLCK_FILL_GREEN, BOLCK_STROKE_GREEN};
          else if (randNum < 80)
               style = {BOLCK_FILL_BLUE, BOLCK_STROKE_BLUE};
          else if (randNum < 90)
               style = {BOLCK_FILL_PURPLE, BOLCK_STROKE_PURPLE};
          else
               style = {BOLCK_FILL_ORANGE, BOLCK_STROKE_ORANGE};

          return {sides, style};
     }
}

void TestRegularPolygonSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto view = em.getView<Input>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b10)
          {
               auto [sides, style] = getSidesAndStyle();
               createEntityBlock(em, worldId, tick, {sides, sides / 10.f}, style, input->x, input->y);
          }
     }

     if (shapeEntityMap.size() <= 500)
     {
          // TODO:设置不同血量与不同密度
          float x = (std::rand() % 960) / 10;
          float y = (std::rand() % 960) / 10;
          x -= 48.f;
          y -= 48.f;
          auto [sides, style] = getSidesAndStyle();
          createEntityBlock(em, worldId, tick, {sides, sides / 10.f}, style, x, y);
     }
}