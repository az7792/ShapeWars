#pragma once
#include <vector>
#include <cstdint>

namespace ecs
{
     /// 组件类型ID管理器
     class ComponentTypeID
     {
          inline static uint32_t componentIDIndex = 0; // 组件ID索引

     public:
          /// 注册组件
          template <typename Component>
          static uint32_t registerComponent()
          {
               static const uint32_t componentId = componentIDIndex++;
               return componentId;
          }

          /// 获取组件数量Comp
          static uint32_t getComponentCount() { return componentIDIndex; }

          /// 获取组件类型ID
          template <typename Component>
          static uint32_t getComponentID() { return registerComponent<Component>(); }

          /// 获取组件类型ID列表
          template <typename... Components>
          static std::vector<uint32_t> getComponentIDList() { return {getComponentID<Components>()...}; }
     };
} // namespace ecs