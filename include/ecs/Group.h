#pragma once
#include <vector>
#include <cstdint>
#include "fwd.h"
#include <iostream>
#include "ecs/EntitySet.h"

namespace ecs
{
     template <typename, typename>
     class Group;

     template <typename... includeComponents, typename... excludeComponents>
     class Group<type_list<includeComponents...>, type_list<excludeComponents...>>
     {
     private:
          friend class EntityManager;
          EntitySet entities_;

          void handleEntityDestroyed(Entity entity)
          {
               entities_.erase(entity);
          }

          void handleComponentAdded(Entity entity)
          {
               if ((ComponentPools::get<includeComponents>().has(entity) && ...) && (!ComponentPools::get<excludeComponents>().has(entity) && ...))
               {
                    entities_.insert(entity);
               }
               else
               {
                    entities_.erase(entity); // 由于过滤，之前的实体可能不再满足条件，例如加入了excludeComponents中的组件
               }
          }

          void handleComponentRemoved(Entity entity)
          {
               if ((ComponentPools::get<includeComponents>().has(entity) && ...) && (!ComponentPools::get<excludeComponents>().has(entity) && ...))
               {
                    entities_.insert(entity); // 由于过滤，之前的实体不满足的实体现在可能满足了，例如移除了excludeComponents中的组件
               }
               else
               {
                    entities_.erase(entity);
               }
          }

     public:
          Group(const std::vector<Entity> &entities)
          {
               for (auto &entity : entities)
               {
                    handleComponentAdded(entity);
               }
          };
          Group() {};
          ~Group() = default;

          /// begin只读
          std::vector<uint32_t>::const_iterator begin() const { return entities_.begin(); }
          /// end只读
          std::vector<uint32_t>::const_iterator end() const { return entities_.end(); }

     private:
     };
}