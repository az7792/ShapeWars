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
#include "type/type.h"
#include "attack.h"
#include "contactList.h"
#include "type/borderWall_f.h"
#include "groupIndex.h"
#include "name.h"
#include "style.h"
#include "parent.h"
#include "children.h"
#include "flag/deleteFlag.h"
#include "flag/canFire.h"
#include "healingOverTime.h"
#include "bulletAttackNum.h"
#include "blockRotationCtrl.h"
#include "blockRevolutionCtrl.h"
#include "barrel.h"

#include <cstdint>

const uint64_t COMP_POSITION = (1ull << 0);   // 0b00000001 位置
const uint64_t COMP_VELOCITY = (1ull << 1);   // 0b00000010 速度
const uint64_t COMP_ANGLE = (1ull << 2);      // 0b00000100 朝向
const uint64_t COMP_POLYGON = (1ull << 3);    // 0b00001000 正多边形
const uint64_t COMP_HP = (1ull << 4);         // 0b00010000 血量
const uint64_t COMP_TYPE = (1ull << 5);       // 0b00100000 类型
const uint64_t COMP_GROUPINDEX = (1ull << 6); // 0b01000000 碰撞组索引
const uint64_t COMP_NAME = (1ull << 7);       // 0b10000000 名称
const uint64_t COMP_STYLE = (1ull << 8);      //0b100000000 填充样式
const uint64_t COMP_BARRELLIST = (1ull << 9);      //枪管列表