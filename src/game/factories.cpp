#include "game/factories.h"
#include "config/config.h"

ecs::Entity createEntityPlayer(ecs::EntityManager &em, b2WorldId &worldId, TcpConnection *tcpConnection, GroupIndex groupIndex)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<HP>(e, static_cast<int16_t>(100), static_cast<int16_t>(100), true);
     em.addComponent<Attack>(e, static_cast<int16_t>(2 * TPS));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "", false, false);
     em.addComponent<Input>(e, 0.f, 0.f, 0ull);
     em.addComponent<TcpConnection *>(e, tcpConnection);
     em.addComponent<Type>(e, static_cast<uint8_t>(__builtin_ctz(CATEGORY_PLAYER)));
     em.addComponent<GroupIndex>(e, groupIndex);
     em.addComponent<Camera>(e, Camera(0.f, 0.f, 1.f));
     Camera *camera = em.getComponent<Camera>(e);
     camera->bodyId = camera->createSensor(worldId);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.position = {0.f, 0.f};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_PLAYER;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET;
     shapeDef.filter.groupIndex = groupIndex.index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = 0.05f;

     b2CreateCircleShape(bodyId, &shapeDef, &circle);

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBlock(ecs::EntityManager &em, b2WorldId &worldId, RegularPolygon regularPolygon, float x, float y)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<HP>(e, static_cast<int16_t>(100), static_cast<int16_t>(100), true);
     em.addComponent<Attack>(e, static_cast<int16_t>(2 * TPS));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "", false, false);
     em.addComponent<Type>(e, static_cast<uint8_t>(__builtin_ctz(CATEGORY_BLOCK)));
     em.addComponent<RegularPolygon>(e, regularPolygon);

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.position = {x, y};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_BLOCK;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = regularPolygon.radius;

     b2CreateCircleShape(bodyId, &shapeDef, &circle);

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}

ecs::Entity createEntityBullet(ecs::EntityManager &em, b2WorldId &worldId, ecs::Entity player)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<HP>(e, static_cast<int16_t>(100), static_cast<int16_t>(100), true);
     em.addComponent<Attack>(e, static_cast<int16_t>(2 * TPS));
     em.addComponent<ContactList>(e);
     em.addComponent<PackData>(e, "", "", false, false);
     em.addComponent<Type>(e, static_cast<uint8_t>(__builtin_ctz(CATEGORY_BULLET)));
     em.addComponent<ecs::Entity>(e, player);
     em.addComponent<RegularPolygon>(e, static_cast<uint8_t>(64), 0.01f); //>=16为圆形

     // 定义刚体
     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.position = {0.f, 0.f};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     shapeDef.enableContactEvents = true;
     shapeDef.filter.categoryBits = CATEGORY_BULLET;
     shapeDef.filter.maskBits = CATEGORY_BLOCK | CATEGORY_PLAYER | CATEGORY_BULLET;
     shapeDef.filter.groupIndex = em.getComponent<GroupIndex>(player)->index;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = 0.01f;

     b2CreateCircleShape(bodyId, &shapeDef, &circle);

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}
