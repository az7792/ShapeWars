#pragma once
#include "fwd.h"
#include <cstdint>
namespace ecs
{
     /// @brief 获取实体的索引部分
     inline uint32_t entityToIndex(Entity entity) { return entity & 0x000FFFFFu; }

     /// @brief 获取实体的版本部分
     inline uint32_t entityToVersion(Entity entity) { return (entity & 0xFFF00000u) >> 20; }
}