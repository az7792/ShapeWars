#pragma once
#include <vector>
#include <set>
#include "config/config.h"
#include "ecs/fwd.h"
#include "box2d/box2d.h"
struct Camera
{
     constexpr static float width = (1920.0f + 100) / 500;
     constexpr static float height = (1080.0f + 100) / 500;
     float x;
     float y;
     float fov;

     std::vector<ecs::Entity> delEntities;    // 当前帧需要删除的实体
     std::set<ecs::Entity> inEntities;        // 剩余的已经在摄像机范围内的实体，需要更新 TODO: 使用稀疏集优化
     std::vector<ecs::Entity> createEntities; // 当前帧刚需要进入摄像机范围的实体，需要创建

     b2BodyId bodyId;

     Camera() = default;

     Camera(float x_, float y_, float fov_)
         : x(x_), y(y_), fov(fov_) {}

     b2BodyId createSensor(b2WorldId worldId)
     {
          b2BodyDef bodyDef = b2DefaultBodyDef();
          bodyDef.type = b2_kinematicBody;
          bodyDef.position = {x, y};
          b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

          b2Polygon box = b2MakeBox(width / 2, height / 2);

          b2ShapeDef shapeDef = b2DefaultShapeDef();
          // shapeDef.filter.categoryBits = (uint64_t)MyCategories::CAMERA;
          // shapeDef.filter.maskBits = 0;
          shapeDef.isSensor = true;
          shapeDef.userData = static_cast<void *>(this);

          b2CreatePolygonShape(bodyId, &shapeDef, &box);
          return bodyId;
     }
};
