#pragma once
#include "camera.h"
#include "input.h"
#include "position.h"
#include "velocity.h"
#include "angle.h"
#include "HP.h"
#include "packData.h"
#include "regularPolygon.h"
#include "box2d/id.h"
#include "type.h"
#include "attack.h"
#include "attackList.h"

#include <cstdint>

const uint64_t COMP_POSITION = (1ull << 0); // 0b00000001 位置
const uint64_t COMP_VELOCITY = (1ull << 1); // 0b00000010 速度
const uint64_t COMP_ANGLE = (1ull << 2);    // 0b00000100 朝向
const uint64_t COMP_POLYGON = (1ull << 3);  // 0b00001000 正多边形
const uint64_t COMP_HP = (1ull << 4);       // 0b00010000 血量
const uint64_t COMP_TYPEID = (1ull << 5);   // 0b00100000 类型ID