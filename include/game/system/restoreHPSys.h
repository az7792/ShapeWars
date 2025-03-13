// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "ecs/EntityManager.h"
#include "box2d/id.h"
void restoreHPSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick);