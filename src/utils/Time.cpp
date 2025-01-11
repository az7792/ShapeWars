#include "utils/Time.h"

Time::Time() : timestamp(std::time(nullptr)), year(0), month(0), day(0),
               hour(0), minute(0), second(0)
{
     update();
}

Time Time::now()
{
     return Time();
}

std::string Time::toDateTimeString() const
{
     std::ostringstream oss;
     // 输出格式例如 : 2025-01-04 12:17:19
     oss << std::setw(4) << std::setfill('0') << year << "-"
         << std::setw(2) << std::setfill('0') << month << "-"
         << std::setw(2) << std::setfill('0') << day << " "
         << std::setw(2) << std::setfill('0') << hour << ":"
         << std::setw(2) << std::setfill('0') << minute << ":"
         << std::setw(2) << std::setfill('0') << second;
     return oss.str();
}

std::string Time::toTimeString() const
{
     std::ostringstream oss;
     // 输出格式例如 : 12:17:19
     oss << std::setw(2) << std::setfill('0') << hour << ":"
         << std::setw(2) << std::setfill('0') << minute << ":"
         << std::setw(2) << std::setfill('0') << second;
     return oss.str();
}

void Time::update()
{
     timestamp = std::time(nullptr);

     std::tm localTime;
     localtime_r(&timestamp, &localTime);

     year = (localTime.tm_year + 1900);
     month = (localTime.tm_mon + 1);
     day = localTime.tm_mday;
     hour = localTime.tm_hour;
     minute = localTime.tm_min;
     second = localTime.tm_sec;
}