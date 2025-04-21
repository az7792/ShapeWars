// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include "ecs/EntityManager.h"
#include "box2d/box2d.h"
#include "game/factories.h"
#include "json/json.hpp"

class TankFactory
{
     ecs::EntityManager *em;
     b2WorldId *worldId;
     nlohmann::json tankdefs;

     TankFactory &operator=(const TankFactory &) = delete;
     TankFactory(const TankFactory &) = delete;
     TankFactory();
     ~TankFactory() = default;

     BarrelParams createBarrelParams(const nlohmann::json &barreldef, Attribute *attribute);

public:
     static TankFactory &instence()
     {
          static TankFactory instance;
          return instance;
     }

     nlohmann::json &getTankDefs();

     /**
      * @brief 初始化tank工厂
      * @param em 实体管理器
      * @param worldId 世界id
      * @param tankdefsPath tank定义的json文件路径
      */
     void init(ecs::EntityManager *em, b2WorldId *worldId, std::string tankdefsPath);

     /**
      * @brief 创建坦克实体
      * @param tick 时间戳
      * @param id 坦克id
      * @param params 玩家参数
      */
     ecs::Entity createTank(uint32_t tick, int id, PlayerParams &params);

     /**
      * @brief 切换坦克
      * @param e 实体
      * @param tick 时间戳
      * @param id 坦克id
      */
     void upgradeTank(ecs::Entity e, uint32_t tick, int id);
};