#pragma once
#include <vector>
#include <cstdint> // for uint32_t
#include <cstddef> // for size_t
#include <assert.h>
#include <memory>
#include <array>
#include "config/config.h"
#include "fwd.h"
#include "utils.h"
namespace ecs
{
     template <typename Component>
     class ComponentPool
     {
     private:
          constexpr static const uint32_t nullValue_ = 0xffffffff;                                      // 标记空稀疏数组的空值
          std::vector<std::unique_ptr<std::array<Component, ECS_COMPONENT_PAGE_SIZE>>> ComponentPages_; // 组件页数组
          std::vector<Component *> ptrs_;                                                               // 指针数组，存放实体的组件指针
          std::vector<ecs::Entity> dense_;                                                              // 稠密数组,存放实体
          std::vector<uint32_t> sparse_;                                                                // 稀疏数组，存放索引

          size_t size_; // 稠密数组可用部分的大小

          size_t pageIndex_; // 空闲位置在最好一页的索引(并不是指第几页)

     public:
          ComponentPool(size_t size = 100) : sparse_(size, nullValue_), size_(0), pageIndex_(ECS_COMPONENT_PAGE_SIZE) {};
          ~ComponentPool() = default;

          /// @brief 获取稠密数组
          std::vector<ecs::Entity> &getDense() { return dense_; }

          /// @brief 插入一个实体(使实体拥有该类型组件)
          template <typename... Args>
          void insert(ecs::Entity entity, Args &&...args)
          {
               uint32_t index = ecs::entityToIndex(entity);
               if (index >= sparse_.size())
               {
                    sparse_.resize(std::max((size_t)index + 1, sparse_.size() * 2), nullValue_);
               }
               if (sparse_[index] == nullValue_)
               {
                    if (size_ == dense_.size()) // 不可重用
                    {
                         dense_.push_back(entity);
                         ptrs_.push_back(nullptr);
                         sparse_[index] = dense_.size() - 1;
                         size_ = dense_.size();

                         if (pageIndex_ == ECS_COMPONENT_PAGE_SIZE) // 组件页满了,需要先创建新页
                         {
                              ComponentPages_.emplace_back(std::make_unique<std::array<Component, ECS_COMPONENT_PAGE_SIZE>>());
                              pageIndex_ = 0;
                         }
                         new (&(*ComponentPages_.back())[pageIndex_]) Component{std::forward<Args>(args)...};
                         ptrs_.back() = &(*ComponentPages_.back())[pageIndex_];
                         pageIndex_++;
                    }
                    else // 可重用
                    {
                         dense_[size_] = entity;
                         *ptrs_[size_] = Component{std::forward<Args>(args)...};
                         sparse_[index] = size_;
                         size_++;
                    }
               } // TODO: 替换组件?
          }

          /// @brief 替换一个实体的组件
          template <typename... Args>
          void replace(ecs::Entity entity, Args &&...args)
          {
               assert(has(entity));
               uint32_t index = ecs::entityToIndex(entity);
               *ptrs_[sparse_[index]] = Component{std::forward<Args>(args)...};
          }

          /// @brief 删除一个实体(使实体失去该类型组件)
          void erase(ecs::Entity entity)
          {
               assert(has(entity));
               uint32_t index = ecs::entityToIndex(entity);
               swap(index, size_ - 1);
               size_--;
               sparse_[index] = nullValue_;
          }

          /// @brief 尝试删除一个实体(使实体失去该类型组件)
          void try_erase(ecs::Entity entity)
          {
               if (has(entity))
               {
                    erase(entity);
               }
          }

          /// @brief 判断实体是否拥有该类型组件
          bool has(ecs::Entity entity) const { return (ecs::entityToIndex(entity) < sparse_.size() && sparse_[ecs::entityToIndex(entity)] != nullValue_); }

          /// @brief 交换两个数的位置
          void swap(ecs::Entity e1, ecs::Entity e2)
          {
               assert(has(e1) && has(e2));
               uint32_t index1 = ecs::entityToIndex(e1);
               uint32_t index2 = ecs::entityToIndex(e2);
               std::swap(dense_[sparse_[index1]], dense_[sparse_[index2]]);
               std::swap(ptrs_[sparse_[index1]], ptrs_[sparse_[index2]]);
               std::swap(sparse_[index1], sparse_[index2]);
          }

          /// @brief 获取实体的组件指针
          Component *get(ecs::Entity entity)
          {
               assert(has(entity));
               return ptrs_[sparse_[ecs::entityToIndex(entity)]];
          }

          size_t size() const { return size_; }
     };
} // namespace ecs