#include "game/system/TestFire.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"

void TestFireSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     auto view = em.getView<Input,b2BodyId>();
     for (auto entity : view)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b01) // 左键
          {
               createEntityBullet(em, worldId, entity);
          }
     }
}