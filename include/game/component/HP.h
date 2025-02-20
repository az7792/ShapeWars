#pragma once
#include <cstdint>
struct HP
{
     int16_t hp;
     int16_t maxHP;
     uint32_t tick = 0;//上次更新的时间
};
