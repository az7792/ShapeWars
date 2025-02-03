#pragma once
#include <string>
#include <cstring>
#include "ecs/utils.h"
// 向字符串追加指定类型的数据
template <typename T>
void strAppend(std::string &str, const T &t)
{
     char data[sizeof(T)];
     if constexpr (std::is_same_v<T, ecs::Entity>)
     {
          uint32_t tmp = ecs::entityToIndex(t);
          std::memcpy(data, &tmp, sizeof(T));
     }
     else
     {
          std::memcpy(data, &t, sizeof(T));
     }
     str += std::string(data, sizeof(T));
};