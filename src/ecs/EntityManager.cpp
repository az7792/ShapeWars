#include "ecs/EntityManager.h"


// Group &ecs::EntityManager::group(const std::vector<uint32_t> &direct, const std::vector<uint32_t> &indirect)
// {
//      groups_.emplace_back(direct, indirect);
//      return groups_.back();
// }

uint32_t ecs::EntityManager::getEntityNum() const
{
     return entityNum_;
}

ecs::Entity ecs::EntityManager::createEntity()
{
     ecs::Entity newEntity;
     if (availableEntityIndex_ == entities_.size())
     {
          newEntity = availableEntityIndex_++; // 高12位的版本号已经是0x000
          entities_.push_back(newEntity);
     }
     else
     {
          newEntity = (entities_[availableEntityIndex_] & 0xFFF00000u) | availableEntityIndex_;
          availableEntityIndex_ = ecs::entityToIndex(entities_[availableEntityIndex_]);
          entities_[ecs::entityToIndex(newEntity)] = newEntity;
     }
     entityNum_++;
     return newEntity;
}

bool ecs::EntityManager::entityIsValid(Entity entity)
{
     return (ecs::entityToIndex(entity) < entities_.size() && entities_[ecs::entityToIndex(entity)] == entity);
}