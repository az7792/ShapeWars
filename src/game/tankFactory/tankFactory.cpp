#include "game/tankFactory/tankFactory.h"
#include "fstream"
#include "utils/Logger.h"
using json = nlohmann::json;

TankFactory::TankFactory() : em(nullptr), worldId(nullptr), tankdefs()
{
}

BarrelParams TankFactory::createBarrelParams(const nlohmann::json &barreldef, Attribute *attribute)
{
     BarrelParams params;
     params.barrel.widthL = barreldef["widthL"];
     params.barrel.widthR = barreldef["widthR"];
     params.barrel.length = params.barrel.nowLength = barreldef["length"];
     params.barrel.offsetAngle = barreldef["offsetAngle"];
     params.barrel.offsetY = barreldef["offsetY"];
     params.barrel.delay = barreldef["delay"];
     params.barrel.cooldown = barreldef["cooldown"].get<int>() - attribute->attr[7];

     params.bulletParams.attack = (2 + 3 * attribute->attr[3]) * barreldef["bullet"]["damage_m"].get<int>();
     params.bulletParams.lifetime = (40 + 2 * attribute->attr[4]) * barreldef["bullet"]["lifetime_m"].get<int>();
     params.bulletParams.initialHP = params.bulletParams.maxHP = (10 + 2 * attribute->attr[5]) * barreldef["bullet"]["maxHP_m"].get<int>();
     params.bulletParams.speed = (10 + 0.8 * attribute->attr[6]) * barreldef["bullet"]["speed_m"].get<float>();
     params.bulletParams.density = barreldef["bullet"]["density"];
     params.bulletParams.radius = barreldef["bullet"]["radius"];

     return params;
}

nlohmann::json &TankFactory::getTankDefs()
{
     return tankdefs;
}

void TankFactory::init(ecs::EntityManager *em, b2WorldId *worldId, std::string tankdefsPath)
{
     this->em = em;
     this->worldId = worldId;
     std::ifstream file(tankdefsPath);
     if (!file.is_open())
     {
          LOG_FATAL("坦克配置文件打开失败: " + tankdefsPath);
          return;
     }
     tankdefs = json::parse(file);
     file.close();
}

ecs::Entity TankFactory::createTank(uint32_t tick, int id, PlayerParams &params)
{
     assert(em != nullptr && worldId != nullptr);
     assert(params.tcpConnection != nullptr);

     auto &tankdef = tankdefs[id];

     params.initialHP = params.maxHP = 100 * tankdef["maxHP_m"].get<int>();
     params.attack = 1 * tankdef["damage_m"].get<int>();
     params.maxSpeed = tankdef["maxSpeed"];
     params.polygonRadius = tankdef["radius"];
     params.tankID = tankdef["id"];
     ecs::Entity e = createEntityPlayer(*em, *worldId, tick, params);

     for (auto &barreldef : tankdef["barrels"])
     {
          auto barrelParams = createBarrelParams(barreldef,em->getComponent<Attribute>(e));
          barrelParams.parentEntity = barrelParams.bulletParams.parentEntity = e;
          createEntityBarrel(*em, barrelParams);
     }

     return e;
}

void TankFactory::upgradeTank(ecs::Entity e, uint32_t tick, int id)
{
     auto &tankdef = tankdefs[id];
     Attribute *attribute = em->getComponent<Attribute>(e);

     // 更新属性
     auto hp = em->getComponent<HP>(e);
     hp->maxHP = hp->hp = (100 + 50 * attribute->attr[1]) * tankdef["maxHP_m"].get<int>();
     hp->tick = tick;
     em->addComponent<Attack>(e)->damage = (1 + 1 * attribute->attr[2]) * tankdef["damage_m"].get<int>();
     em->addComponent<Velocity>(e)->maxSpeed = tankdef["maxSpeed"].get<float>() + 0.5f * attribute->attr[8];
     em->addComponent<RegularPolygon>(e)->radius = tankdef["radius"];
     em->addComponent<TankID>(e)->id = tankdef["id"];
     em->addComponent<TankID>(e)->tick = tick;

     // 更新球体大小
     std::vector<b2ShapeId> shapes;
     shapes.resize(b2Body_GetShapeCount(*em->getComponent<b2BodyId>(e)));
     b2Body_GetShapes(*em->getComponent<b2BodyId>(e), shapes.data(), shapes.size());

     b2Circle circle = {b2Vec2_zero, tankdef["radius"]};
     for (auto &shapeId : shapes)
     {
          b2Shape_SetCircle(shapeId, &circle);
     }

     // 更新炮管
     auto ch = em->getComponent<Children>(e);
     for (auto child : ch->children)
     {
          em->destroyEntity(child);
     }
     ch->children.clear();
     for (auto &barreldef : tankdef["barrels"])
     {
          auto barrelParams = createBarrelParams(barreldef,attribute);
          barrelParams.parentEntity = barrelParams.bulletParams.parentEntity = e;
          createEntityBarrel(*em, barrelParams);
     }
}