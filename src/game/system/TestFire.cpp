#include "game/system/TestFire.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"

void TestFireSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     static int num = 0;
     auto view = em.getView<Input>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b01) // 左键
          {
               // createEntityBlock(em, worldId, {16, 0.05f, true}, input->x, input->y);
               createEntityBullet(em, worldId, entity);
               num++;
               if (num % 50 == 0)
               {
                    std::cout << "create bullet:" << num << std::endl;
               }
          }
     }
}