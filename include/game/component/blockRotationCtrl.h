// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
// 方块自转控制组件
// rotationTime(秒)
struct BlockRotationCtrl
{
     uint16_t rotationTime; // 自转一圈的时间(秒) 每tick转动的角度 angle += 2*PI/(rotationTick/TPS) 逆时针就减
     // 当前角度使用单独的Angle组件存储
     bool isClockwise = true; // 是否顺时针转动
};
