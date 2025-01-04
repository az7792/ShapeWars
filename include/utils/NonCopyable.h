/// @brief 派生类无法进行拷贝构造和赋值构造
class NonCopyable
{
public:
     NonCopyable() = default;  // 默认构造函数
     ~NonCopyable() = default; // 默认析构函数

     // 删除拷贝构造函数
     NonCopyable(const NonCopyable &) = delete;

     // 删除拷贝赋值运算符
     NonCopyable &operator=(const NonCopyable &) = delete;

     // 可以定义移动构造函数和移动赋值运算符
     NonCopyable(NonCopyable &&) noexcept = default;
     NonCopyable &operator=(NonCopyable &&) noexcept = default;
};