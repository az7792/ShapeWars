#pragma once
#include <algorithm> // std::min_element
#include "ComponentPool.h"
#include "ComponentTypeID.h"
#include "utils/SparseSet.h"
#include "fwd.h"
namespace ecs
{
     /// @brief 实体管理器
     class EntityManager
     {
          /// @brief 无效实体(ID为0xFFFFF)
          constexpr static const uint32_t nullEntity = 0x000FFFFFu;

          /// @brief 已死亡的实体(版本号为0xFFF)，用于回收
          constexpr static const uint32_t deadEntity = 0x00000FFFu;

          /// @brief 实体视图
          using view = std::vector<Entity>;

          // friend class Group;

     private:
          std::vector<Entity> entities_;      // 所有实体,ID与下标相同且版本页相同才可能是有效的实体
          uint32_t availableEntityIndex_ = 0; // 第一个空闲的实体索引
          // std::vector<Group> groups_;         // 所有组
          //  SparseSet componentIDs_;            // 已创建的组件类型id
     private:
          /// 获取组件池
          template <typename Component>
          static ComponentPool<Component> &getComponentPool()
          {
               static ComponentPool<Component> pool;
               return pool;
          }

     public: // Etity & Component
          /// @brief 创建组
          /// @param direct 直接拥有的组件类型ID列表
          /// @param indirect 间接拥有的组件类型ID列表
          /// @warning 组创建后无法在程序结束前销毁
          // Group &group(const std::vector<uint32_t> &direct, const std::vector<uint32_t> &indirect = {});

          /// @brief 创建一个实体
          Entity createEntity();

          /// @brief 销毁一个实体
          void destroyEntity(Entity entity);

          /// @brief 判断实体是否有效
          bool entityIsValid(Entity entity);

          /// @brief 给实体添加组件
          /// @tparam Component 组件类型
          /// @param entity 实体
          /// @param ...args 组件构造参数
          template <typename Component, typename... Args>
          void addComponent(Entity entity, Args &&...args)
          {
               if (!entityIsValid(entity))
                    return;
               auto &pool = getComponentPool<Component>();
               pool.insert(entity, std::forward<Args>(args)...);
          }

          /// @brief 给实体更换组件
          /// @tparam Component 组件类型
          /// @param entity 实体
          /// @param ...args 组件参数
          template <typename Component, typename... Args>
          void replaceComponent(Entity entity, Args &&...args)
          {
               if (!entityIsValid(entity))
                    return;
               auto &pool = getComponentPool<Component>();
               pool.replace(entity, std::forward<Args>(args)...);
          }

          /// @brief 删除实体的某个组件
          template <typename Component>
          void removeComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return;
               auto &pool = getComponentPool<Component>();
               pool.erase(entity);
          }

          /// @brief 获取实体的某个组件
          template <typename Component>
          Component *getComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return nullptr;
               auto &pool = getComponentPool<Component>();
               return pool.get(entity);
          }

          /// @brief 实体是否包含某个组件
          template <typename Component>
          bool hasComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return false;
               auto &pool = getComponentPool<Component>();
               return pool.has(entity);
          }

          /// @brief 实体是否包含多个指定组件
          template <typename... Components>
          bool hasComponents(Entity entity)
          {
               if (!entityIsValid(entity))
                    return false;
               return (hasComponent<Components>(entity) && ...);
          }

     public: // View
          template <typename... Components>
          std::vector<ecs::Entity> getView()
          {
               std::vector<ecs::Entity> entities;
               std::vector<size_t> poolsSize = {getComponentPool<Components>().size()...};
               std::vector<std::vector<ecs::Entity> *> pools = {&getComponentPool<Components>().getDense()...};

               size_t index = std::min_element(poolsSize.begin(), poolsSize.end()) - poolsSize.begin();

               for (size_t i = 0; i < poolsSize[index]; i++)
               {
                    if (hasComponents<Components...>((*pools[index])[i]))
                         entities.push_back((*pools[index])[i]);
               }
               return entities;
          }
     };

} // namespace ecs
// 对于实体的操作：
// using Entity uin32_t;
// 创建一个实体：EntityManager.CreateEntity()->Entity;
// 判断实体是否有效：EntityManager.IsValid(Entity)->bool;
// 删除一个实体,并移除所有组件：EntityManager.DestroyEntity(Entity)->void;

// 实体添加组件：EntityManager.AddComponent<Component>(Entity,args...)->void;
// 实体添加组件：EntityManager.AddComponent<Component>(Entity)->component_ptr;

// 实体删除组件：EntityManager.RemoveComponent<Component>(Entity)->void;

// 实体获取组件：EntityManager.GetComponent<Component>(Entity)->component_ptr;

// 实体替换组件：EntityManager.ReplaceComponent<Component>(Entity,args...)->void;

// 实体是否包含某个组件：EntityManager.HasComponent<Component>(Entity)->bool;

// 实体是否包含某一组组件：EntityManager.AllOfComponents<Component1,Component2...>(Entity)->bool;

// 实体是否包含任意一个组件：EntityManager.AnyOfComponents<Component1,Component2...>(Entity)->bool;

// 获取实体的组件数量：EntityManager.GetComponentCount(Entity)->size_t;

// 获取实体的组件集合：EntityManager.GetComponentList(Entity)->std::vector<component_ptr>;