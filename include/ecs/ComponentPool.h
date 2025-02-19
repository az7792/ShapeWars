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
#include "utils/SparseMap.h"
#include "ComponentTypeID.h"
// TODO: 空类型优化
/*
   TODO：优化std::array,array会把一整页的组件全部初始化，如果该组件有动态数据且默认分配很大空间会导致内存浪费
     例如成员变量有vector<int>,并且默认构造会把vector分配一大块内存，导致浪费
     struct A{
         std::vector<int> vec;
         A():vec(1000){}
     };
*/
namespace ecs
{

     class ComponentPoolBase
     {
     public:
          /// @brief 尝试删除一个实体(使实体失去该类型组件)
          virtual bool try_erase(ecs::Entity entity) = 0;
     };

     template <typename Component>
     class ComponentPool : public ComponentPoolBase
     {
     private:
          template <typename, typename>
          friend class Group;

          constexpr static const uint32_t nullValue_ = 0xffffffff;                                      // 标记空稀疏数组的空值
          std::vector<std::unique_ptr<std::array<Component, ECS_COMPONENT_PAGE_SIZE>>> ComponentPages_; // 组件页数组
          std::vector<Component *> ptrs_;                                                               // 指针数组，存放实体的组件指针
          std::vector<ecs::Entity> dense_;                                                              // 稠密数组,存放实体
          std::vector<uint32_t> sparse_;                                                                // 稀疏数组，存放索引

          size_t size_; // 稠密数组可用部分的大小

          size_t pageIndex_; // 空闲位置在最好一页的索引(并不是指第几页)

          /// @brief 交换两个稠密数组的位置
          void swap(uint32_t i1, uint32_t i2)
          {
               assert(has(ecs::entityToIndex(dense_[i1])) && has(ecs::entityToIndex(dense_[i2])));
               std::swap(dense_[i1], dense_[i2]);
               std::swap(ptrs_[i1], ptrs_[i2]);
               std::swap(sparse_[ecs::entityToIndex(dense_[i1])], sparse_[ecs::entityToIndex(dense_[i2])]);
          }

     public:
          ComponentPool(size_t size = 10) : sparse_(size, nullValue_), size_(0), pageIndex_(ECS_COMPONENT_PAGE_SIZE) {};
          ~ComponentPool() = default;

          /// @brief 获取稠密数组
          std::vector<ecs::Entity> &getDense() { return dense_; }

          /// @brief 插入一个实体(使实体拥有该类型组件)
          /// @note: 如果需要保留旧实体里动态数据已经分配的空间，需要隐式弃置移动赋值运算符
          template <typename... Args>
          Component *insert(ecs::Entity entity, Args &&...args)
          {
               uint32_t index = ecs::entityToIndex(entity);
               if (index >= sparse_.size())
               {
                    sparse_.resize(std::max((size_t)index + 1, sparse_.size() * 2), nullValue_);
               }
               if (sparse_[index] == nullValue_)
               {
                    if (size_ == dense_.size()) // 不可重用指针
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
                         // 如果需要保留旧实体里动态数据已经分配的空间，需要隐式弃置移动赋值运算符
                         (*ComponentPages_.back())[pageIndex_] = Component{std::forward<Args>(args)...};
                         ptrs_.back() = &(*ComponentPages_.back())[pageIndex_];
                         pageIndex_++;
                    }
                    else // 可重用指针
                    {
                         dense_[size_] = entity;
                         // 如果需要保留旧实体里动态数据已经分配的空间，需要隐式弃置移动赋值运算符
                         *ptrs_[size_] = Component{std::forward<Args>(args)...};
                         sparse_[index] = size_;
                         size_++;
                    }
               } // MAYBE: 替换组件?
               return ptrs_[sparse_[index]];
          }

          /// @brief 替换一个实体的组件
          /// @note: 如果需要保留旧实体里动态数据已经分配的空间，需要隐式弃置移动赋值运算符
          template <typename... Args>
          Component *replace(ecs::Entity entity, Args &&...args)
          {
               assert(has(entity));
               uint32_t index = ecs::entityToIndex(entity);
               // 如果需要保留旧实体里动态数据已经分配的空间，需要隐式弃置移动赋值运算符
               *ptrs_[sparse_[index]] = Component{std::forward<Args>(args)...};
               return ptrs_[sparse_[index]];
          }

          /// @brief 删除一个实体(使实体失去该类型组件)
          void erase(ecs::Entity entity)
          {
               //不会真的删除
               assert(has(entity));
               uint32_t index = ecs::entityToIndex(entity);
               size_--;
               swap(sparse_[index], size_);
               sparse_[index] = nullValue_;
          }

          /// @brief 尝试删除一个实体(使实体失去该类型组件)
          bool try_erase(ecs::Entity entity) override
          {
               if (has(entity))
               {
                    erase(entity);
                    return true;
               }
               return false;
          }

          /// @brief 判断实体是否拥有该类型组件
          bool has(ecs::Entity entity) const { return (ecs::entityToIndex(entity) < sparse_.size() && sparse_[ecs::entityToIndex(entity)] != nullValue_); }

          /// @brief 获取实体的组件指针
          Component *get(ecs::Entity entity)
          {
               assert(has(entity));
               return ptrs_[sparse_[ecs::entityToIndex(entity)]];
          }

          size_t size() const { return size_; }
     };

     namespace ComponentPools
     {
          inline SparseMap<ComponentPoolBase *> componentPools; // 组件ID与组件池的映射,通过EntityManager管理

          template <typename Component>
          inline bool firstInit(ComponentPool<Component> &pool)
          {
               componentPools.insert(ComponentTypeID::getID<Component>(), static_cast<ComponentPoolBase *>(&pool));
               return true;
          }

          /// 获取组件池
          template <typename Component>
          inline ComponentPool<Component> &get()
          {
               static ComponentPool<Component> pool;
               static bool firstInit_ = firstInit<Component>(pool);
               (void)firstInit_; // 防止未使用警告
               return pool;
          }
     };
} // namespace ecs