#include <iostream>
#include "utils/Logger.h"
#include "box2d/box2d.h"
using namespace std;
// 测试内存泄漏：
// 用该命令运行程序： valgrind --leak-check=full ./server
int main()
{
     Logger::instance().setLevel(LogLevel::DEBUG);
     Logger::instance().setTimeFormat(TimeFormat::TimeOnly);
     return 0;
}