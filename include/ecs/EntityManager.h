#pragma once
#include <algorithm> // std::min_element
#include "ComponentPool.h"
#include "utils/SparseMap.h"
#include "ComponentTypeID.h"
#include "utils/SparseSet.h"
#include "utils/Signal.h"
#include "Group.h"
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

          /// 获取信号对应组件的信号
          /// 该类组件池加入或删除实体时用于通知对应的群
          template <typename Component>
          Signal<void, Entity> &getSignal()
          {
               static Signal<void, Entity> signal;
               return signal;
          }

          /// 实体更新组件时触发
          template <typename Component>
          void OnComponentReplaced(Entity entity, Component *cptr)
          {
               static Signal<void, Entity, Component *> OnComponentReplaced;
               OnComponentReplaced(entity, cptr);
          }

          /// 实体销毁时触发
          Signal<void, Entity> OnEntityDestroyed;

          /// 实体创建时触发
          Signal<void, Entity> OnEntityCreated;

          std::vector<std::function<void()>> systems_; // 系统列表

     private:
          // TODO: 使用分页优化
          std::vector<std::unique_ptr<Entity>> entities_; // 所有实体,ID与下标相同且版本好相同才可能是有效的实体,使用指针是为了指针稳定性

          uint32_t entityNum_ = 0;            // 实体数量
          uint32_t availableEntityIndex_ = 0; // 第一个空闲的实体索引
     public:                                  // system
          EntityManager &addSystem(std::function<void()> system)
          {
               systems_.emplace_back(system);
               return *this; // 链式调用
          }

          /// @brief 执行所有系统
          void updateSystems()
          {
               for (auto &system : systems_)
               {
                    system();
               }
          }

          EntityManager() { std::cout << "EntityManager created" << std::endl; }
          ~EntityManager() { std::cout << "EntityManager destroyed" << std::endl; }

     public: // Etity & Component
          /// 获取所有实体
          std::vector<Entity> getEntities();

          /// @brief 创建组
          /// 实体的组件必须不在排除类别并且拥有包含列表的全部类型组件
          /// @param includeComponents 需要包含的组件
          /// @param excludeComponents 需要排除的组件
          /// @warning 组创建后无法在程序结束前销毁
          template <typename... includeComponents, typename... excludeComponents>
          Group<type_list<includeComponents...>, type_list<excludeComponents...>> *group(type_list<excludeComponents...>)
          {
               using GroupType = Group<type_list<includeComponents...>, type_list<excludeComponents...>>;
               static std::unique_ptr<GroupType> groupInstance = nullptr;

               if (!groupInstance)
               {
                    groupInstance = std::make_unique<GroupType>(getEntities());

                    // 连接信号
                    OnEntityDestroyed.connect(std::bind(&GroupType::handleEntityDestroyed, groupInstance.get(), std::placeholders::_1));
                    (getSignal<includeComponents>().connect(std::bind(&GroupType::handleComponentAdded, groupInstance.get(), std::placeholders::_1)), ...);
                    (getSignal<excludeComponents>().connect(std::bind(&GroupType::handleComponentRemoved, groupInstance.get(), std::placeholders::_1)), ...);
               }

               return groupInstance.get();
          }

          /// @brief 创建组
          template <class... includeComponents>
          Group<type_list<includeComponents...>, type_list<>> *group()
          {
               return group<includeComponents...>(type_list<>{});
          }

          uint32_t getEntityNum() const;

          ecs::Entity *getEntityPtr(ecs::Entity entity);

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
          Component *addComponent(Entity entity, Args &&...args)
          {
               if (!entityIsValid(entity))
                    return nullptr;
               auto &pool = ComponentPools::get<Component>();
               Component *cptr = pool.insert(entity, std::forward<Args>(args)...);
               getSignal<Component>().execute(entity);
               return cptr;
          }

          /// @brief 给实体更新组件的数据
          /// @tparam Component 组件类型
          /// @param entity 实体
          /// @param ...args 组件参数
          template <typename Component, typename... Args>
          Component *replaceComponent(Entity entity, Args &&...args)
          {
               if (!entityIsValid(entity))
                    return nullptr;
               auto &pool = ComponentPools::get<Component>();
               Component *cptr = pool.replace(entity, std::forward<Args>(args)...);
               OnComponentReplaced<Component>(entity, cptr);
               return cptr;
          }

          /// @brief 删除实体的某个组件
          template <typename Component>
          void removeComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return;
               auto &pool = ComponentPools::get<Component>();
               pool.erase(entity);
               getSignal<Component>().execute(entity);
          }

          /// @brief 获取实体的某个组件
          template <typename Component>
          Component *getComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return nullptr;
               auto &pool = ComponentPools::get<Component>();
               return pool.get(entity);
          }

          /// @brief 实体是否包含某个组件
          template <typename Component>
          bool hasComponent(Entity entity)
          {
               if (!entityIsValid(entity))
                    return false;
               auto &pool = ComponentPools::get<Component>();
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
               std::vector<size_t> poolsSize = {ComponentPools::get<Components>().size()...};
               std::vector<std::vector<ecs::Entity> *> pools = {&ComponentPools::get<Components>().getDense()...};

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