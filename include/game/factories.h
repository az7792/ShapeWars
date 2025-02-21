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

// 创建玩家
ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, TcpConnection *tcpConnection, GroupIndex groupIndex, std::string name);

// 创建资源方块
ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, RegularPolygon regularPolygon, float x, float y);

// 创建子弹
ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, ecs::Entity player);

/**
 * 以(0,0)为中心，建立边界墙
 * 使用的长宽为半宽，因此建立x*y的边界墙需要传递(x/2,y/2)
 * @param width 边界墙宽度(半宽)
 * @param height 边界墙高度(半高)
 */
ecs::Entity createEntityBrderWall(ecs::EntityManager &em, b2WorldId &worldId, float width, float height);