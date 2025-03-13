// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
struct PackData
{
     std::string createData;
     std::string updateData;
     bool isCreated = false;
     bool isUpdated = false;
     //为了隐式弃置移动赋值运算符，保证在插入或者替换组件时调用拷贝赋值，这样可以保留旧实体中动态容器已分配的空间
     PackData &operator=(const PackData &other) = default;
};
