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
               createEntityBlock(em, worldId, tick, {5, 0.5f}, input->x, input->y);
          }
     }

     if (shapeEntityMap.size() <= 1500)
     {
          float x = (std::rand() % 960) / 10;
          float y = (std::rand() % 960) / 10;
          createEntityBlock(em, worldId, tick, {5, 0.5f}, x - 48.f, y - 48.f);
     }
}