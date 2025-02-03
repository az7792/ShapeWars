#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "box2d/box2d.h"

void playerMovementSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     auto group = em.group<Position, Velocity, b2BodyId, Input>();
     for (auto entity : group)
     {
          b2BodyId *bodyId = em.getComponent<b2BodyId>(entity);
          Input *input = em.getComponent<Input>(entity);
          int dx = 0, dy = 0;
          if (input->state & (1ull << 2)) // W
               dy += 1;
          if (input->state & (1ull << 3)) // A
               dx -= 1;
          if (input->state & (1ull << 4)) // S
               dy -= 1;
          if (input->state & (1ull << 5)) // D
               dx += 1;
          b2Vec2 vel = b2Vec2_zero;
          if (dx == 0 && dy == 0)
               vel.x = vel.y = 0;
          else if (dx != 0 && dy != 0)
          {
               vel.x = 0.1f * dx / sqrtf32(2), vel.y = 0.1f * dy / sqrtf32(2);
          }
          else if (dx != 0)
               vel.x = 0.1f * dx, vel.y = 0;
          else if (dy != 0)
               vel.y = 0.1f * dy, vel.x = 0;
          b2Body_SetLinearVelocity(*bodyId, vel);
     }
}