#pragma once
#include <functional>

template <typename Type, typename... Args>
class Signal
{
     std::vector<std::function<Type(Args...)>> slots_;

public:
     Signal() = default;
     ~Signal() = default;
     /// @brief 连接槽函数
     void connect(const std::function<Type(Args...)> &slot)
     {
          slots_.push_back(slot);
     }

     /// @brief 断开槽函数(未实现)
     void disconnect(const std::function<Type(Args...)> &slot)
     {
          // MAYBE
     }

     /// @brief 执行所有槽函数
     void execute(Args... args)
     {
          for (auto &slot : slots_)
          {
               slot(args...);
          }
     }

     /// @brief 执行所有槽函数
     void operator()(Args... args)
     {
          for (auto &slot : slots_)
          {
               slot(args...);
          }
     }
};