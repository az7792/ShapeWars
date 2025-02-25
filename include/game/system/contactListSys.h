#pragma once
#include "ecs/EntityManager.h"
#include "box2d/id.h"
void contactListSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick);