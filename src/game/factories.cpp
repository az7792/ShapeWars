// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#include "game/factories.h"
#include "config/config.h"
#include <ctime>

ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const PlayerParams &params)
{
     assert(params.tcpConnection != nullptr);
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<Angle>(e, 0.f);
     em.addComponent<HP>(e, params.initialHP, params.maxHP, tick);
     em.addComponent<Attack>(e, params.attack);
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Input>(e, 0.f, 0.f, 0ull);
     em.addComponent<TcpConnection *>(e, params.tcpConnection);
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_PLAYER));
     em.addComponent<GroupIndex>(e, params.groupIndex);
     em.addComponent<Name>(e, params.name);
     em.addComponent<RegularPolygon>(e, static_cast<uint8_t>(64), params.polygonRadius); //>=16为圆形
     em.addComponent<Children>(e);
     em.addComponent<Camera>(e, params.position.x, params.position.y, 1.f, true);
     em.addComponent<Score>(e, static_cast<int32_t>(1e5)); // HACK : 1e5 测试用
     em.addComponent<Attribute>(e);
     em.addComponent<HealingOverTime>(e, tick);
     em.addComponent<TankID>(e, params.tankID, tick);
     em.addComponent<AttackFilter>(e, params.groupIndex);
     if (params.isDrone)
     {
          em.addComponent<BulletLimit>(e, static_cast<uint8_t>(0), static_cast<uint8_t>(10));
     }
     Camera *camera = em.getComponent<Camera>(e);
     camera->bodyId = camera->createSensor(worldId);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = params.position;
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 4.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_PLAYER;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_BORDER_WALL | CATEGORY_DRONE;
     shapeDef.filter.groupIndex = params.groupIndex;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = params.polygonRadius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

bool addBarrelsToPlayer(ecs::EntityManager &em, uint8_t num, BarrelParams params)
{
     assert(params.parentEntity != ecs::nullEntity);
     float addAngle = 2.f * M_PI / num;
     for (uint8_t i = 0; i < num; i++)
     {
          createEntityBarrel(em, params);
          params.barrel.offsetAngle += addAngle;
     }
     return true;
}

ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const BlockParams &params)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<Angle>(e, 0.f);
     em.addComponent<BlockRotationCtrl>(e, static_cast<uint16_t>(30), static_cast<bool>(std::rand() % 2));
     em.addComponent<BlockRevolutionCtrl>(e, 0.5f, 0.1f, static_cast<uint16_t>(30), static_cast<bool>(std::rand() % 2), static_cast<float>(std::rand() % 360 / 180.f * M_PI));
     em.addComponent<HP>(e, params.initialHP, params.maxHP, tick);
     em.addComponent<Attack>(e, params.attack);
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_BLOCK));
     em.addComponent<RegularPolygon>(e, params.polygon);
     em.addComponent<Style>(e, params.style);
     em.addComponent<Score>(e, params.score);
     em.addComponent<HealingOverTime>(e, tick);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = params.position;
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     // shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_BLOCK;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_BORDER_WALL | CATEGORY_DRONE;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = params.polygon.radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBarrel(ecs::EntityManager &em, const BarrelParams &params)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Barrel>(e, params.barrel);
     em.addComponent<BulletParams>(e, params.bulletParams);
     em.addComponent<FireStatus>(e, static_cast<uint8_t>(0b00000000));
     em.addComponent<Parent>(e, params.parentEntity);

     // 添加到父对象的炮管列表中
     em.getComponent<Children>(params.parentEntity)->children.push_back(e);
     return e;
}

ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, const BulletParams &params)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<Angle>(e, params.angle);
     em.addComponent<HP>(e, params.initialHP, params.maxHP, tick);
     em.addComponent<Attack>(e, params.attack);
     em.addComponent<LifeTime>(e, tick, params.lifetime);
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_BULLET));
     em.addComponent<GroupIndex>(e, em.getComponent<GroupIndex>(params.parentEntity)->index);
     em.addComponent<Parent>(e, params.parentEntity);
     em.addComponent<RegularPolygon>(e, params.sides, params.radius); //>=16为圆形
     em.addComponent<Score>(e, 0);
     if ((params.sides & 0x7F) >= 16 && params.isCtrl == false) // 只控制圆形子弹的生命周期
          em.addComponent<BulletAccelerationFlag>(e);
     if (params.isCtrl)
     {
          em.addComponent<DroneFlag>(e);
          em.addComponent<AttackFilter>(e, em.getComponent<GroupIndex>(params.parentEntity)->index);
     }

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = params.position;
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     bodyDef.linearDamping = 0.7f;
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = params.density; // 默认为1
     shapeDef.friction = 0.1f;          // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     if (params.isCtrl)
          shapeDef.filter.categoryBits = CATEGORY_DRONE;
     else
          shapeDef.filter.categoryBits = CATEGORY_BULLET;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_DRONE;
     if (!params.isCtrl)
          shapeDef.filter.groupIndex = em.getComponent<GroupIndex>(params.parentEntity)->index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = em.getComponent<RegularPolygon>(e)->radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;
     b2Vec2 vel = (b2Vec2){cosf(params.angle), sinf(params.angle)};
     vel = b2Normalize(vel) * params.speed;
     b2Body_SetLinearVelocity(bodyId, vel);

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBrderWall(ecs::EntityManager &em, b2WorldId &worldId, float width, float height)
{
     auto entity = em.createEntity();
     em.addComponent<BorderWall_F>(entity);

     // 创建墙
     b2Segment s;
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.position = (b2Vec2){0.f, 0.f};
     bodyDef.fixedRotation = true;
     b2BodyId Wall = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef(); // 默认为静态
     shapeDef.userData = static_cast<void *>(em.getEntityPtr(entity));
     shapeDef.filter.categoryBits = CATEGORY_BORDER_WALL;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER;
     shapeDef.enableContactEvents = false;
     // up
     s.point1 = (b2Vec2){-width, height};
     s.point2 = (b2Vec2){width, height};
     shapeEntityMap[b2StoreShapeId(b2CreateSegmentShape(Wall, &shapeDef, &s))] = entity;
     // left
     s.point1 = (b2Vec2){width, height};
     s.point2 = (b2Vec2){width, -height};
     shapeEntityMap[b2StoreShapeId(b2CreateSegmentShape(Wall, &shapeDef, &s))] = entity;
     // down
     s.point1 = (b2Vec2){width, -height};
     s.point2 = (b2Vec2){-width, -height};
     shapeEntityMap[b2StoreShapeId(b2CreateSegmentShape(Wall, &shapeDef, &s))] = entity;
     // right
     s.point1 = (b2Vec2){-width, -height};
     s.point2 = (b2Vec2){-width, height};
     shapeEntityMap[b2StoreShapeId(b2CreateSegmentShape(Wall, &shapeDef, &s))] = entity;

     return entity;
}
