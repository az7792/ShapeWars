#pragma once
#include <cstdint>
/// @brief 通过TypeID::get<type>()获取该类型的唯一类型ID
/// @warning 线程不安全
class TypeID
{ 

     inline static uint32_t idIndex = 0; // C++ 17 的写法

public:
     /// @brief 获取类型的唯一ID
     /// @tparam  类型
     /// @return uint32:ID
     template <typename>
     static uint32_t get() noexcept
     {
          static const uint32_t value = idIndex++;
          return value;
     }
};