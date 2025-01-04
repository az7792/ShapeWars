#pragma once
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>

/**
 * @class Time
 * @brief 获取当前时间，并提供toString方法
 */
class Time
{
public:
     /// @brief 时间戳
     std::time_t timestamp;

     /// @brief `timestamp`对应的年月日时分秒
     int year, month, day, hour, minute, second;

     /// @brief 默认构造函数，初始化为当前时间。
     Time();

     /// @brief 返回包含当前时间的`Time`对象。
     static Time now();

     ///@brief 返回`YYYY-MM-DD HH:MM:SS`格式的字符串。
     std::string toString() const;

     ///@brief 更新 `Time` 对象的时间为当前时间。
     void update();
};