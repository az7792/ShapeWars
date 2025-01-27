#include "ecs/ComponentPool.h"

namespace ecs
{
     namespace ComponentPools
     {
          SparseMap<ComponentPoolBase *> componentPools;
     }
}