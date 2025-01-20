#pragma once
#include <vector>
#include <cstdint>
#include "fwd.h"
#include <iostream>
#include "utils/SparseSet.h"

namespace ecs
{
     template <typename, typename>
     class Group;

     template <typename... includeComponents, typename... excludeComponents>
     class Group<type_list<includeComponents...>, type_list<excludeComponents...>>
     {
     private:
          friend class EntityManager;
          SparseSet entities_;

          void handleEntityDestroyed(Entity entity)
          {
               entities_.erase(ecs::entityToIndex(entity));
          }

          void handleComponentAdded(Entity entity)
          {
               if ((ComponentPools::get<includeComponents>().has(entity) && ...) && (!ComponentPools::get<excludeComponents>().has(entity) && ...))
               {
                    entities_.insert(ecs::entityToIndex(entity));
               }
               else
               {
                    entities_.erase(ecs::entityToIndex(entity)); // 由于过滤，之前的实体可能不再满足条件
               }
          }

          void handleComponentRemoved(Entity entity)
          {
               entities_.erase(ecs::entityToIndex(entity));
          }

     public:
          Group() = default;
          ~Group() = default;

          /// begin只读
          std::vector<uint32_t>::const_iterator begin() const { return entities_.begin(); }
          /// end只读
          std::vector<uint32_t>::const_iterator end() const { return entities_.end(); }

     private:
     };
}