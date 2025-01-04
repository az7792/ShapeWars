#include <iostream>
#include <thread>
#include "box2d/box2d.h"
#include "utils/Time.h"
using namespace std;
int main()
{
     auto tim = Time::now();
     cout << tim.toString() << endl;
     this_thread::sleep_for(chrono::seconds(2));
     tim.update();
     cout << tim.toString();
     return 0;
}