#pragma once
#include "ecs/EntityManager.h"
#include "component/fwd.h"
#include "net/TcpConnection.h"

#include "box2d/box2d.h"
ecs::Entity createEntityTest(ecs::EntityManager &em, b2WorldId &worldId, TcpConnection *tcpConnection)
{
     ecs::Entity e = em.createEntity();
     em.addComponent<Position>(e);
     em.addComponent<Velocity>(e);
     em.addComponent<HP>(e, 100, 100, false);
     em.addComponent<Camera>(e, Camera(0.f, 0.f, 1.f));
     Camera *camera = em.getComponent<Camera>(e);
     camera->bodyId = camera->createSensor(worldId);

     em.addComponent<PackData>(e, "", false);
     em.addComponent<Input>(e, 0.f, 0.f, 0ull);
     em.addComponent<TcpConnection *>(e, tcpConnection);

     b2BodyDef bodyDef = b2DefaultBodyDef();
     bodyDef.type = b2_dynamicBody;
     bodyDef.position = {0.f, 0.f};
     bodyDef.userData = static_cast<void *>(em.getEntityPtr(e));
     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

     b2ShapeDef shapeDef = b2DefaultShapeDef();
     shapeDef.density = 1.f;   // 默认为1
     shapeDef.friction = 0.1f; // 动态物体需要设置密度和摩擦系数
     shapeDef.userData = bodyDef.userData;
     // shapeDef.filter.categoryBits = (uint64_t)MyCategories::PLAYER;
     // shapeDef.filter.maskBits = 0;

     // 圆形
     b2Circle circle;
     circle.center = b2Vec2_zero; // 这个坐标是相对bodyDef.position而言的偏移量
     circle.radius = 0.05f;

     b2CreateCircleShape(bodyId, &shapeDef, &circle);
     b2Body_SetLinearVelocity(bodyId, {0.001f, 0.001f});

     em.addComponent<b2BodyId>(e, bodyId);
     return e;
}
