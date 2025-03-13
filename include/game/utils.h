// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
#include <cstring>
#include "ecs/utils.h"
#include "box2d/math_functions.h"

// 向字符串追加指定类型的数据
template <typename T>
void strAppend(std::string &str, const T &t)
{
     char data[sizeof(T)];
     std::memcpy(data, &t, sizeof(T));
     str += std::string(data, sizeof(T));
};

b2Vec2 SmoothDampVelocity(b2Vec2 current, b2Vec2 target, b2Vec2 currentVelocity, float smoothTime, float maxSpeed);