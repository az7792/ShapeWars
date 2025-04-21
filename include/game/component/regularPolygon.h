// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
/**
 * 正多边形组件
 * @param radius 半径
 * @param sides 边数
 * @note sides最高位1为星形，0为正多边形，低7位为边数
 */
struct RegularPolygon
{
     uint8_t sides;
     float radius;
};
