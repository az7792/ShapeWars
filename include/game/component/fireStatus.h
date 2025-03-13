// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
/**
 * 0b00000000: 不能开火
 * 0b00000001: 可以开火
 * 0b00000010: 开火中，第一段，炮管往回(靠近圆心)收
 * 0b00000100: 开火中，第二段，炮管往外(原理圆心)收
 */
struct FireStatus
{
     uint8_t status;
};
