// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <stdint.h>
/**
 * 炮管由梯形组成,默认朝向右边
 * @param widthL 靠近圆心侧梯形宽度
 * @param widthR 远离圆心侧梯形宽度
 * @param length 梯形长度
 * @param nowLength 当前长度
 * @param offsetAngle 相对玩家朝向的偏移角度
 * @param cooldown 冷却时间(tick) 必须是>=2
 * @param LastTick 上一次发射的时间(tick)
 */
struct Barrel
{
     float widthL;          // 靠近圆心侧梯形宽度
     float widthR;          // 远离圆心侧梯形宽度
     float length;          // 梯形长度
     float nowLength;       // 当前长度
     float offsetAngle = 0; // 相对玩家朝向的偏移角度
     float offsetY = 0.3;   // 相对玩家位置的偏移量(>0 向上移动 , <0 向下移动)
     uint32_t delay = 0;    // 发射延迟(tick)
     uint32_t cooldown = 0; // 冷却时间(tick)
     uint32_t LastTick = 0; // 上一次发射的时间(tick)
};
