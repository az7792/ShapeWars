#pragma once
#include <vector>
#include <cstdint>

namespace ecs
{
     /// 组件类型ID管理器
     class ComponentTypeID
     {
          static uint32_t add()
          {
               static uint32_t componentIDIndex = 0; // 组件ID索引
               return componentIDIndex++;
          }

     public:
          /// 获取组件类型ID
          template <typename Component>
          static uint32_t getID()
          {
               static const uint32_t componentId = add();
               return componentId;
          }


          /// 获取组件类型ID列表
          template <typename... Components>
          static std::vector<uint32_t> getIDList() { return {getID<Components>()...}; }
     };
} // namespace ecs