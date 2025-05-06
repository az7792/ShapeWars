// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/utils.h"
#include "config/config.h"
#include <iostream>

b2Vec2 SmoothDampVelocity(b2Vec2 current, b2Vec2 target, b2Vec2 currentVelocity, float smoothTime, float maxSpeed)
{
     // // 计算目标方向
     // b2Vec2 direction = target - current;
     // float distance = b2Length(direction);

     // // 如果距离很小，直接返回零速度
     // if (distance < 0.02f) // 小于1px
     // {
     //      return b2Vec2_zero;
     // }

     // // 计算目标速度
     // b2Vec2 targetVelocity = b2Normalize(direction) * (distance / smoothTime);

     // // 限制目标速度
     // if (b2Length(targetVelocity) > maxSpeed)
     // {
     //      targetVelocity = b2Normalize(targetVelocity) * maxSpeed;
     // }

     // // 平滑速度
     // float omega = 2.0f / smoothTime;
     // float x = omega / TPS; // omega * deltaTime;
     // float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

     // b2Vec2 newVelocity = (currentVelocity - (currentVelocity - targetVelocity) * exp);

     // return newVelocity;

     // Based on Game Programming Gems 4 Chapter 1.10
     b2Vec2 maxSpeedVec = b2Abs(b2Normalize(target - current) * maxSpeed);

     smoothTime = std::max(0.0001f, smoothTime);
     float omega = 2.f / smoothTime;

     float deltaTime = 1.f / TPS;

     float x = omega * deltaTime;
     float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
     b2Vec2 change = current - target;
     b2Vec2 originalTo = target;

     // Clamp maximum speed
     b2Vec2 maxChange = maxSpeedVec * smoothTime;
     change = b2Clamp(change, -maxChange, maxChange);
     target = current - change;

     b2Vec2 temp = (currentVelocity + omega * change) * deltaTime;
     b2Vec2 output = target + (change + temp) * exp;

     // Prevent overshooting
     if ((originalTo.x > current.x) == (output.x > originalTo.x))
     {
          output.x = originalTo.x;
     }
     if ((originalTo.y > current.y) == (output.y > originalTo.y))
     {
          output.y = originalTo.y;
     }
     return (output - current) * TPS;
}