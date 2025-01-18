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

int main()
{
     ecs::EntityManager em;
     ecs::Entity e1 = em.createEntity();
     ecs::Entity e2 = em.createEntity();
     ecs::Entity e3 = em.createEntity();
     ecs::Entity e4 = 4;
     em.addComponent<Position>(e1, 1.0f, 2.0f);
     em.addComponent<Velocity>(e1, 3.0f, 4.0f);
     em.addComponent<Name>(e1, "e1");

     em.addComponent<Position>(e2, 5.0f, 6.0f);
     em.addComponent<Velocity>(e2, 7.0f, 8.0f);

     em.addComponent<Name>(e3, "e3");

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

     // 获取一个不存在的组件应该 failed.
     // name = em.getComponent<Name>(e2);

     return 0;
}