// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <stdint.h>
// 实体的生命周期
struct LifeTime
{
     uint32_t startTick;        // 开始时间
     uint32_t durationTick = 0; // 持续时间
};