#include "ecs/EntityManager.h"

// Group &ecs::EntityManager::group(const std::vector<uint32_t> &direct, const std::vector<uint32_t> &indirect)
// {
//      groups_.emplace_back(direct, indirect);
//      return groups_.back();
// }

std::vector<ecs::Entity> ecs::EntityManager::getEntities()
{
     std::vector<Entity> entities;
     for (auto &entity : entities_)
     {
          if (entityIsValid(*entity))
          {
               entities.push_back(*entity);
          }
     }
     return entities;
}

uint32_t ecs::EntityManager::getEntityNum() const
{
     return entityNum_;
}

ecs::Entity *ecs::EntityManager::getEntityPtr(ecs::Entity entity)
{
     return entities_[ecs::entityToIndex(entity)].get();
}

ecs::Entity ecs::EntityManager::createEntity()
{
     ecs::Entity newEntity;
     if (availableEntityIndex_ == entities_.size())
     {
          newEntity = availableEntityIndex_++; // 高12位的版本号已经是0x000
          entities_.push_back(std::make_unique<ecs::Entity>(newEntity));
     }
     else
     {
          newEntity = (*entities_[availableEntityIndex_] & 0xFFF00000u) | availableEntityIndex_;
          availableEntityIndex_ = ecs::entityToIndex(*entities_[availableEntityIndex_]);
          *entities_[ecs::entityToIndex(newEntity)] = newEntity;
     }
     entityNum_++;
     OnEntityCreated(newEntity);
     return newEntity;
}

void ecs::EntityManager::destroyEntity(Entity entity)
{
     if (!entityIsValid(entity))
          return;
     // 将Entity从EntityManager管理的池子中删除
     for (auto &[_, pool] : ComponentPools::componentPools)
     {
          pool->try_erase(entity);//这儿并不会触发handleComponentRemoved
     }
     // 跳转版本号和entities_
     uint32_t index = ecs::entityToIndex(entity);
     uint32_t version = ecs::entityToVersion(entity) + 1; // 版本号加1
     if (version == deadEntity)
          version = 0;
     *entities_[index] = (version << 20) | availableEntityIndex_;
     availableEntityIndex_ = index;
     entityNum_--;
     // 通过完全组删除实体
     OnEntityDestroyed(entity); // 使用旧实体
}

bool ecs::EntityManager::entityIsValid(Entity entity)
{
     return (ecs::entityToIndex(entity) < entities_.size() && *entities_[ecs::entityToIndex(entity)] == entity);
}