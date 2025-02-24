#include "game/system/TestFire.h"
#include "game/component/fwd.h"
#include "game/factories.h"
#include "box2d/box2d.h"

void TestFireSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick)
{
     auto group = em.group<Input, b2BodyId>();
     for (auto entity : *group)
     {
          Input *input = em.getComponent<Input>(entity);
          if (input->state & 0b01) // 左键
          {
               auto &barrelList = em.getComponent<Children>(entity)->children;
               for (auto barrelEntity : barrelList)
               {
                    auto *barrel = em.getComponent<Barrel>(barrelEntity);
                    if (tick - barrel->LastTick >= barrel->cooldown)
                         em.addComponent<CanFire>(barrelEntity);
               }
          }
     }

     static std::vector<ecs::Entity> willDeleteCanFire;
     willDeleteCanFire.clear();
     auto group2 = em.group<CanFire, Barrel>();
     for (auto entity : *group2)
     {
          auto *barrel = em.getComponent<Barrel>(entity);
          Parent *parent = em.getComponent<Parent>(entity);
          float angle = em.getComponent<Angle>(parent->parent)->angle + barrel->offsetAngle;
          createEntityBullet(em, worldId, tick, parent->parent, angle);
          barrel->LastTick = tick;
          willDeleteCanFire.push_back(entity);
     }
     for (auto entity : willDeleteCanFire)
     {
          em.removeComponent<CanFire>(entity);
     }
}