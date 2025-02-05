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

          /// 注册组件
          /// 请在程序最开始时注册所有组件，注册顺序严格按照 协议表的顺序
          /// 不需要同步到前端的组件可以不注册
          template <typename Component>
          static void registerComponent()
          {
               getID<Component>();
          }

          /// 获取组件类型ID列表
          template <typename... Components>
          static std::vector<uint32_t> getIDList() { return {getID<Components>()...}; }
     };
} // namespace ecs