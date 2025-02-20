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
               createEntityBlock(em, worldId, tick, {5, 0.05f}, input->x, input->y);
          }
     }

     if (shapeEntityMap.size() <= 1500)
     {
          float x = (std::rand() % 800) / 100;
          float y = (std::rand() % 800) / 100;
          createEntityBlock(em, worldId, tick, {5, 0.05f}, x - 4.f, y - 4.f);
     }
}