#pragma once
#include <vector>
#include <cstdint> // for uint32_t
#include <cstddef> // for size_t

/**
 * @brief 用于管理非负整数(uint32_t)的集合
 * 对外表现出哈希表的特性，但是更快
 * 允许存储的范围[0,size)
 * @warning size不能太大，否则会导致内存占用过大
 */
class SparseSet
{
private:
     const uint32_t nullValue = 0xffffffff; //[0] 不要交互顺序
     std::vector<uint32_t> dense_, sparse_; //[1]

public:
     SparseSet(size_t size = 1000);
     ~SparseSet() = default;

     /// @brief 插入一个数
     /// @exception 如果valu>=size，会抛出异常
     void insert(uint32_t v);

     /// @brief 去除一个数
     void erase(uint32_t v);

     /// @brief 判断是否存在某个数
     bool find(uint32_t v) const;

     /// @brief 获取用于遍历的dense数组
     const std::vector<uint32_t> &getDense() const;
};
