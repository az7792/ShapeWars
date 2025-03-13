// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "ecs/EntitySet.h"
#include "config/config.h"
#include "ecs/fwd.h"
#include "box2d/box2d.h"
#include <iostream>
struct Camera
{
     constexpr static float width = (1920.0f + 100) / 50;
     constexpr static float height = (1080.0f + 100) / 50;
     float x;
     float y;
     float fov;

     std::vector<ecs::Entity> delEntities;    // 当前帧需要删除的实体(在视野中被销毁的实体)
     std::vector<ecs::Entity> removeEntities; // 当前帧需要移出的实体(移动到视野外的实体)
     ecs::EntitySet inEntities;               // 剩余的已经在摄像机范围内的实体，需要更新
     std::vector<ecs::Entity> createEntities; // 当前帧刚需要进入摄像机范围的实体，需要创建

     b2BodyId bodyId;

     Camera() = default;

     ~Camera() = default;

     Camera(float x_, float y_, float fov_)
         : x(x_), y(y_), fov(fov_), inEntities(1) {}

     // 为了隐式弃置移动赋值运算符，保证在插入或者替换组件时调用拷贝赋值，这样可以保留旧实体中动态容器已分配的空间
     Camera &operator=(const Camera &) = default;

     b2BodyId createSensor(b2WorldId worldId)
     {
          b2BodyDef bodyDef = b2DefaultBodyDef();
          bodyDef.type = b2_kinematicBody;
          bodyDef.fixedRotation = true;
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
