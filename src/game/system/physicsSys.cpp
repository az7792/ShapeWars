// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/system/physicsSys.h"
#include "box2d/box2d.h"
#include "config/config.h"

void physicsSys(b2WorldId &worldId)
{
     b2World_Step(worldId, 1.0f / TPS, 4);
}