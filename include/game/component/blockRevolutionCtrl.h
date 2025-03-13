// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
// 方块公转控制器组件
// 时间为revolutionTime(秒)
struct BlockRevolutionCtrl
{
     /**
      * 目标速度大小
      * nowVelocity = (1.f - rate) * nowVelocity + rate * targetSpeed;
      */
     float targetSpeed;
     float rate;              // 过度到目标速度的速率
     uint16_t revolutionTime; // 公转一圈需要时间(秒) 每tick的朝向 velocityDirection += 2*PI/(revolutionTime/TPS) 逆时针就减
     bool isClockwise = true; // 是否顺时针转动
     float velocityAngle = 0.f;     // 速度方向(弧度制)
};
