#include "game/utils.h"
#include "config/config.h"

b2Vec2 SmoothDampVelocity(b2Vec2 current, b2Vec2 target, b2Vec2 currentVelocity, float smoothTime, float maxSpeed)
{
     // 计算目标方向
     b2Vec2 direction = target - current;
     float distance = b2Length(direction);

     // 如果距离很小，直接返回零速度
     if (distance < 0.002f) // 小于1px
     {
          return b2Vec2_zero;
     }

     // 计算目标速度
     b2Vec2 targetVelocity = b2Normalize(direction) * (distance / smoothTime);

     // 限制目标速度
     if (b2Length(targetVelocity) > maxSpeed)
     {
          targetVelocity = b2Normalize(targetVelocity) * maxSpeed;
     }

     // 平滑速度
     float omega = 2.0f / smoothTime;
     float x = omega / TPS; // omega * deltaTime;
     float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

     b2Vec2 newVelocity = (currentVelocity - (currentVelocity - targetVelocity) * exp);

     return newVelocity;
}