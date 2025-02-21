#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"
#include "game/system/TestRegularPolygon.h"

void TestRegularPolygonSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto view = em.getView<Input>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b10)
          {
               auto randNum = std::rand() % 10;
               if (randNum < 4)
                    createEntityBlock(em, worldId, tick, {3, 0.3f}, input->x, input->y);
               else if (randNum < 7)
                    createEntityBlock(em, worldId, tick, {4, 0.4f}, input->x, input->y);
               else if (randNum < 9)
                    createEntityBlock(em, worldId, tick, {5, 0.5f}, input->x, input->y);
               else
                    createEntityBlock(em, worldId, tick, {6, 0.6f}, input->x, input->y);
          }
     }

     if (shapeEntityMap.size() <= 1000)
     {
          //TODO:设置不同血量与不同密度
          float x = (std::rand() % 960) / 10;
          float y = (std::rand() % 960) / 10;
          x -= 48.f;
          y -= 48.f;
          auto randNum = std::rand() % 10;
          if (randNum < 4)
               createEntityBlock(em, worldId, tick, {3, 0.3f}, x, y);
          else if (randNum < 7)
               createEntityBlock(em, worldId, tick, {4, 0.4f}, x, y);
          else if (randNum < 9)
               createEntityBlock(em, worldId, tick, {5, 0.5f}, x, y);
          else
               createEntityBlock(em, worldId, tick, {6, 0.6f}, x, y);
     }
}