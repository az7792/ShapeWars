// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "ecs/EntityManager.h"
#include "component/fwd.h"
#include "net/TcpConnection.h"
#include <unordered_map>

#include "box2d/box2d.h"

// ShapeID -> Entity的映射
inline std::unordered_map<uint64_t, ecs::Entity> shapeEntityMap;

// willDeleteShapes中保存将要删除的形状ID
inline std::vector<b2ShapeId> willDeleteShapes;

// 炮管实体的配置参数
struct BarrelParams
{
     Barrel barrel;                              // 炮管的配置参数
     ecs::Entity parentEntity = ecs::nullEntity; // 炮管的父实体
};

// 玩家实体的配置参数
struct PlayerParams
{
     TcpConnection *tcpConnection = nullptr; // 玩家的TCP连接
     int32_t groupIndex = 0;                 // 玩家所在碰撞组的索引
     std::string name;                       // 玩家的名字
     int16_t attack = 1 * TPS;               // 玩家刚体的攻击力
     float maxSpeed = 5.f;                   // 玩家的最大线速度
     b2Vec2 position{0.f, 0.f};              // 玩家的初始位置
     int16_t initialHP = 1000;               // 玩家的初始血量
     int16_t maxHP = 1000;                   // 玩家的最大血量
     float polygonRadius = 0.5f;             // 玩家的刚体半径
};

// 子弹实体的配置参数
struct BulletParams
{
     ecs::Entity parentEntity;  // 子弹的父实体
     int16_t attack = 13 * TPS; // 子弹的攻击力
     int16_t initialHP = 100;   // 子弹的初始血量
     int16_t maxHP = 100;       // 子弹的最大血量
     uint32_t lifetime = 30;    // 子弹的存活时间
     float speed = 10.0f;       // 子弹的速度
     float radius = 0.2f;       // 子弹的半径
     float angle = 0.0f;        // 子弹的发射角度
};

// 方块实体的配置参数
struct BlockParams
{
     RegularPolygon polygon;    // 方块的形状
     Style style;               // 方块的样式
     b2Vec2 position{0.f, 0.f}; // 方块的初始位置
     int32_t score = 10;        // 方块的分数
     int16_t attack = 2 * TPS;  // 方块的攻击力
     int16_t initialHP = 100;   // 方块的初始血量
     int16_t maxHP = 100;       // 方块的最大血量
};

// 创建玩家
ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const PlayerParams &params);

/**
 *  为玩家添加num个炮管，炮管均匀分布在圆上
 *  默认从参数给定的offsetAngel为起点旋转
 */
bool addBarrelsToPlayer(ecs::EntityManager &em, uint8_t num, BarrelParams params);

// 创建资源方块
ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const BlockParams &params);

// 创建炮管
ecs::Entity createEntityBarrel(ecs::EntityManager &em, const BarrelParams &params);

// 创建子弹
ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const BulletParams &params);

/**
 * 以(0,0)为中心，建立边界墙
 * 使用的长宽为半宽，因此建立x*y的边界墙需要传递(x/2,y/2)
 * @param width 边界墙宽度(半宽)
 * @param height 边界墙高度(半高)
 */
ecs::Entity createEntityBrderWall(ecs::EntityManager &em, b2WorldId &worldId, float width, float height);