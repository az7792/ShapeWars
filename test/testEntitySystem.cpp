// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

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
          // cout << em.addComponent<Name>(entity)->name << ":pos:" << pos->x << " " << pos->y << endl;
          // cout << em.addComponent<Name>(entity)->name << ":vel:" << vel->dx << " " << vel->dy << endl;
          // cout << endl;
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
     em.addComponent<Velocity>(e1, 3.0f, 1.0f);
     em.addComponent<Name>(e1, "e1");

     em.addComponent<Position>(e2, 1.0f, 1.0f);
     em.addComponent<Velocity>(e2, 2.0f, 2.0f);
     em.addComponent<Name>(e2, "e2");

     auto time1 = chrono::high_resolution_clock::now();
     for (int i = 0; i < 1000000; ++i)
     {
           em.updateSystems();
     }
     auto time2 = chrono::high_resolution_clock::now();
     auto duration = chrono::duration_cast<chrono::microseconds>(time2 - time1).count();
     cout << "Time taken: " << duration << " microseconds" << endl;

     return 0;
}