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

void moveSystem(ecs::EntityManager &em, float dt)
{
     auto &G = em.group<Position, Velocity>();
     for (auto entity : G)
     {
          Position *pos = em.getComponent<Position>(entity);
          Velocity *vel = em.getComponent<Velocity>(entity);
          pos->x += vel->dx * dt;
          pos->y += vel->dy * dt;
          vel->dx *= 0.9f;
          vel->dy *= 0.9f;
          cout << "pos:" << pos->x << " " << pos->y << endl;
          cout << "vel:" << vel->dx << " " << vel->dy << endl;
          cout << endl;
     }
}

int main()
{
     ecs::EntityManager em;
     em.addSystem(std::bind(moveSystem, std::ref(em), 0.1f));

     ecs::Entity e1 = em.createEntity();
     ecs::Entity e2 = em.createEntity();
     ecs::Entity e3 = em.createEntity();
     em.addComponent<Position>(e1, 0.0f, 0.0f);
     em.addComponent<Velocity>(e1, 3.0f, 0.0f);
     em.addComponent<Name>(e1, "e1");

     for (int i = 0; i < 10; ++i)
     {
          em.updateSystems();
          this_thread::sleep_for(chrono::milliseconds(500));
     }

     return 0;
}