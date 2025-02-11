#pragma once

#include <vector>
#include "box2d/id.h"

struct ContactList
{
     std::vector<b2ShapeId> list; // 接触的形状列表
};
