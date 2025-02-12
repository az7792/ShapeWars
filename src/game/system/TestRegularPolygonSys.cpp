#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"
#include "game/system/TestRegularPolygon.h"

void TestRegularPolygonSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     static int num = 0;
     auto view = em.getView<Input>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b10)
          {
               createEntityBlock(em, worldId, {5, 0.05f, true}, input->x, input->y);
               num++;
               if(num % 50 == 0)
               {
                    std::cout << "create block " << num << std::endl;
               }
          }
     }
}