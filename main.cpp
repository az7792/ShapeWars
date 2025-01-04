#include <iostream>
#include <thread>
#include "box2d/box2d.h"
#include "utils/Time.h"
#include "utils/ThreadPool.h"
using namespace std;
int main()
{
     ThreadPool t(16);
     for (int i = 1; i <= 10; ++i)
     {
          t.submit([i]()
                   { std::cout << i << endl; });
     }
     auto tim = Time::now();
     cout << tim.toString() << endl;
     this_thread::sleep_for(chrono::seconds(2));
     tim.update();
     cout << tim.toString();
     t.close();
     return 0;
}