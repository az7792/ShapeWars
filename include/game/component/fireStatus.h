// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
/**
 * 0b00000000: 空闲1 ： 初始空闲状态，需要delay
 * 0b00000001: 空闲2 ： 等待delay中
 * 0b00000010: 空闲3 ： 等待开火CD中，不需要delay
 * 0b00000100: 开火
 * 0b00001000: 开火中，第一段，炮管往回(靠近圆心)收
 * 0b00010000: 开火中，第二段，炮管往外(原理圆心)收
 */
struct FireStatus
{
     uint8_t status;
};
