// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <stdint.h>
/**
 * 炮管由梯形组成
 * @param widthL 靠近圆心侧梯形宽度
 * @param widthR 远离圆心侧梯形宽度
 * @param length 梯形长度
 * @param nowLength 当前长度
 * @param offsetAngle 相对玩家朝向的偏移角度
 * @param cooldown 冷却时间(tick) 必须是2的倍数
 * @param LastTick 上一次发射的时间(tick)
 */
struct Barrel
{
     float widthL;          // 靠近圆心侧梯形宽度
     float widthR;          // 远离圆心侧梯形宽度
     float length;          // 梯形长度
     float nowLength;       // 当前长度
     float offsetAngle = 0; // 相对玩家朝向的偏移角度
     uint32_t cooldown = 0; // 冷却时间(tick)
     uint32_t LastTick = 0; // 上一次发射的时间(tick)
};
