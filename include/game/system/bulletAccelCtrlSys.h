// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "ecs/EntityManager.h"
#include "box2d/id.h"

// 子弹加速度控制系统
void bulletAccelCtrlSys(ecs::EntityManager &em, b2WorldId &worldId, uint32_t &tick);