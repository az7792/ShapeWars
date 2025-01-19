#include <bits/stdc++.h>
using namespace std;

// namespace ecs
// {
//      /**
//       * @brief 一个用于处理类型列表的类，仅此而已。
//       * @tparam Type 提供给类型列表的类型。
//       */
//      template <typename... Type>
//      struct type_list
//      {
//           /*! @brief 类型列表类型。 */
//           using type = type_list;
//           /*! @brief 类型列表中元素的编译时常量数量。 */
//           static constexpr auto size = sizeof...(Type);
//      };

//      /**
//       * @brief 别名用于观察到的元素列表。
//       * @tparam Type 类型列表。
//       */
//      template <typename... Type>
//      struct get_t final : type_list<Type...>
//      {
//           /*! @brief 默认构造函数。 */
//           explicit constexpr get_t() = default;
//      };

//      /**
//       * @brief 观察到的元素列表的变量模板。
//       * @tparam Type 类型列表。
//       */
//      template <typename... Type>
//      inline constexpr get_t<Type...> get{};
// }

// template <typename Derived, typename T>
// class Base
// {
// protected:
//      T m_value;
//      int tmpValue;

// public:
//      Base() : m_value(0), tmpValue(10)
//      {
//           std::cout << "Type name: " << typeid(Derived).name() << std::endl;
//           cout << "base 构造" << endl;
//      }
//      ~Base()
//      {
//           std::cout << "Type name: " << typeid(Derived).name() << std::endl;
//           cout << "base 析构" << endl;
//      }
//      void addAndPrint()
//      {
//           static_cast<Derived *>(this)->addAndPrintImpl();
//      }

//      template <typename tt>
//      void Printf(tt v) const
//      {
//           static_cast<const Derived *>(this)->template PrintfImpl<tt>(v);
//      }

//      int getTmpValue() const
//      {
//           return tmpValue;
//      }
// };

// template <typename T>
// class A final : public Base<A<T>, T>
// {
//      using base = Base<A<T>, T>;
//      friend base;
//      void addAndPrintImpl()
//      {
//           this->m_value += 1;
//           this->tmpValue += 10;
//           cout << "m_value = " << this->m_value << endl;
//           cout << "tmpValue = " << this->tmpValue << endl;
//      }

//      template <typename tt>
//      void PrintfImpl(tt v) const
//      {
//           cout << "A::PrintfImpl<" << typeid(tt).name() << ">: " << v << endl;
//      }
// };

// template <typename T>
// class B final : public Base<B<T>, T>
// {
//      using base = Base<B<T>, T>;
//      friend base;
//      void addAndPrintImpl()
//      {
//           this->m_value += 2;
//           this->tmpValue += 20;
//           cout << "m_value = " << this->m_value << endl;
//           cout << "tmpValue = " << this->tmpValue << endl;
//      }

//      template <typename tt>
//      void PrintfImpl(tt v) const
//      {
//           cout << "B::PrintfImpl<" << typeid(tt).name() << ">: " << v << endl;
//      }
// };

class Base
{
public:
     virtual void P() = 0;
     virtual ~Base() { cout << "Base 析构" << endl; };
};

template <typename T>
class A : public Base
{
     T m_value;

public:
     A() : m_value(10) {}
     ~A() { cout << "A 析构" << endl; };
     void P() override
     {
          cout << m_value << endl;
     }
};

int main()
{
     A<int> * a = new A<int>();
     auto b = static_cast<Base *>(a);
     b->P();
     delete b;

     // A<double> *a = new A<double>();
     // B<int> b;
     // a->addAndPrint(); // m_value = 1
     // b.addAndPrint();  // m_value = 2
     // cout << "tmpValue = " << a->getTmpValue() << endl;
     // cout << "tmpValue = " << b.getTmpValue() << endl;
     // a->Printf<double>(10.0); // A::PrintfImpl<double>: 10
     // a->Printf<int>(10);      // A::PrintfImpl<int>: 10
     // b.Printf<double>(20.0);  // B::PrintfImpl<double>: 20
     // b.Printf<int>(20);       // B::PrintfImpl<int>: 20
     // delete a;

     // // 创建一个类型列表，包含 int, double, char 类型
     // using my_list = ecs::type_list<int, double, char>;

     // // 获取类型列表的大小
     // cout << "Size of type_list: " << my_list::size << endl; // 输出：3

     // // 使用 get_t 来创建一个类型列表的实例
     // ecs::get_t<int, double, char> my_get_t_instance;

     // // 获取 get_t 实例的类型列表大小
     // cout << "Size of get_t instance: " << decltype(my_get_t_instance)::size << endl; // 输出：3

     // // 使用变量模板 get 来访问类型列表
     // auto my_get = ecs::get<int, double, char, float>;
     // cout << my_get.size << endl; // 输出：3
     // // cout << "Size of get variable template: " << ecs::get<int, double, char>::size << endl; // 输出：3

     return 0;
}
// #pragma once
// #include <vector>
// #include <cstdint> // for uint32_t
// #include <cstddef> // for size_t
// #include <assert.h>
// #include <memory>
// #include <array>
// #include "config/config.h"
// #include "fwd.h"
// #include "utils.h"
// namespace ecs
// {
//      template <typename Derived, typename Component>
//      class ComponentPoolBase
//      {
//      protected:
//           constexpr static const uint32_t nullValue_ = 0xffffffff;                                      // 标记空稀疏数组的空值
//           std::vector<std::unique_ptr<std::array<Component, ECS_COMPONENT_PAGE_SIZE>>> ComponentPages_; // 组件页数组
//           std::vector<Component *> ptrs_;                                                               // 指针数组，存放实体的组件指针
//           std::vector<ecs::Entity> dense_;                                                              // 稠密数组,存放实体
//           std::vector<uint32_t> sparse_;                                                                // 稀疏数组，存放索引

//           size_t size_; // 稠密数组可用部分的大小

//           size_t pageIndex_; // 空闲位置在最好一页的索引(并不是指第几页)

//      public:
//           ComponentPoolBase(size_t size = 100) : sparse_(size, nullValue_), size_(0), pageIndex_(ECS_COMPONENT_PAGE_SIZE) {};
//           ~ComponentPoolBase() = default;

//           /// @brief 获取稠密数组
//           std::vector<ecs::Entity> &getDense() { return dense_; }

//           /// @brief 获取稠密数组有效部分的大小
//           size_t size() const { return size_; }

//           /// @brief 插入一个实体(使实体拥有该类型组件)
//           template <typename... Args>
//           void insert(ecs::Entity entity, Args &&...args)
//           {
//                static_cast<Derived *>(this)->insertImpl(entity, std::forward<Args>(args)...);
//           }

//           /// @brief 替换一个实体的组件
//           template <typename... Args>
//           void replace(ecs::Entity entity, Args &&...args)
//           {
//                static_cast<Derived *>(this)->replaceImpl(entity, std::forward<Args>(args)...);
//           }

//           /// @brief 删除一个实体(使实体失去该类型组件)
//           void erase(ecs::Entity entity)
//           {
//                static_cast<Derived *>(this)->eraseImpl(entity);
//           }

//           /// @brief 尝试删除一个实体(使实体失去该类型组件)
//           void tryErase(ecs::Entity entity)
//           {
//                static_cast<Derived *>(this)->tryEraseImpl(entity);
//           }

//           /// @brief 判断实体是否拥有该类型组件
//           bool has(ecs::Entity entity) const
//           {
//                return static_cast<const Derived *>(this)->hasImpl(entity);
//           }

//           /// @brief 交换两个数的位置
//           void swap(ecs::Entity e1, ecs::Entity e2)
//           {
//                static_cast<Derived *>(this)->swapImpl(e1, e2);
//           }

//           /// @brief 获取实体的组件指针
//           Component *get(ecs::Entity entity)
//           {
//                return static_cast<Derived *>(this)->getImpl(entity);
//           }
//      };

//      template <typename Component>
//      class ComponentPool final : public ComponentPoolBase<ComponentPool<Component>, Component>
//      {
//      private:
//           using Base = ComponentPoolBase<ComponentPool<Component>, Component>;
//           friend Base;

//           /// @brief 插入一个实体(使实体拥有该类型组件)
//           template <typename... Args>
//           void insertImpl(ecs::Entity entity, Args &&...args)
//           {
//                uint32_t index = ecs::entityToIndex(entity);
//                if (index >= this->sparse_.size())
//                {
//                     this->sparse_.resize(std::max((size_t)index + 1, this->sparse_.size() * 2), this->nullValue_);
//                }
//                if (this->sparse_[index] == this->nullValue_)
//                {
//                     if (this->size_ == this->dense_.size()) // 不可重用
//                     {
//                          this->dense_.push_back(entity);
//                          this->ptrs_.push_back(nullptr);
//                          this->sparse_[index] = this->dense_.size() - 1;
//                          this->size_ = this->dense_.size();

//                          if (this->pageIndex_ == ECS_COMPONENT_PAGE_SIZE) // 组件页满了,需要先创建新页
//                          {
//                               this->ComponentPages_.emplace_back(std::make_unique<std::array<Component, ECS_COMPONENT_PAGE_SIZE>>());
//                               this->pageIndex_ = 0;
//                          }
//                          new (&(*this->ComponentPages_.back())[this->pageIndex_]) Component{std::forward<Args>(args)...};
//                          this->ptrs_.back() = &(*this->ComponentPages_.back())[this->pageIndex_];
//                          this->pageIndex_++;
//                     }
//                     else // 可重用
//                     {
//                          this->dense_[this->size_] = entity;
//                          *this->ptrs_[this->size_] = Component{std::forward<Args>(args)...};
//                          this->sparse_[index] = this->size_;
//                          this->size_++;
//                     }
//                } // TODO: 替换组件?
//           }

//           /// @brief 替换一个实体的组件
//           template <typename... Args>
//           void replaceImpl(ecs::Entity entity, Args &&...args)
//           {
//                assert(hasImpl(entity));
//                uint32_t index = ecs::entityToIndex(entity);
//                *this->ptrs_[this->sparse_[index]] = Component{std::forward<Args>(args)...};
//           }

//           /// @brief 删除一个实体(使实体失去该类型组件)
//           void eraseImpl(ecs::Entity entity)
//           {
//                assert(hasImpl(entity));
//                uint32_t index = ecs::entityToIndex(entity);
//                swapImpl(index, this->size_ - 1);
//                this->size_--;
//                this->sparse_[index] = this->nullValue_;
//           }

//           /// @brief 尝试删除一个实体(使实体失去该类型组件)
//           void tryEraseImpl(ecs::Entity entity)
//           {
//                if (hasImpl(entity))
//                {
//                     eraseImpl(entity);
//                }
//           }

//           /// @brief 判断实体是否拥有该类型组件
//           bool hasImpl(ecs::Entity entity) const { return (ecs::entityToIndex(entity) < this->sparse_.size() && this->sparse_[ecs::entityToIndex(entity)] != this->nullValue_); }

//           /// @brief 交换两个数的位置
//           void swapImpl(ecs::Entity e1, ecs::Entity e2)
//           {
//                assert(hasImpl(e1) && hasImpl(e2));
//                uint32_t index1 = ecs::entityToIndex(e1);
//                uint32_t index2 = ecs::entityToIndex(e2);
//                std::swap(this->dense_[this->sparse_[index1]], this->dense_[this->sparse_[index2]]);
//                std::swap(this->ptrs_[this->sparse_[index1]], this->ptrs_[this->sparse_[index2]]);
//                std::swap(this->sparse_[index1], this->sparse_[index2]);
//           }

//           /// @brief 获取实体的组件指针
//           Component *getImpl(ecs::Entity entity)
//           {
//                assert(hasImpl(entity));
//                return this->ptrs_[this->sparse_[ecs::entityToIndex(entity)]];
//           }
//      };
// } // namespace ecs