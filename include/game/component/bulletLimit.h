// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
/**
 * 限制实体已发射的子弹数量
 * 只对无人机坦克生效
*/
struct BulletLimit
{
     uint8_t num;
     uint8_t max;
};
