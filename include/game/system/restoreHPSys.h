#pragma once
#include "ecs/EntityManager.h"
#include "box2d/id.h"
void restoreHPSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick);