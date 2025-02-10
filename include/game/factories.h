#pragma once
#include "ecs/EntityManager.h"
#include "component/fwd.h"
#include "net/TcpConnection.h"

#include "box2d/box2d.h"
// 创建玩家
ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, TcpConnection *tcpConnection, GroupIndex groupIndex);

// 创建资源方块
ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, RegularPolygon regularPolygon, float x, float y);

// 创建子弹
ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, ecs::Entity player);