#pragma once
#include <vector>
#include "ecs/fwd.h"
#include <cstdint>
#include <cstddef>
namespace ecs
{
     class EntitySet
     {
     private:
          constexpr static const uint32_t nullValue = 0xffffffff; // 标记空位置
          std::vector<Entity> dense_;                             // 稠密数组,存放实际数据
          std::vector<Entity> sparse_;                            // 稀疏数组，存放索引
     public:
          EntitySet(size_t size = 1000);
          ~EntitySet() = default;

          /// @brief 插入一个数
          /// @exception 如果valu>=size，会抛出异常
          void insert(Entity v);

          /// @brief 去除一个数
          void erase(Entity v);

          /// @brief 判断是否存在某个数
          bool find(Entity v) const;

          /// begin只读
          std::vector<Entity>::const_iterator begin() const { return dense_.begin(); }
          /// end只读
          std::vector<Entity>::const_iterator end() const { return dense_.end(); }
     };
} // namespace ecs