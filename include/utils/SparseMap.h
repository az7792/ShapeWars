#pragma once
#include <vector>
#include <utility>   // for std::pair
#include <cstdint>   // for uint32_t
#include <cstddef>   // for size_t
#include <stdexcept> // for std::out_of_range

/**
 * @brief 用于管理键值对的稀疏集合，支持高效插入、删除和查找
 * @tparam ValueType 值的类型
 */
template <typename ValueType>
class SparseMap
{
private:
     using PairType = std::pair<uint32_t, ValueType>;

     const uint32_t nullValue = 0xffffffff; // 标记空位置
     std::vector<PairType> dense_;          // 稠密数组存储键值对
     std::vector<uint32_t> sparse_;         // 稀疏数组，存索引键值对的位置

public:
     SparseMap(size_t size = 1000) : sparse_(size, nullValue) {}
     ~SparseMap() = default;

     /// @brief 插入键值对,已经存在的键会被覆盖
     /// @param key 键
     /// @param value 值
     /// @exception 如果key >= size，会抛出异常
     void insert(uint32_t key, const ValueType &value)
     {
          if (key >= sparse_.size())
          {
               sparse_.resize(std::max((size_t)key + 1, sparse_.size() * 2), nullValue);
          }
          if (sparse_[key] == nullValue)
          {
               dense_.emplace_back(key, value);
               sparse_[key] = dense_.size() - 1;
          }
          else
          {
               dense_[sparse_[key]].second = value;
          }
     }

     /// @brief 删除键值对
     /// @param key 键
     void erase(uint32_t key)
     {
          if (key < sparse_.size() && sparse_[key] != nullValue)
          {
               sparse_[dense_.back().first] = sparse_[key];
               std::swap(dense_[sparse_[key]], dense_.back());
               dense_.pop_back();
               sparse_[key] = nullValue;
          }
     }

     /// @brief 查找键是否存在
     bool find(uint32_t key) const
     {
          if (key >= sparse_.size())
          {
               return false;
          }
          return sparse_[key] != nullValue;
     }

     /// @brief 通过键访问值（可修改）
     /// @param key 键
     /// @return 如果键存在，返回对应的值
     /// @exception 如果键不存在,会抛出异常
     ValueType &operator[](uint32_t key)
     {
          if (key >= sparse_.size())
          {
               throw std::out_of_range("SparseSet::operator[]: value超出范围");
          }
          return dense_[sparse_[key]].second;
     }

     /// @brief 通过键访问值（只读）
     /// @param key 键
     /// @return 如果键存在，返回对应的值
     /// @exception 如果键不存在,会抛出异常
     const ValueType &operator[](uint32_t key) const
     {
          if (key >= sparse_.size())
          {
               throw std::out_of_range("SparseSet::operator[]: value超出范围");
          }
          return dense_[sparse_[key]].second;
     }

     /// begin
     auto begin() const { return dense_.begin(); }
     /// end
     auto end() const { return dense_.end(); }
};
