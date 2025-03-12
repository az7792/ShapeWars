#include "game/system/fwd.h"
#include "game/component/fwd.h"
#include "box2d/box2d.h"

namespace
{
     // 将x从[a,b]映射到[c,d]
     float mapValue(float x, float a, float b, float c, float d)
     {
          return c + ((x - a) * (d - c)) / (b - a);
     }

     // float easeOutSine(float x)
     // {
     //      return std::sin((x * M_PI) / 2);
     // }

     // float easeInCubic(float x)
     // {
     //      return x * x * x;
     // }
}

void playerMovementSys(ecs::EntityManager &em, b2WorldId &worldId)
{
     auto group = em.group<Position, Velocity, b2BodyId, Input>();
     for (auto entity : *group)
     {
          b2BodyId *bodyId = em.getComponent<b2BodyId>(entity);
          Input *input = em.getComponent<Input>(entity);
          Velocity *velocity = em.getComponent<Velocity>(entity);
          int dx = 0, dy = 0;
          if (input->state & (1ull << 2)) // W
               dy += 1;
          if (input->state & (1ull << 3)) // A
               dx -= 1;
          if (input->state & (1ull << 4)) // S
               dy -= 1;
          if (input->state & (1ull << 5)) // D
               dx += 1;

          if (dx == 0 && dy == 0)
          {
               b2Vec2 vel = b2Body_GetLinearVelocity(*bodyId);
               vel *= 0.92f;
               b2Body_SetLinearVelocity(*bodyId, vel);
          }
          else
          {
               float lenVel = b2Length(b2Body_GetLinearVelocity(*bodyId));
               float force = (dx != 0 && dy != 0) ? 0.7071067811865475f : 1.f;
               force *= 25.f * b2Body_GetMass(*bodyId);
               b2Vec2 forceVec = b2Vec2{dx * force, dy * force};
               b2Body_ApplyForce(*bodyId, forceVec, b2Vec2_zero, true);
               if (lenVel > velocity->maxSpeed)
               {
                    b2Body_SetLinearVelocity(*bodyId, b2Normalize(b2Body_GetLinearVelocity(*bodyId)) * velocity->maxSpeed);
               }
          }

          // float add = 0, speed = 0;
          // float len = b2Length(velocity->progress); // 原始长度
          // float oldVel = b2Length(b2Body_GetLinearVelocity(*bodyId));
          // if (dx == 0 && dy == 0)
          // {
          //      add = (dx != 0 && dy != 0) ? velocity->stepD / std::sqrt(2.f) : velocity->stepD;
          //      if (len > velocity->stepD)
          //           velocity->progress = b2Normalize(velocity->progress) * (len - velocity->stepD);
          //      else
          //           velocity->progress = b2Vec2_zero;
          //      speed = easeInCubic(b2Length(velocity->progress)) * velocity->maxSpeed;
          // }
          // else
          // {
          //      add = (dx != 0 && dy != 0) ? velocity->stepA / std::sqrt(2.f) : velocity->stepA;
          //      velocity->progress += b2Vec2{dx * add, dy * add};
          //      if (len > 1.f)
          //           velocity->progress = b2Normalize(velocity->progress) * len;
          //      else if (b2Length(velocity->progress) > 1.f)
          //           velocity->progress = b2Normalize(velocity->progress) * 1.f;
          //      speed = easeOutSine(b2Length(velocity->progress)) * velocity->maxSpeed;
          // }
          // speed = speed * 0.3 + 0.7 * oldVel;
          // b2Vec2 vel = b2Normalize(velocity->progress) * speed;
          // b2Body_SetLinearVelocity(*bodyId, vel);
     }
}