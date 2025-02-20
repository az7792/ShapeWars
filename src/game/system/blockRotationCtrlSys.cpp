#include "game/system/blockRotationCtrlSys.h"
#include "game/component/fwd.h"

void blockRotationCtrlSys(ecs::EntityManager &em)
{
     auto group = em.group<Angle, BlockRotationCtrl>();
     for (auto entity : *group)
     {
          Angle *angle = em.getComponent<Angle>(entity);
          BlockRotationCtrl *ctrl = em.getComponent<BlockRotationCtrl>(entity);
          float tmp = 2 * M_PI / (ctrl->rotationTime * TPS);
          angle->angle += ctrl->isClockwise ? tmp : -tmp;
          //修正到[0,2PI)
          if (angle->angle >= 2 * M_PI)
               angle->angle -= 2 * M_PI;
          if (angle->angle < 0)
               angle->angle += 2 * M_PI;
     }
}