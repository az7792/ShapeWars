// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
/// @brief 持续对自己进行加血
/// @warning 初始化时 *TPS, 使用时 /TPS
struct HealingOverTime
{
     uint32_t startTick;               // 获取该组件的时间
     int16_t healing = 0;              // 每秒加血量（为负即为减血）
     int16_t healingTarget = 1 * TPS;  // 目标加血量
     uint32_t durationTick = 20 * TPS; // 持续时间
     uint32_t cooldownTick = 15 * TPS; // 冷却时间
};
