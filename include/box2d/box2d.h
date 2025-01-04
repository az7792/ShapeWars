// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

#include "base.h"
#include "collision.h"
#include "id.h"
#include "types.h"

#include <stdbool.h>

/**
 * @defgroup world 世界模块
 * 这些函数允许您创建一个模拟世界。
 *
 * 您可以向世界中添加刚体和关节约束并运行模拟。可以获取接触点和法线等接触信息以及事件。
 * 您可以查询世界，例如检查重叠、投射光线或形状。还包括调试信息，例如调试绘制、计时信息和计数器。
 * 文档请参考：https://box2d.org/
 * @{
 */

/// 创建一个用于刚体模拟的世界。一个世界包含物体、形状和约束。最多可以创建128个世界。
/// 每个世界是完全独立的，可以并行模拟。
/// @return 世界ID。
B2_API b2WorldId b2CreateWorld(const b2WorldDef *def);

/// 销毁一个世界
B2_API void b2DestroyWorld(b2WorldId worldId);

/// 世界ID验证。提供最多64K个分配的验证。
B2_API bool b2World_IsValid(b2WorldId id);

/// 模拟一个世界的单个时间步。此操作执行碰撞检测、积分和约束求解。
/// @param worldId 要模拟的世界
/// @param timeStep 模拟的时间步，应该是一个固定值，通常为1/60。
/// @param subStepCount 子步数，增加子步数可以提高精度，通常为4。
B2_API void b2World_Step(b2WorldId worldId, float timeStep, int subStepCount);

/// 调用此函数以绘制形状和其他调试绘制数据
B2_API void b2World_Draw(b2WorldId worldId, b2DebugDraw *draw);

/// 获取当前时间步的物体事件。事件数据是瞬态的，请勿保存对该数据的引用。
B2_API b2BodyEvents b2World_GetBodyEvents(b2WorldId worldId);

/// 获取当前时间步的传感器事件。事件数据是瞬态的，请勿保存对该数据的引用。
B2_API b2SensorEvents b2World_GetSensorEvents(b2WorldId worldId);

/// 获取当前时间步的接触事件。事件数据是瞬态的，请勿保存对该数据的引用。
B2_API b2ContactEvents b2World_GetContactEvents(b2WorldId worldId);

/// 对所有可能与给定AABB重叠的形状进行重叠测试
B2_API b2TreeStats b2World_OverlapAABB(b2WorldId worldId, b2AABB aabb, b2QueryFilter filter, b2OverlapResultFcn *fcn,
							    void *context);

/// 对所有与给定点重叠的形状进行重叠测试
B2_API b2TreeStats b2World_OverlapPoint(b2WorldId worldId, b2Vec2 point, b2Transform transform,
								b2QueryFilter filter, b2OverlapResultFcn *fcn, void *context);

/// 对所有与给定圆形重叠的形状进行重叠测试。可以使用零半径作为点查询。
B2_API b2TreeStats b2World_OverlapCircle(b2WorldId worldId, const b2Circle *circle, b2Transform transform,
								 b2QueryFilter filter, b2OverlapResultFcn *fcn, void *context);

/// 对所有与给定胶囊形状重叠的形状进行重叠测试
B2_API b2TreeStats b2World_OverlapCapsule(b2WorldId worldId, const b2Capsule *capsule, b2Transform transform,
								  b2QueryFilter filter, b2OverlapResultFcn *fcn, void *context);

/// 对所有与给定多边形重叠的形状进行重叠测试
B2_API b2TreeStats b2World_OverlapPolygon(b2WorldId worldId, const b2Polygon *polygon, b2Transform transform,
								  b2QueryFilter filter, b2OverlapResultFcn *fcn, void *context);

/// 向世界中投射一条光线以收集光线路径中的形状。
/// 回调函数决定是获取最近点、任意点还是多个点。
/// 光线投射会忽略包含起点的形状。
/// @note 回调函数可能按任意顺序接收形状
/// @param worldId 要投射光线的世界
/// @param origin 光线的起点
/// @param translation 从起点到终点的光线位移
/// @param filter 包含用于过滤结果中不需要的形状的位标志
/// @param fcn 用户实现的回调函数
/// @param context 传递给回调函数的用户上下文
/// @return 遍历性能计数器
B2_API b2TreeStats b2World_CastRay(b2WorldId worldId, b2Vec2 origin, b2Vec2 translation, b2QueryFilter filter,
							b2CastResultFcn *fcn, void *context);

/// 向世界中投射光线以收集最近的命中点。此函数为便捷函数。
/// 功能比b2World_CastRay()少，不支持自定义过滤。
B2_API b2RayResult b2World_CastRayClosest(b2WorldId worldId, b2Vec2 origin, b2Vec2 translation, b2QueryFilter filter);

/// 向世界中投射一个圆形。类似于投射光线，但投射的是圆而不是点。
/// @see b2World_CastRay
B2_API b2TreeStats b2World_CastCircle(b2WorldId worldId, const b2Circle *circle, b2Transform originTransform,
							   b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn *fcn, void *context);

/// 向世界中投射一个胶囊形状。类似于投射光线，但投射的是胶囊而不是点。
/// @see b2World_CastRay
B2_API b2TreeStats b2World_CastCapsule(b2WorldId worldId, const b2Capsule *capsule, b2Transform originTransform,
							    b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn *fcn, void *context);

/// 使用多边形在世界中进行投射。类似于投射光线，但投射的是多边形而不是点。
/// @see b2World_CastRay
B2_API b2TreeStats b2World_CastPolygon(b2WorldId worldId, const b2Polygon *polygon, b2Transform originTransform,
							    b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn *fcn, void *context);

/// 启用或禁用休眠。如果应用程序不需要休眠，可以通过在世界级别禁用休眠来提高性能。
/// @see b2WorldDef
B2_API void b2World_EnableSleeping(b2WorldId worldId, bool flag);

/// 是否启用了刚体休眠功能？
B2_API bool b2World_IsSleepingEnabled(b2WorldId worldId);

/// 启用或禁用动态与静态刚体间的连续碰撞检测。通常应保持启用连续碰撞检测，以防止快速移动的物体穿过静态物体。禁用连续碰撞检测的性能提升微乎其微。
/// @see b2WorldDef
B2_API void b2World_EnableContinuous(b2WorldId worldId, bool flag);

/// 是否启用了连续碰撞检测？
B2_API bool b2World_IsContinuousEnabled(b2WorldId worldId);

/// 调整恢复系数的阈值。建议不要将此值设得过小，因为这会阻止刚体进入休眠状态。单位通常为米/秒。
/// @see b2WorldDef
B2_API void b2World_SetRestitutionThreshold(b2WorldId worldId, float value);

/// 获取恢复系数的速度阈值。单位通常为米/秒。
B2_API float b2World_GetRestitutionThreshold(b2WorldId worldId);

/// 调整碰撞事件的阈值。该值控制产生 b2ContactHitEvent 碰撞事件所需的碰撞速度。单位通常为米/秒。
/// @see b2WorldDef::hitEventThreshold
B2_API void b2World_SetHitEventThreshold(b2WorldId worldId, float value);

/// 获取碰撞事件的速度阈值。单位通常为米/秒。
B2_API float b2World_GetHitEventThreshold(b2WorldId worldId);

/// 注册自定义过滤器回调（可选）。
B2_API void b2World_SetCustomFilterCallback(b2WorldId worldId, b2CustomFilterFcn *fcn, void *context);

/// 注册预求解回调（可选）。
B2_API void b2World_SetPreSolveCallback(b2WorldId worldId, b2PreSolveFcn *fcn, void *context);

/// 为整个世界设置重力向量。Box2D 不定义“上”方向，这由应用程序自行决定。单位通常为 m/s²。
/// @see b2WorldDef
B2_API void b2World_SetGravity(b2WorldId worldId, b2Vec2 gravity);

/// 获取重力向量
B2_API b2Vec2 b2World_GetGravity(b2WorldId worldId);

/// 应用径向爆炸
/// @param worldId 世界 ID
/// @param explosionDef 爆炸的定义
B2_API void b2World_Explode(b2WorldId worldId, const b2ExplosionDef *explosionDef);

/// 调整接触的调节参数
/// @param worldId 世界 ID
/// @param hertz 接触刚度（以周期/秒为单位）
/// @param dampingRatio 接触的弹性系数，1 表示临界阻尼（无量纲）
/// @param pushVelocity 最大接触约束的推出速度（单位：米/秒）
/// @note 高级功能
B2_API void b2World_SetContactTuning(b2WorldId worldId, float hertz, float dampingRatio, float pushVelocity);

/// 调整关节的调节参数
/// @param worldId 世界 ID
/// @param hertz 接触刚度（以周期/秒为单位）
/// @param dampingRatio 接触的弹性系数，1 表示临界阻尼（无量纲）
/// @note 高级功能
B2_API void b2World_SetJointTuning(b2WorldId worldId, float hertz, float dampingRatio);

/// 设置最大线速度。单位通常为 m/s。
B2_API void b2World_SetMaximumLinearVelocity(b2WorldId worldId, float maximumLinearVelocity);

/// 获取最大线速度。单位通常为 m/s。
B2_API float b2World_GetMaximumLinearVelocity(b2WorldId worldId);

/// 启用或禁用约束的预热启动（Warm Starting）。此为高级测试功能。禁用休眠会大幅降低稳定性，并且没有性能提升。
B2_API void b2World_EnableWarmStarting(b2WorldId worldId, bool flag);

/// 是否启用了约束的预热启动（Warm Starting）？
B2_API bool b2World_IsWarmStartingEnabled(b2WorldId worldId);

/// 获取当前世界的性能信息
B2_API b2Profile b2World_GetProfile(b2WorldId worldId);

/// 获取世界计数器和尺寸
B2_API b2Counters b2World_GetCounters(b2WorldId worldId);

/// 设置用户数据指针
B2_API void b2World_SetUserData(b2WorldId worldId, void *userData);

/// 获取用户数据指针
B2_API void *b2World_GetUserData(b2WorldId worldId);

/// 将内存统计信息转储到 box2d_memory.txt
B2_API void b2World_DumpMemoryStats(b2WorldId worldId);

/// 仅用于内部测试
B2_API void b2World_RebuildStaticTree(b2WorldId worldId);

/// 仅用于内部测试
B2_API void b2World_EnableSpeculative(b2WorldId worldId, bool flag);

/** @} */

/**
 * @defgroup body Body
 * 这是 Body API。
 * @{
 */

/// 根据定义创建一个刚体。不保留对定义的引用，因此可以在栈上创建定义并将其作为指针传递。
/// @code{.c}
/// b2BodyDef bodyDef = b2DefaultBodyDef();
/// b2BodyId myBodyId = b2CreateBody(myWorldId, &bodyDef);
/// @endcode
/// @warning 该函数在回调期间会被锁定。
B2_API b2BodyId b2CreateBody(b2WorldId worldId, const b2BodyDef *def);

/// 根据 ID 销毁一个刚体。这会销毁与刚体相关的所有形状和关节。
/// 请勿保留对相关形状和关节的引用。
B2_API void b2DestroyBody(b2BodyId bodyId);

/// 验证刚体标识符的有效性。可用于检测失效的 ID。最多支持 64K 次分配的验证。
B2_API bool b2Body_IsValid(b2BodyId id);

/// 获取刚体的类型：静态、运动学或动态。
B2_API b2BodyType b2Body_GetType(b2BodyId bodyId);

/// 更改刚体的类型。这是一个开销较大的操作，会自动更新质量属性，无论自动质量设置为何。
B2_API void b2Body_SetType(b2BodyId bodyId, b2BodyType type);

/// 设置刚体的用户数据。
B2_API void b2Body_SetUserData(b2BodyId bodyId, void *userData);

/// 获取存储在刚体中的用户数据。
B2_API void *b2Body_GetUserData(b2BodyId bodyId);

/// 获取刚体的世界位置（刚体原点的位置）。
B2_API b2Vec2 b2Body_GetPosition(b2BodyId bodyId);

/// 以余弦/正弦对（复数）形式获取刚体的世界旋转。
B2_API b2Rot b2Body_GetRotation(b2BodyId bodyId);

/// 获取刚体的世界变换。
B2_API b2Transform b2Body_GetTransform(b2BodyId bodyId);

/// 设置刚体的世界变换。这相当于传送，开销较大。
/// @note 通常应使用所需的变换创建刚体。
/// @see b2BodyDef::position 和 b2BodyDef::angle
B2_API void b2Body_SetTransform(b2BodyId bodyId, b2Vec2 position, b2Rot rotation);

/// 根据世界点获取刚体的局部点。
B2_API b2Vec2 b2Body_GetLocalPoint(b2BodyId bodyId, b2Vec2 worldPoint);

/// 根据局部点获取刚体的世界点。
B2_API b2Vec2 b2Body_GetWorldPoint(b2BodyId bodyId, b2Vec2 localPoint);

/// 根据世界向量获取刚体的局部向量。
B2_API b2Vec2 b2Body_GetLocalVector(b2BodyId bodyId, b2Vec2 worldVector);

/// 根据局部向量获取刚体的世界向量。
B2_API b2Vec2 b2Body_GetWorldVector(b2BodyId bodyId, b2Vec2 localVector);

/// 获取刚体质心的线速度，通常以米/秒为单位。
B2_API b2Vec2 b2Body_GetLinearVelocity(b2BodyId bodyId);

/// 获取刚体的角速度，以弧度/秒为单位。
B2_API float b2Body_GetAngularVelocity(b2BodyId bodyId);

/// 设置刚体的线速度，通常以米/秒为单位。
B2_API void b2Body_SetLinearVelocity(b2BodyId bodyId, b2Vec2 linearVelocity);

/// 设置刚体的角速度，以弧度/秒为单位。
B2_API void b2Body_SetAngularVelocity(b2BodyId bodyId, float angularVelocity);

/// 在世界点施加一个力。如果力未施加在质心，则会产生扭矩并影响角速度。可选唤醒刚体。
/// 如果刚体未唤醒，则忽略该力。
/// @param bodyId 刚体 ID
/// @param force 世界力向量，通常以牛顿（N）为单位
/// @param point 力的作用点的世界位置
/// @param wake 是否唤醒刚体
B2_API void b2Body_ApplyForce(b2BodyId bodyId, b2Vec2 force, b2Vec2 point, bool wake);

/// 向质心施加一个力。可选唤醒刚体。如果刚体未唤醒，则忽略该力。
/// @param bodyId 刚体 ID
/// @param force 世界力向量，通常以牛顿（N）为单位
/// @param wake 是否唤醒刚体
B2_API void b2Body_ApplyForceToCenter(b2BodyId bodyId, b2Vec2 force, bool wake);

/// 施加扭矩。这会影响角速度，但不会影响线速度。可选唤醒刚体。
/// 如果刚体未唤醒，则忽略该扭矩。
/// @param bodyId 刚体 ID
/// @param torque 绕 z 轴的扭矩（屏幕外方向），通常以 N*m 为单位
/// @param wake 是否唤醒刚体
B2_API void b2Body_ApplyTorque(b2BodyId bodyId, float torque, bool wake);

/// 在某点施加冲量。立即修改速度。如果施加点不在质心，则会修改角速度。可选唤醒刚体。
/// 如果刚体未唤醒，则忽略该冲量。
/// @param bodyId 刚体 ID
/// @param impulse 世界冲量向量，通常以 N*s 或 kg*m/s 为单位
/// @param point 作用点的世界位置
/// @param wake 是否唤醒刚体
/// @warning 应用于单次冲量。如果需要持续作用的力，应使用力，这样与子步长求解器配合更好。
B2_API void b2Body_ApplyLinearImpulse(b2BodyId bodyId, b2Vec2 impulse, b2Vec2 point, bool wake);

/// 向质心施加冲量。立即修改速度。如果刚体未唤醒，则忽略该冲量。可选唤醒刚体。
/// @param bodyId 刚体 ID
/// @param impulse 世界冲量向量，通常以 N*s 或 kg*m/s 为单位
/// @param wake 是否唤醒刚体
/// @warning 应用于单次冲量。如果需要持续作用的力，应使用力，这样与子步长求解器配合更好。
B2_API void b2Body_ApplyLinearImpulseToCenter(b2BodyId bodyId, b2Vec2 impulse, bool wake);

/// 施加角冲量。如果刚体未唤醒，则忽略该冲量。可选唤醒刚体。
/// @param bodyId 刚体 ID
/// @param impulse 角冲量，通常以 kg*m*m/s 为单位
/// @param wake 是否唤醒刚体
/// @warning 应用于单次冲量。如果需要持续作用的力，应使用力，这样与子步长求解器配合更好。
B2_API void b2Body_ApplyAngularImpulse(b2BodyId bodyId, float impulse, bool wake);

/// 获取刚体的质量，通常以千克为单位。
B2_API float b2Body_GetMass(b2BodyId bodyId);

/// 获取刚体的旋转惯量，通常以 kg*m^2 为单位。
B2_API float b2Body_GetRotationalInertia(b2BodyId bodyId);

/// 获取刚体的局部质心位置。
B2_API b2Vec2 b2Body_GetLocalCenterOfMass(b2BodyId bodyId);

/// 获取刚体的世界质心位置。
B2_API b2Vec2 b2Body_GetWorldCenterOfMass(b2BodyId bodyId);

/// 覆盖刚体的质量属性。通常使用形状几何和密度自动计算。
/// 如果添加或移除形状或更改刚体类型，则此信息会丢失。
B2_API void b2Body_SetMassData(b2BodyId bodyId, b2MassData massData);

/// 获取刚体的质量数据。
B2_API b2MassData b2Body_GetMassData(b2BodyId bodyId);

/// 更新刚体的质量属性为其所有形状的质量属性的总和。
/// 通常不需要调用此函数，除非您之前使用了 SetMassData 重写了质量，并希望稍后重置质量。
/// 也可以在禁用了自动质量计算的情况下使用此函数。
/// 无论刚体的类型如何，都应该调用此函数。
B2_API void b2Body_ApplyMassFromShapes(b2BodyId bodyId);

/// 调整线性阻尼。通常在线性阻尼值会在创建之前通过 b2BodyDef 设置。
B2_API void b2Body_SetLinearDamping(b2BodyId bodyId, float linearDamping);

/// 获取当前的线性阻尼值。
B2_API float b2Body_GetLinearDamping(b2BodyId bodyId);

/// 调整角阻尼。通常在创建之前通过 b2BodyDef 设置。
B2_API void b2Body_SetAngularDamping(b2BodyId bodyId, float angularDamping);

/// 获取当前的角阻尼值。
B2_API float b2Body_GetAngularDamping(b2BodyId bodyId);

/// 调整重力缩放值。通常在创建之前通过 b2BodyDef 设置。
/// @see b2BodyDef::gravityScale
B2_API void b2Body_SetGravityScale(b2BodyId bodyId, float gravityScale);

/// 获取当前的重力缩放值。
B2_API float b2Body_GetGravityScale(b2BodyId bodyId);

/// 返回 true 如果此刚体当前处于唤醒状态。
B2_API bool b2Body_IsAwake(b2BodyId bodyId);

/// 将刚体从休眠状态唤醒。这会唤醒该刚体接触的整个岛屿。
/// @warning 将刚体置于休眠状态会导致接触此刚体的整个岛屿进入休眠状态，
/// 这可能代价昂贵并且可能不符合预期。
B2_API void b2Body_SetAwake(b2BodyId bodyId, bool awake);

/// 启用或禁用此刚体的休眠功能。如果禁用休眠，该刚体会被唤醒。
B2_API void b2Body_EnableSleep(b2BodyId bodyId, bool enableSleep);

/// 返回 true 如果此刚体启用了休眠功能。
B2_API bool b2Body_IsSleepEnabled(b2BodyId bodyId);

/// 设置休眠阈值，通常以米每秒为单位。
B2_API void b2Body_SetSleepThreshold(b2BodyId bodyId, float sleepThreshold);

/// 获取休眠阈值，通常以米每秒为单位。
B2_API float b2Body_GetSleepThreshold(b2BodyId bodyId);

/// 返回 true 如果此刚体当前处于启用状态。
B2_API bool b2Body_IsEnabled(b2BodyId bodyId);

/// 禁用一个刚体，通过将其完全从模拟中移除。这操作代价昂贵。
B2_API void b2Body_Disable(b2BodyId bodyId);

/// 启用一个刚体，通过将其添加到模拟中。这操作代价昂贵。
B2_API void b2Body_Enable(b2BodyId bodyId);

/// 设置此刚体为固定旋转。无论何种情况下，这都会导致质量被重置。
B2_API void b2Body_SetFixedRotation(b2BodyId bodyId, bool flag);

/// 此刚体是否为固定旋转状态？
B2_API bool b2Body_IsFixedRotation(b2BodyId bodyId);

/// 设置此刚体为子弹类型。子弹会对动态刚体（但不会对其他子弹）进行连续碰撞检测。
B2_API void b2Body_SetBullet(b2BodyId bodyId, bool flag);

/// 此刚体是否为子弹类型？
B2_API bool b2Body_IsBullet(b2BodyId bodyId);

/// 启用/禁用所有形状上的命中事件
/// @see b2ShapeDef::enableHitEvents
B2_API void b2Body_EnableHitEvents(b2BodyId bodyId, bool enableHitEvents);

/// 获取拥有该物体的世界
B2_API b2WorldId b2Body_GetWorld(b2BodyId bodyId);

/// 获取此物体上的形状数量
B2_API int b2Body_GetShapeCount(b2BodyId bodyId);

/// 获取此物体上所有形状的 ID，最多存储到提供的容量中。
/// @returns 存储到用户数组中的形状 ID 数量
B2_API int b2Body_GetShapes(b2BodyId bodyId, b2ShapeId *shapeArray, int capacity);

/// 获取此物体上的关节数量
B2_API int b2Body_GetJointCount(b2BodyId bodyId);

/// 获取此物体上所有关节的 ID，最多存储到提供的容量中。
/// @returns 存储到用户数组中的关节 ID 数量
B2_API int b2Body_GetJoints(b2BodyId bodyId, b2JointId *jointArray, int capacity);

/// 获取检索该物体所有接触点数据所需的最大容量
B2_API int b2Body_GetContactCapacity(b2BodyId bodyId);

/// 获取物体的接触点数据
B2_API int b2Body_GetContactData(b2BodyId bodyId, b2ContactData *contactData, int capacity);

/// 计算包含所有附加形状的当前世界 AABB。注意，这可能不包括物体原点。
/// 如果没有附加形状，则返回的 AABB 是空的并以物体原点为中心。
B2_API b2AABB b2Body_ComputeAABB(b2BodyId bodyId);

/** @} */

/**
 * @defgroup shape 形状
 * 用于创建、销毁和访问的函数。
 * 形状将原始几何体绑定到物体，并包含包括摩擦力和弹性在内的材质属性。
 * @{
 */

/// 创建一个圆形并将其附加到物体。形状定义和几何体将被完全克隆。
/// 在下一步更新之前不会创建接触点。
/// @return 用于访问形状的形状 ID
B2_API b2ShapeId b2CreateCircleShape(b2BodyId bodyId, const b2ShapeDef *def, const b2Circle *circle);

/// 创建一个线段形状并将其附加到物体。形状定义和几何体将被完全克隆。
/// 在下一步更新之前不会创建接触点。
/// @return 用于访问形状的形状 ID
B2_API b2ShapeId b2CreateSegmentShape(b2BodyId bodyId, const b2ShapeDef *def, const b2Segment *segment);

/// 创建一个胶囊形状并将其附加到物体。形状定义和几何体将被完全克隆。
/// 在下一步更新之前不会创建接触点。
/// @return 用于访问形状的形状 ID
B2_API b2ShapeId b2CreateCapsuleShape(b2BodyId bodyId, const b2ShapeDef *def, const b2Capsule *capsule);

/// 创建一个多边形形状并将其附加到物体。形状定义和几何体将被完全克隆。
/// 在下一步更新之前不会创建接触点。
/// @return 用于访问形状的形状 ID
B2_API b2ShapeId b2CreatePolygonShape(b2BodyId bodyId, const b2ShapeDef *def, const b2Polygon *polygon);

/// 销毁一个形状。如果一次销毁物体上的多个形状，可以延迟更新物体质量以提高性能。
/// @see b2Body_ApplyMassFromShapes
B2_API void b2DestroyShape(b2ShapeId shapeId, bool updateBodyMass);

/// 验证形状标识符。提供最多 64K 分配的验证。
B2_API bool b2Shape_IsValid(b2ShapeId id);

/// 获取形状的类型
B2_API b2ShapeType b2Shape_GetType(b2ShapeId shapeId);

/// 获取附加此形状的物体 ID
B2_API b2BodyId b2Shape_GetBody(b2ShapeId shapeId);

/// 获取拥有此形状的世界
B2_API b2WorldId b2Shape_GetWorld(b2ShapeId shapeId);

/// 如果形状是传感器，则返回 true
B2_API bool b2Shape_IsSensor(b2ShapeId shapeId);

/// 设置形状的用户数据
B2_API void b2Shape_SetUserData(b2ShapeId shapeId, void *userData);

/// 获取形状的用户数据。当您从事件或查询中获取形状 ID 时，这非常有用。
B2_API void *b2Shape_GetUserData(b2ShapeId shapeId);

/// 设置形状的质量密度，通常以 kg/m^2 为单位。
/// 这将可选地更新父物体的质量属性。
/// @see b2ShapeDef::density, b2Body_ApplyMassFromShapes
B2_API void b2Shape_SetDensity(b2ShapeId shapeId, float density, bool updateBodyMass);

/// 获取形状的质量密度，通常以 kg/m^2 为单位
B2_API float b2Shape_GetDensity(b2ShapeId shapeId);

/// 设置形状的摩擦力
/// @see b2ShapeDef::friction
B2_API void b2Shape_SetFriction(b2ShapeId shapeId, float friction);

/// 获取形状的摩擦系数
B2_API float b2Shape_GetFriction(b2ShapeId shapeId);

/// 设置形状的恢复系数（弹性系数）
/// @see b2ShapeDef::restitution
B2_API void b2Shape_SetRestitution(b2ShapeId shapeId, float restitution);

/// 获取形状的恢复系数
B2_API float b2Shape_GetRestitution(b2ShapeId shapeId);

/// 获取形状的过滤器
B2_API b2Filter b2Shape_GetFilter(b2ShapeId shapeId);

/// 设置当前过滤器。此操作的开销与重新创建形状几乎相当。
/// @see b2ShapeDef::filter
B2_API void b2Shape_SetFilter(b2ShapeId shapeId, b2Filter filter);

/// 启用该形状的传感器事件。仅适用于运动学和动态刚体，传感器将忽略此设置。
/// @see b2ShapeDef::isSensor
B2_API void b2Shape_EnableSensorEvents(b2ShapeId shapeId, bool flag);

/// 如果启用了传感器事件，返回 true
B2_API bool b2Shape_AreSensorEventsEnabled(b2ShapeId shapeId);

/// 启用该形状的接触事件。仅适用于运动学和动态刚体，传感器将忽略此设置。
/// @see b2ShapeDef::enableContactEvents
B2_API void b2Shape_EnableContactEvents(b2ShapeId shapeId, bool flag);

/// 如果启用了接触事件，返回 true
B2_API bool b2Shape_AreContactEventsEnabled(b2ShapeId shapeId);

/// 启用该形状的预解决接触事件。仅适用于动态刚体。此操作开销较大，需谨慎处理以避免多线程问题。传感器将忽略此设置。
/// @see b2PreSolveFcn
B2_API void b2Shape_EnablePreSolveEvents(b2ShapeId shapeId, bool flag);

/// 如果启用了预解决事件，返回 true
B2_API bool b2Shape_ArePreSolveEventsEnabled(b2ShapeId shapeId);

/// 启用该形状的接触命中事件。传感器将忽略此设置。
/// @see b2WorldDef.hitEventThreshold
B2_API void b2Shape_EnableHitEvents(b2ShapeId shapeId, bool flag);

/// 如果启用了命中事件，返回 true
B2_API bool b2Shape_AreHitEventsEnabled(b2ShapeId shapeId);

/// 测试某点是否与形状重叠
B2_API bool b2Shape_TestPoint(b2ShapeId shapeId, b2Vec2 point);

/// 对形状进行直接射线检测
B2_API b2CastOutput b2Shape_RayCast(b2ShapeId shapeId, const b2RayCastInput *input);

/// 获取形状的圆形副本。确保类型正确。
B2_API b2Circle b2Shape_GetCircle(b2ShapeId shapeId);

/// 获取形状的线段副本。确保类型正确。
B2_API b2Segment b2Shape_GetSegment(b2ShapeId shapeId);

/// 获取形状的链条段副本。链条段来源于链条形状。
/// 确保类型正确。
B2_API b2ChainSegment b2Shape_GetChainSegment(b2ShapeId shapeId);

/// 获取形状的胶囊体副本。确保类型正确。
B2_API b2Capsule b2Shape_GetCapsule(b2ShapeId shapeId);

/// 获取形状的凸多边形副本。确保类型正确。
B2_API b2Polygon b2Shape_GetPolygon(b2ShapeId shapeId);

/// 将形状更改为圆形或更新当前圆形。
/// 此操作不会修改质量属性。
/// @see b2Body_ApplyMassFromShapes
B2_API void b2Shape_SetCircle(b2ShapeId shapeId, const b2Circle *circle);

/// 将形状更改为胶囊体或更新当前胶囊体。
/// 此操作不会修改质量属性。
/// @see b2Body_ApplyMassFromShapes
B2_API void b2Shape_SetCapsule(b2ShapeId shapeId, const b2Capsule *capsule);

/// 将形状更改为线段或更新当前线段。
B2_API void b2Shape_SetSegment(b2ShapeId shapeId, const b2Segment *segment);

/// 将形状更改为多边形或更新当前多边形。
/// 此操作不会修改质量属性。
/// @see b2Body_ApplyMassFromShapes
B2_API void b2Shape_SetPolygon(b2ShapeId shapeId, const b2Polygon *polygon);

/// 如果形状类型为链条段，获取其父链条的 ID，否则返回 b2_nullChainId。
B2_API b2ChainId b2Shape_GetParentChain(b2ShapeId shapeId);

/// 获取该形状上所有接触点数据所需的最大容量
B2_API int b2Shape_GetContactCapacity(b2ShapeId shapeId);

/// 获取形状的接触点数据。提供的 shapeId 将作为 contactData 中的 shapeIdA 或 shapeIdB。
B2_API int b2Shape_GetContactData(b2ShapeId shapeId, b2ContactData *contactData, int capacity);

/// 获取当前的世界 AABB
B2_API b2AABB b2Shape_GetAABB(b2ShapeId shapeId);

/// 获取形状上最接近目标点的点。目标点和结果点均在世界空间中。
B2_API b2Vec2 b2Shape_GetClosestPoint(b2ShapeId shapeId, b2Vec2 target);

/// 链形状

/// 创建一个链形状
/// @see b2ChainDef 了解详细信息
B2_API b2ChainId b2CreateChain(b2BodyId bodyId, const b2ChainDef *def);

/// 销毁一个链形状
B2_API void b2DestroyChain(b2ChainId chainId);

/// 获取拥有该链形状的世界
B2_API b2WorldId b2Chain_GetWorld(b2ChainId chainId);

/// 获取链上的段数
B2_API int b2Chain_GetSegmentCount(b2ChainId chainId);

/// 将链段形状 ID 填充到用户数组中，最多填充到指定的容量。返回实际填充的段数。
B2_API int b2Chain_GetSegments(b2ChainId chainId, b2ShapeId *segmentArray, int capacity);

/// 设置链的摩擦系数
/// @see b2ChainDef::friction
B2_API void b2Chain_SetFriction(b2ChainId chainId, float friction);

/// 获取链的摩擦系数
B2_API float b2Chain_GetFriction(b2ChainId chainId);

/// 设置链的回弹系数（弹性）
B2_API void b2Chain_SetRestitution(b2ChainId chainId, float restitution);

/// 获取链的回弹系数
B2_API float b2Chain_GetRestitution(b2ChainId chainId);

/// 链标识符验证。支持验证多达 64K 个分配。
B2_API bool b2Chain_IsValid(b2ChainId id);

/** @} */

/**
 * @defgroup joint 关节
 * @brief 关节允许将刚体连接在一起，同时允许各种形式的相对运动。
 * @{
 */

/// 销毁一个关节
B2_API void b2DestroyJoint(b2JointId jointId);

/// 关节标识符验证。支持验证多达 64K 个分配。
B2_API bool b2Joint_IsValid(b2JointId id);

/// 获取关节类型
B2_API b2JointType b2Joint_GetType(b2JointId jointId);

/// 获取关节中物体 A 的 ID
B2_API b2BodyId b2Joint_GetBodyA(b2JointId jointId);

/// 获取关节中物体 B 的 ID
B2_API b2BodyId b2Joint_GetBodyB(b2JointId jointId);

/// 获取拥有该关节的世界
B2_API b2WorldId b2Joint_GetWorld(b2JointId jointId);

/// 获取物体 A 的局部锚点
B2_API b2Vec2 b2Joint_GetLocalAnchorA(b2JointId jointId);

/// 获取物体 B 的局部锚点
B2_API b2Vec2 b2Joint_GetLocalAnchorB(b2JointId jointId);

/// 切换是否允许连接的物体之间碰撞
B2_API void b2Joint_SetCollideConnected(b2JointId jointId, bool shouldCollide);

/// 检查是否允许连接的物体之间碰撞
B2_API bool b2Joint_GetCollideConnected(b2JointId jointId);

/// 设置关节上的用户数据
B2_API void b2Joint_SetUserData(b2JointId jointId, void *userData);

/// 获取关节上的用户数据
B2_API void *b2Joint_GetUserData(b2JointId jointId);

/// 唤醒连接到此关节的物体
B2_API void b2Joint_WakeBodies(b2JointId jointId);

/// 获取此关节的当前约束力
B2_API b2Vec2 b2Joint_GetConstraintForce(b2JointId jointId);

/// 获取此关节的当前约束扭矩
B2_API float b2Joint_GetConstraintTorque(b2JointId jointId);

/**
 * @defgroup distance_joint 距离关节
 * @brief 距离关节的功能。
 * @{
 */

/// 创建一个距离关节
/// @see b2DistanceJointDef 了解详细信息
B2_API b2JointId b2CreateDistanceJoint(b2WorldId worldId, const b2DistanceJointDef *def);

/// 设置距离关节的静止长度
/// @param jointId 距离关节的ID
/// @param length 新的距离关节长度
B2_API void b2DistanceJoint_SetLength(b2JointId jointId, float length);

/// 获取距离关节的静止长度
B2_API float b2DistanceJoint_GetLength(b2JointId jointId);

/// 启用/禁用距离关节弹簧。禁用时，距离关节为刚性。
B2_API void b2DistanceJoint_EnableSpring(b2JointId jointId, bool enableSpring);

/// 距离关节的弹簧是否启用？
B2_API bool b2DistanceJoint_IsSpringEnabled(b2JointId jointId);

/// 设置弹簧的刚度（赫兹）
B2_API void b2DistanceJoint_SetSpringHertz(b2JointId jointId, float hertz);

/// 设置弹簧的阻尼比（无量纲）
B2_API void b2DistanceJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取弹簧的赫兹值
B2_API float b2DistanceJoint_GetSpringHertz(b2JointId jointId);

/// 获取弹簧的阻尼比
B2_API float b2DistanceJoint_GetSpringDampingRatio(b2JointId jointId);

/// 启用关节限制。只有当关节弹簧启用时，限制才有效。否则，关节为刚性，限制没有效果。
B2_API void b2DistanceJoint_EnableLimit(b2JointId jointId, bool enableLimit);

/// 距离关节的限制是否启用？
B2_API bool b2DistanceJoint_IsLimitEnabled(b2JointId jointId);

/// 设置距离关节的最小和最大长度
B2_API void b2DistanceJoint_SetLengthRange(b2JointId jointId, float minLength, float maxLength);

/// 获取距离关节的最小长度
B2_API float b2DistanceJoint_GetMinLength(b2JointId jointId);

/// 获取距离关节的最大长度
B2_API float b2DistanceJoint_GetMaxLength(b2JointId jointId);

/// 获取距离关节的当前长度
B2_API float b2DistanceJoint_GetCurrentLength(b2JointId jointId);

/// 启用/禁用距离关节马达
B2_API void b2DistanceJoint_EnableMotor(b2JointId jointId, bool enableMotor);

/// 距离关节的马达是否启用？
B2_API bool b2DistanceJoint_IsMotorEnabled(b2JointId jointId);

/// 设置距离关节马达的速度，通常以米每秒为单位
B2_API void b2DistanceJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);

/// 获取距离关节马达的速度，通常以米每秒为单位
B2_API float b2DistanceJoint_GetMotorSpeed(b2JointId jointId);

/// 设置距离关节马达的最大力量，通常以牛顿为单位
B2_API void b2DistanceJoint_SetMaxMotorForce(b2JointId jointId, float force);

/// 获取距离关节马达的最大力量，通常以牛顿为单位
B2_API float b2DistanceJoint_GetMaxMotorForce(b2JointId jointId);

/// 获取距离关节马达的当前力量，通常以牛顿为单位
B2_API float b2DistanceJoint_GetMotorForce(b2JointId jointId);

/** @} */

/**
 * @defgroup motor_joint 马达关节
 * @brief 马达关节的相关函数。
 *
 * 马达关节用于驱动两个物体之间的相对变换。它通过应用必要的力和扭矩来实现
 * 随时间变化的相对位置和旋转。
 * @{
 */

/// 创建一个马达关节
/// @see b2MotorJointDef 获取详细信息
B2_API b2JointId b2CreateMotorJoint(b2WorldId worldId, const b2MotorJointDef *def);

/// 设置马达关节的线性偏移目标
B2_API void b2MotorJoint_SetLinearOffset(b2JointId jointId, b2Vec2 linearOffset);

/// 获取马达关节的线性偏移目标
B2_API b2Vec2 b2MotorJoint_GetLinearOffset(b2JointId jointId);

/// 设置马达关节的角度偏移目标，单位为弧度
B2_API void b2MotorJoint_SetAngularOffset(b2JointId jointId, float angularOffset);

/// 获取马达关节的角度偏移目标，单位为弧度
B2_API float b2MotorJoint_GetAngularOffset(b2JointId jointId);

/// 设置马达关节的最大力量，通常以牛顿为单位
B2_API void b2MotorJoint_SetMaxForce(b2JointId jointId, float maxForce);

/// 获取马达关节的最大力量，通常以牛顿为单位
B2_API float b2MotorJoint_GetMaxForce(b2JointId jointId);

/// 设置马达关节的最大扭矩，通常以牛顿米为单位
B2_API void b2MotorJoint_SetMaxTorque(b2JointId jointId, float maxTorque);

/// 获取马达关节的最大扭矩，通常以牛顿米为单位
B2_API float b2MotorJoint_GetMaxTorque(b2JointId jointId);

/// 设置马达关节的修正因子，通常为[0, 1]之间的值
B2_API void b2MotorJoint_SetCorrectionFactor(b2JointId jointId, float correctionFactor);

/// 获取马达关节的修正因子，通常为[0, 1]之间的值
B2_API float b2MotorJoint_GetCorrectionFactor(b2JointId jointId);

/**@}*/

/**
 * @defgroup mouse_joint 鼠标关节
 * @brief 鼠标关节的函数。
 *
 * 鼠标关节设计用于样本应用程序，但在用户通过光标移动刚体的应用程序中也可能有用。
 * @{
 */

/// 创建鼠标关节
/// @see b2MouseJointDef 获取详细信息
B2_API b2JointId b2CreateMouseJoint(b2WorldId worldId, const b2MouseJointDef *def);

/// 设置鼠标关节目标
B2_API void b2MouseJoint_SetTarget(b2JointId jointId, b2Vec2 target);

/// 获取鼠标关节目标
B2_API b2Vec2 b2MouseJoint_GetTarget(b2JointId jointId);

/// 设置鼠标关节的弹簧刚度（赫兹）
B2_API void b2MouseJoint_SetSpringHertz(b2JointId jointId, float hertz);

/// 获取鼠标关节的弹簧刚度（赫兹）
B2_API float b2MouseJoint_GetSpringHertz(b2JointId jointId);

/// 设置鼠标关节的弹簧阻尼比（无量纲）
B2_API void b2MouseJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取鼠标关节的阻尼比（无量纲）
B2_API float b2MouseJoint_GetSpringDampingRatio(b2JointId jointId);

/// 设置鼠标关节的最大力，通常以牛顿为单位
B2_API void b2MouseJoint_SetMaxForce(b2JointId jointId, float maxForce);

/// 获取鼠标关节的最大力，通常以牛顿为单位
B2_API float b2MouseJoint_GetMaxForce(b2JointId jointId);

/**@}*/

/**
 * @defgroup null_joint 空关节
 * @brief 空关节的函数。
 *
 * 空关节用于禁用两个刚体之间的碰撞。作为关节的副作用，它还将两个刚体保持在同一个模拟岛中。
 * @{
 */

/// 创建一个空关节。
/// @see b2NullJointDef 获取详细信息
B2_API b2JointId b2CreateNullJoint(b2WorldId worldId, const b2NullJointDef *def);

/**@}*/

/**
 * @defgroup prismatic_joint 活塞关节
 * @brief 活塞关节允许沿单一轴线平移而没有旋转。
 *
 * 活塞关节适用于像活塞和移动平台这样的情况，在这些情况下，您希望刚体沿某个轴平移而不发生旋转。也叫做 *滑动* 关节。
 * @{
 */

/// 创建一个活塞（滑动）关节。
/// @see b2PrismaticJointDef 获取详细信息
B2_API b2JointId b2CreatePrismaticJoint(b2WorldId worldId, const b2PrismaticJointDef *def);

/// 启用/禁用关节弹簧
B2_API void b2PrismaticJoint_EnableSpring(b2JointId jointId, bool enableSpring);

/// 活塞关节的弹簧是否启用？
B2_API bool b2PrismaticJoint_IsSpringEnabled(b2JointId jointId);

/// 设置活塞关节的刚度（赫兹）。
/// 这个值通常应该小于仿真速率的四分之一。例如，如果仿真以60Hz运行，那么关节刚度应该为15Hz或更低。
B2_API void b2PrismaticJoint_SetSpringHertz(b2JointId jointId, float hertz);

/// 获取活塞关节的刚度（赫兹）
B2_API float b2PrismaticJoint_GetSpringHertz(b2JointId jointId);

/// 设置活塞关节的阻尼比（无量纲）
B2_API void b2PrismaticJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取活塞弹簧的阻尼比（无量纲）
B2_API float b2PrismaticJoint_GetSpringDampingRatio(b2JointId jointId);

/// 启用/禁用活塞关节限制
B2_API void b2PrismaticJoint_EnableLimit(b2JointId jointId, bool enableLimit);

/// 活塞关节的限制是否启用？
B2_API bool b2PrismaticJoint_IsLimitEnabled(b2JointId jointId);

/// 获取活塞关节的下限
B2_API float b2PrismaticJoint_GetLowerLimit(b2JointId jointId);

/// 获取活塞关节的上限
B2_API float b2PrismaticJoint_GetUpperLimit(b2JointId jointId);

/// 设置活塞关节的限制
B2_API void b2PrismaticJoint_SetLimits(b2JointId jointId, float lower, float upper);

/// 启用/禁用活塞关节马达
B2_API void b2PrismaticJoint_EnableMotor(b2JointId jointId, bool enableMotor);

/// 活塞关节的马达是否启用？
B2_API bool b2PrismaticJoint_IsMotorEnabled(b2JointId jointId);

/// 设置活塞关节的马达速度，通常以米每秒为单位
B2_API void b2PrismaticJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);

/// 获取活塞关节的马达速度，通常以米每秒为单位
B2_API float b2PrismaticJoint_GetMotorSpeed(b2JointId jointId);

/// 设置活塞关节的最大马达力，通常以牛顿为单位
B2_API void b2PrismaticJoint_SetMaxMotorForce(b2JointId jointId, float force);

/// 获取活塞关节的最大马达力，通常以牛顿为单位
B2_API float b2PrismaticJoint_GetMaxMotorForce(b2JointId jointId);

/// 获取活塞关节的当前马达力，通常以牛顿为单位
B2_API float b2PrismaticJoint_GetMotorForce(b2JointId jointId);

/// 获取当前关节的平移，通常以米为单位。
B2_API float b2PrismaticJoint_GetTranslation(b2JointId jointId);

/// 获取当前关节的平移速度，通常以米每秒为单位。
B2_API float b2PrismaticJoint_GetSpeed(b2JointId jointId);

/** @} */

/**
 * @defgroup revolute_joint Revolute Joint
 * @brief 转动关节允许在2D平面中进行相对旋转，而没有相对平移。
 *
 * 转动关节可能是最常见的关节类型。它可以用于布娃娃物理和链条。
 * 也称为*铰链*或*销*关节。
 * @{
 */

/// 创建一个转动关节
/// @see b2RevoluteJointDef 获取详细信息
B2_API b2JointId b2CreateRevoluteJoint(b2WorldId worldId, const b2RevoluteJointDef *def);

/// 启用/禁用转动关节的弹簧
B2_API void b2RevoluteJoint_EnableSpring(b2JointId jointId, bool enableSpring);

/// 转动关节的角度弹簧是否启用？
B2_API bool b2RevoluteJoint_IsSpringEnabled(b2JointId jointId);

/// 设置转动关节弹簧的赫兹刚度
B2_API void b2RevoluteJoint_SetSpringHertz(b2JointId jointId, float hertz);

/// 获取转动关节弹簧的赫兹刚度
B2_API float b2RevoluteJoint_GetSpringHertz(b2JointId jointId);

/// 设置转动关节弹簧的阻尼比，无量纲
B2_API void b2RevoluteJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取转动关节弹簧的阻尼比，无量纲
B2_API float b2RevoluteJoint_GetSpringDampingRatio(b2JointId jointId);

/// 获取转动关节相对于参考角度的当前角度（弧度）
/// @see b2RevoluteJointDef::referenceAngle
B2_API float b2RevoluteJoint_GetAngle(b2JointId jointId);

/// 启用/禁用转动关节的限制
B2_API void b2RevoluteJoint_EnableLimit(b2JointId jointId, bool enableLimit);

/// 转动关节的限制是否启用？
B2_API bool b2RevoluteJoint_IsLimitEnabled(b2JointId jointId);

/// 获取转动关节的下限（弧度）
B2_API float b2RevoluteJoint_GetLowerLimit(b2JointId jointId);

/// 获取转动关节的上限（弧度）
B2_API float b2RevoluteJoint_GetUpperLimit(b2JointId jointId);

/// 设置转动关节的上下限（弧度）
B2_API void b2RevoluteJoint_SetLimits(b2JointId jointId, float lower, float upper);

/// 启用/禁用转动关节的马达
B2_API void b2RevoluteJoint_EnableMotor(b2JointId jointId, bool enableMotor);

/// 转动关节的马达是否启用？
B2_API bool b2RevoluteJoint_IsMotorEnabled(b2JointId jointId);

/// 设置转动关节马达的速度（弧度/秒）
B2_API void b2RevoluteJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);

/// 获取转动关节马达的速度（弧度/秒）
B2_API float b2RevoluteJoint_GetMotorSpeed(b2JointId jointId);

/// 获取转动关节当前的马达扭矩，通常以牛顿·米为单位
B2_API float b2RevoluteJoint_GetMotorTorque(b2JointId jointId);

/// 设置转动关节的最大马达扭矩，通常以牛顿·米为单位
B2_API void b2RevoluteJoint_SetMaxMotorTorque(b2JointId jointId, float torque);

/// 获取转动关节的最大马达扭矩，通常以牛顿·米为单位
B2_API float b2RevoluteJoint_GetMaxMotorTorque(b2JointId jointId);

/**@}*/

/**
 * @defgroup weld_joint Weld Joint
 * @brief 焊接关节完全约束两个物体之间的相对变换，同时允许弹簧性
 *
 * 焊接关节约束两个物体之间的相对旋转和平移。旋转和平移
 * 都可以具有阻尼弹簧。
 *
 * @note 焊接关节的精度受到求解器精度的限制。长链的焊接关节可能会弯曲。
 * @{
 */

/// 创建一个焊接关节
/// @see b2WeldJointDef 获取详细信息
B2_API b2JointId b2CreateWeldJoint(b2WorldId worldId, const b2WeldJointDef *def);

/// 获取焊接关节的参考角度（弧度）
B2_API float b2WeldJoint_GetReferenceAngle(b2JointId jointId);

/// 设置焊接关节的参考角度（弧度），必须在[-pi, pi]范围内。
B2_API void b2WeldJoint_SetReferenceAngle(b2JointId jointId, float angleInRadians);

/// 设置焊接关节的线性刚度（赫兹）。0表示刚性。
B2_API void b2WeldJoint_SetLinearHertz(b2JointId jointId, float hertz);

/// 获取焊接关节的线性刚度（赫兹）
B2_API float b2WeldJoint_GetLinearHertz(b2JointId jointId);

/// 设置焊接关节的线性阻尼比（无量纲）
B2_API void b2WeldJoint_SetLinearDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取焊接关节的线性阻尼比（无量纲）
B2_API float b2WeldJoint_GetLinearDampingRatio(b2JointId jointId);

/// 设置焊接关节的角度刚度（赫兹）。0表示刚性。
B2_API void b2WeldJoint_SetAngularHertz(b2JointId jointId, float hertz);

/// 获取焊接关节的角度刚度（赫兹）
B2_API float b2WeldJoint_GetAngularHertz(b2JointId jointId);

/// 设置焊接关节的角度阻尼比（无量纲）
B2_API void b2WeldJoint_SetAngularDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取焊接关节的角度阻尼比（无量纲）
B2_API float b2WeldJoint_GetAngularDampingRatio(b2JointId jointId);

/** @} */

/**
 * @defgroup wheel_joint 车轮关节
 * 车轮关节可用于模拟车辆上的车轮。
 *
 * 车轮关节限制了物体 B 沿着物体 A 的局部轴线移动。物体 B 可以自由旋转。支持线性弹簧、线性限制和旋转马达。
 *
 * @{
 */

/// 创建一个车轮关节
/// @see b2WheelJointDef 获取详细信息
B2_API b2JointId b2CreateWheelJoint(b2WorldId worldId, const b2WheelJointDef *def);

/// 启用/禁用车轮关节弹簧
B2_API void b2WheelJoint_EnableSpring(b2JointId jointId, bool enableSpring);

/// 车轮关节弹簧是否已启用？
B2_API bool b2WheelJoint_IsSpringEnabled(b2JointId jointId);

/// 设置车轮关节的弹簧刚度（赫兹）
B2_API void b2WheelJoint_SetSpringHertz(b2JointId jointId, float hertz);

/// 获取车轮关节的弹簧刚度（赫兹）
B2_API float b2WheelJoint_GetSpringHertz(b2JointId jointId);

/// 设置车轮关节的弹簧阻尼比，单位为无量纲
B2_API void b2WheelJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);

/// 获取车轮关节的弹簧阻尼比，单位为无量纲
B2_API float b2WheelJoint_GetSpringDampingRatio(b2JointId jointId);

/// 启用/禁用车轮关节限制
B2_API void b2WheelJoint_EnableLimit(b2JointId jointId, bool enableLimit);

/// 车轮关节限制是否已启用？
B2_API bool b2WheelJoint_IsLimitEnabled(b2JointId jointId);

/// 获取车轮关节的下限
B2_API float b2WheelJoint_GetLowerLimit(b2JointId jointId);

/// 获取车轮关节的上限
B2_API float b2WheelJoint_GetUpperLimit(b2JointId jointId);

/// 设置车轮关节限制
B2_API void b2WheelJoint_SetLimits(b2JointId jointId, float lower, float upper);

/// 启用/禁用车轮关节马达
B2_API void b2WheelJoint_EnableMotor(b2JointId jointId, bool enableMotor);

/// 车轮关节马达是否已启用？
B2_API bool b2WheelJoint_IsMotorEnabled(b2JointId jointId);

/// 设置车轮关节马达的速度（弧度每秒）
B2_API void b2WheelJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);

/// 获取车轮关节马达的速度（弧度每秒）
B2_API float b2WheelJoint_GetMotorSpeed(b2JointId jointId);

/// 设置车轮关节的最大马达扭矩，通常为牛顿·米
B2_API void b2WheelJoint_SetMaxMotorTorque(b2JointId jointId, float torque);

/// 获取车轮关节的最大马达扭矩，通常为牛顿·米
B2_API float b2WheelJoint_GetMaxMotorTorque(b2JointId jointId);

/// 获取车轮关节当前的马达扭矩，通常为牛顿·米
B2_API float b2WheelJoint_GetMotorTorque(b2JointId jointId);

/**@}*/

/**@}*/
