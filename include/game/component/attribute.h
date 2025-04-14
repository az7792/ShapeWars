#pragma once
#include <cstdint>
#include <array>
/**
 * index:
  - 0 ：回复速度
  - 1 ：最大生命值
  - 2 ：身体碰撞伤害
  - 3 ：子弹伤害
  - 4 ：子弹飞行时长
  - 5 ：子弹血量
  - 6 ：子弹速度
  - 7 ：子弹密度
  - 8 ：子弹射速
  - 9 ：角色移动速度
 */
struct Attribute
{
     std::array<uint8_t, 10> attr;
};
