// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "ecs/EntityManager.h"
#include "box2d/box2d.h"
#include "game/factories.h"

// 创建默认坦克
ecs::Entity defaultTank(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const PlayerParams &params);

// 创建双管坦克
ecs::Entity doubleTank(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const PlayerParams &params);