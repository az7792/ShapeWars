// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/tankFactory/tankFactory.h"

ecs::Entity doubleTank(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const PlayerParams &params)
{
     ecs::Entity player = createEntityPlayer(em, worldId, tick, params);

     BarrelParams barrelParams;
     barrelParams.parentEntity = player;
     barrelParams.bulletParams.parentEntity = player;
     barrelParams.barrel.widthL = barrelParams.barrel.widthR = 0.45f;
     barrelParams.barrel.length = barrelParams.barrel.nowLength = 1.f;
     barrelParams.barrel.offsetAngle = 0.f;
     barrelParams.barrel.cooldown = 15;
     barrelParams.barrel.offsetY = 0.25f;

     // 创建炮管并添加到玩家子实体中
     createEntityBarrel(em, barrelParams);
     barrelParams.barrel.offsetY = -0.25f;
     barrelParams.barrel.delay = 10;
     createEntityBarrel(em, barrelParams);

     return player;
}