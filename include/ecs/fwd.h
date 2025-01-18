#pragma once

namespace ecs
{
     class Group;
     class EntityManager;
     class View;

     /// @brief 实体类型
     /// @note 高12位为版本，低20位为ID
     using Entity = uint32_t;

     template <typename... Type>
     struct type_list
     {
          using type = type_list;
          constexpr static auto size = sizeof...(Type);
     };
}