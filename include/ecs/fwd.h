// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
namespace ecs
{
     class EntityManager;

     template <typename, typename>
     class Group;

     /// @brief 实体类型
     /// @note 高12位为版本，低20位为ID
     using Entity = uint32_t;

     constexpr Entity nullEntity = 0xFFFFFFFFu;

     template <typename... Type>
     struct type_list
     {
          using type = type_list;
          constexpr static auto size = sizeof...(Type);
     };
}