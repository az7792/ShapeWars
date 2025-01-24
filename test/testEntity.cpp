#include "ecs/EntityManager.h"
#include <bits/stdc++.h>
using namespace std;

struct Position
{
     float x, y;
};

struct Velocity
{
     float dx, dy;
};

struct Name
{
     string name;
};

struct Empty
{
};

int main()
{
     ecs::EntityManager em;

     auto &G = em.group<Name>(ecs::type_list<Position, Velocity>{});

     ecs::Entity e1 = em.createEntity();
     ecs::Entity e2 = em.createEntity();
     ecs::Entity e3 = em.createEntity();
     ecs::Entity e4 = 4;
     em.addComponent<Position>(e1, 1.0f, 2.0f);
     em.addComponent<Velocity>(e1, 3.0f, 4.0f);
     em.addComponent<Name>(e1, "e1");
     for (auto &entity : G)
     {
          cout << "NameEntity: " << entity << endl;
     }

     em.addComponent<Position>(e2, 5.0f, 6.0f);
     em.addComponent<Velocity>(e2, 7.0f, 8.0f);
     em.addComponent<Name>(e2, "e2");

     em.addComponent<Velocity>(e3, 33.0f, 4.0f);
     em.addComponent<Name>(e3, "e3");
     cout<<"--------------\n";
     for (auto &entity : G)
     {
          cout << "NameEntity: " << entity << endl;
     }

     assert(ecs::ComponentPools::get<Position>().size() == 2);
     assert(ecs::ComponentPools::get<Velocity>().size() == 3);
     assert(ecs::ComponentPools::get<Name>().size() == 3);

     // 测试是否添加成功
     assert(em.hasComponent<Position>(e1));
     assert(em.hasComponent<Velocity>(e1));
     assert(em.hasComponent<Name>(e1));
     bool result = em.hasComponents<Position, Velocity, Name>(e1);
     assert(result);

     assert(em.hasComponent<Position>(e2));
     assert(em.hasComponent<Velocity>(e2));

     assert(em.hasComponent<Name>(e3));

     // 测试是否移除成功
     em.removeComponent<Position>(e1);
     em.removeComponent<Velocity>(e1);

     result = em.hasComponents<Position, Velocity, Name>(e1);
     assert(!result);
     assert(!em.hasComponent<Position>(e1));
     assert(!em.hasComponent<Velocity>(e1));

     // 测试获取组件
     Name *name = em.getComponent<Name>(e1);
     assert(name->name == "e1");

     // 测试替换组件
     em.replaceComponent<Name>(e1, "e1_new");
     name = em.getComponent<Name>(e1);
     assert(name->name == "e1_new");

     // 测试实体是否有效
     assert(em.entityIsValid(e1));
     assert(em.entityIsValid(e2));
     assert(em.entityIsValid(e3));
     assert(!em.entityIsValid(e4));

     // 测试视图
     em.addComponent<Position>(e1, 9.0f, 10.0f);
     em.addComponent<Velocity>(e1, 11.0f, 12.0f);
     auto view = em.getView<Position, Velocity>();
     for (auto entity : view)
     {
          Position *pos = em.getComponent<Position>(entity);
          Velocity *vel = em.getComponent<Velocity>(entity);
          cout << "Entity: " << entity << " Position: (" << pos->x << ", " << pos->y << ") Velocity: (" << vel->dx << ", " << vel->dy << ")" << endl;
     }

     auto view2 = em.getView<Name>();
     for (auto entity : view2)
     {
          Name *name = em.getComponent<Name>(entity);
          cout << "Entity: " << entity << " Name: " << name->name << endl;
     }

     em.destroyEntity(e1);
     assert(!em.entityIsValid(e1));
     cout << "--------------\n";
     view2 = em.getView<Name>();
     for (auto entity : view2)
     {
          Name *name = em.getComponent<Name>(entity);
          cout << "Entity: " << entity << " Name: " << name->name << endl;
     }
     em.destroyEntity(e2);

     e1 = em.createEntity();
     e2 = em.createEntity();

     cout << ecs::entityToVersion(e1) << " " << ecs::entityToIndex(e1) << endl;
     cout << ecs::entityToVersion(e2) << " " << ecs::entityToIndex(e2) << endl;

     cout << "----------------\n";
     em.addComponent<Empty>(e1);
     em.addComponent<Empty>(e2);
     em.addComponent<Name>(e2, "e2_new");

     view2 = em.getView<Empty>();
     for (auto entity : view2)
     {
          cout << ecs::entityToIndex(entity) << endl;
     }
     cout << "-----------------\n";
     em.destroyEntity(e1);
     view2 = em.getView<Empty>();
     for (auto entity : view2)
     {
          cout << em.getComponent<Name>(entity)->name << endl;
     }
     cout << "-----------------\n";
     em.destroyEntity(e2);
     em.destroyEntity(e3);

     assert(em.getEntityNum() == 0);

     return 0;
}