#include "game/factories.h"
#include "config/config.h"
#include <ctime>

ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, TcpConnection *tcpConnection, GroupIndex groupIndex)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<Angle>(e, 0.f);
     em.addComponent<HP>(e, static_cast<int16_t>(1000), static_cast<int16_t>(1000), tick);
     em.addComponent<Attack>(e, static_cast<int16_t>(2 * TPS));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Input>(e, 0.f, 0.f, 0ull);
     em.addComponent<TcpConnection *>(e, tcpConnection);
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_PLAYER));
     em.addComponent<GroupIndex>(e, groupIndex);
     em.addComponent<RegularPolygon>(e, static_cast<uint8_t>(64), 0.5f); //>=16为圆形
     em.addComponent<Camera>(e, 0.f, 0.f, 1.f);
     Camera *camera = em.getComponent<Camera>(e);
     camera->bodyId = camera->createSensor(worldId);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = {0.f, 0.f};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_PLAYER;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_BORDER_WALL;
     shapeDef.filter.groupIndex = groupIndex.index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = em.getComponent<RegularPolygon>(e)->radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, RegularPolygon regularPolygon, float x, float y)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<Angle>(e, 0.f);
     em.addComponent<BlockRotationCtrl>(e, static_cast<uint16_t>(30), static_cast<bool>(std::rand() % 2));
     em.addComponent<BlockRevolutionCtrl>(e, 0.5f, 0.1f, static_cast<uint16_t>(30), static_cast<bool>(std::rand() % 2), static_cast<float>(std::rand() % 360 / 180.f * M_PI));
     em.addComponent<HP>(e, static_cast<int16_t>(100), static_cast<int16_t>(100), tick);
     em.addComponent<Attack>(e, static_cast<int16_t>(2 * TPS));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_BLOCK));
     em.addComponent<RegularPolygon>(e, regularPolygon);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = {x, y};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     // shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_BLOCK;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET | CATEGORY_BORDER_WALL;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = regularPolygon.radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, uint32_t tick, ecs::Entity player)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<HP>(e, static_cast<int16_t>(100), static_cast<int16_t>(100), tick);
     em.addComponent<Attack>(e, static_cast<int16_t>(12 * TPS));
     em.addComponent<HealingOverTime>(e, static_cast<int16_t>(-2 * TPS), tick, std::numeric_limits<uint32_t>::max());
     em.addComponent<BulletAttackNum>(e, static_cast<uint8_t>(4));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "");
     em.addComponent<Type>(e, static_cast<uint8_t>(CATEGORY_BULLET));
     em.addComponent<GroupIndex>(e, em.getComponent<GroupIndex>(player)->index);
     em.addComponent<ecs::Entity>(e, player);
     em.addComponent<RegularPolygon>(e, static_cast<uint8_t>(64), 0.2f); //>=16为圆形

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.fixedRotation = true;
     bodyDef.position = b2Body_GetPosition(*em.getComponent<b2BodyId>(player));
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 5.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_BULLET;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET;
     shapeDef.filter.groupIndex = em.getComponent<GroupIndex>(player)->index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = em.getComponent<RegularPolygon>(e)->radius;

     shapeEntityMap[b2StoreShapeId(b2CreateCircleShape(bodyId, &shapeDef, &circle))] = e;
     auto *input = em.getComponent<Input>(player);
     b2Vec2 vel = (b2Vec2){input->x, input->y} - bodyDef.position;
     vel = b2Normalize(vel) * 10.f;
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
