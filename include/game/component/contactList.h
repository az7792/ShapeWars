// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include "box2d/id.h"

struct ContactList
{
     std::vector<b2ShapeId> list; // 接触的形状列表
     //为了隐式弃置移动赋值运算符，保证在插入或者替换组件时调用拷贝赋值，这样可以保留旧实体中动态容器已分配的空间
     ContactList &operator=(const ContactList &other) = default;
};
