#include <iostream>
#include <thread>
#include "box2d/box2d.h"
#include "utils/Time.h"
#include "utils/ThreadPool.h"
#include "utils/Logger.h"
using namespace std;
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     ThreadPool t(2);
     for(int i=0;i<10;++i)
     {
          t.submit([&](){
               LOG_DEBUG(to_string(i));
          });
     }
     return 0;
}