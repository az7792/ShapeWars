// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

#include "base.h"

#include <stdint.h>

/**
 * @defgroup id Ids
 * 这些id作为Box2D内部对象的句柄。它们应被视为不透明数据，并通过值传递。
 * 如果您只需要id类型而不需要整个Box2D API，请包含此头文件。
 * 所有id初始化为零时视为null。
 *
 * 例如在C++中：
 *
 * @code{.cxx}
 * b2WorldId worldId = {};
 * @endcode
 *
 * 或者在C中：
 *
 * @code{.c}
 * b2WorldId worldId = {0};
 * @endcode
 *
 * 这两种情况都视为null。
 *
 * @warning 请勿使用这些id的内部实现。它们可能会发生变化。id应视为不透明对象。
 * @warning 您应该使用id访问Box2D中的对象。请勿访问src文件夹中的文件。此类用法不被支持。
 * @{
 */

/// World id引用一个世界实例。应将其视为不透明句柄。
typedef struct b2WorldId
{
	uint16_t index1;
	uint16_t revision;
} b2WorldId;

/// Body id引用一个物体实例。应将其视为不透明句柄。
typedef struct b2BodyId
{
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2BodyId;

/// Shape id引用一个形状实例。应将其视为不透明句柄。
typedef struct b2ShapeId
{
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2ShapeId;

/// Chain id引用一个链实例。应将其视为不透明句柄。
typedef struct b2ChainId
{
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2ChainId;

/// Joint id引用一个关节实例。应将其视为不透明句柄。
typedef struct b2JointId
{
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2JointId;

/// 使用这些将您的标识符设置为null。
/// 您也可以使用零初始化来获得null。
static const b2WorldId b2_nullWorldId = B2_ZERO_INIT;
static const b2BodyId b2_nullBodyId = B2_ZERO_INIT;
static const b2ShapeId b2_nullShapeId = B2_ZERO_INIT;
static const b2ChainId b2_nullChainId = B2_ZERO_INIT;
static const b2JointId b2_nullJointId = B2_ZERO_INIT;

/// 宏，用于确定任何id是否为null。
#define B2_IS_NULL( id ) ( id.index1 == 0 )

/// 宏，用于确定任何id是否非null。
#define B2_IS_NON_NULL( id ) ( id.index1 != 0 )

/// 比较两个id是否相等。对于b2WorldId无效。
#define B2_ID_EQUALS( id1, id2 ) ( id1.index1 == id2.index1 && id1.world0 == id2.world0 && id1.revision == id2.revision )

/// 将物体id存储到uint64_t。
B2_INLINE uint64_t b2StoreBodyId( b2BodyId id )
{
	return ( (uint64_t)id.index1 << 32 ) | ( (uint64_t)id.world0 ) << 16 | (uint64_t)id.revision;
}

/// 将uint64_t加载到物体id中。
B2_INLINE b2BodyId b2LoadBodyId( uint64_t x )
{
	b2BodyId id = { (int32_t)( x >> 32 ), (uint16_t)( x >> 16 ), (uint16_t)( x ) };
	return id;
}

/// 将形状id存储到uint64_t。
B2_INLINE uint64_t b2StoreShapeId( b2ShapeId id )
{
	return ( (uint64_t)id.index1 << 32 ) | ( (uint64_t)id.world0 ) << 16 | (uint64_t)id.revision;
}

/// 将uint64_t加载到形状id中。
B2_INLINE b2ShapeId b2LoadShapeId( uint64_t x )
{
	b2ShapeId id = { (int32_t)( x >> 32 ), (uint16_t)( x >> 16 ), (uint16_t)( x ) };
	return id;
}

/// 将链id存储到uint64_t。
B2_INLINE uint64_t b2StoreChainId( b2ChainId id )
{
	return ( (uint64_t)id.index1 << 32 ) | ( (uint64_t)id.world0 ) << 16 | (uint64_t)id.revision;
}

/// 将uint64_t加载到链id中。
B2_INLINE b2ChainId b2LoadChainId( uint64_t x )
{
	b2ChainId id = { (int32_t)( x >> 32 ), (uint16_t)( x >> 16 ), (uint16_t)( x ) };
	return id;
}

/// 将关节id存储到uint64_t。
B2_INLINE uint64_t b2StoreJointId( b2JointId id )
{
	return ( (uint64_t)id.index1 << 32 ) | ( (uint64_t)id.world0 ) << 16 | (uint64_t)id.revision;
}

/// 将uint64_t加载到关节id中。
B2_INLINE b2JointId b2LoadJointId( uint64_t x )
{
	b2JointId id = { (int32_t)( x >> 32 ), (uint16_t)( x >> 16 ), (uint16_t)( x ) };
	return id;
}

/**@}*/