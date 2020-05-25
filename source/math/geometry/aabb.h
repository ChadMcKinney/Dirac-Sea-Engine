/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <algorithm>
#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// AABB - axis aligned bounding box
///////////////////////////////////////////////////////////////////////
template <typename T>
struct AABB
{
	AABB();
	AABB(EZero);
	AABB(EUninitialized);
	AABB(EEmpty); // useful for starting with nothing and expanding an AABB procedurally using Add
	AABB(const Vec3<T>& _minmax); // sets min and max to same point
	AABB(const Vec3<T>& _min, const Vec3<T>& _max); // asserts all elements of min <= max

	inline bool operator==(const AABB<T>& rhs) const;
	inline bool operator!=(const AABB<T>& rhs) const;
	inline bool IsEquivalent(const AABB<T>& rhs, T epsilon) const;

	// merges point into this, selecting min/max elements amongst the aabb and v
	inline void MergePoint(const Vec3<T>& v);

	// merges aabb into this, selecting min/max elements amongsth both aabbs
	inline void MergeAABB(const AABB<T>& aabb);

	// moves min and max by offset
	inline void Move(const Vec3<T>& offset);

	// will create AABB with the min and max elements among both points
	inline static AABB CreateAABBFromPair(const Vec3<T>& a, const Vec3<T>& b);

	// will create ABB with the min and max elements among all points
	inline static AABB CreateAABBFromPoints(const Vec3<T> points[], size_t numPoints);

	Vec3<T> min, max;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB()
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB(EZero)
	: min(EZero::Constructor)
	, max(EZero::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB(EUninitialized)
	: min(EUninitialized::Constructor)
	, max(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB(EEmpty)
	: min(kFMax<T>, kFMax<T>, kFMax<T>)
	, max(kFMin<T>, kFMin<T>, kFMin<T>)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB(const Vec3<T>& _minmax)
	: min(_minmax)
	, max(_minmax)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
AABB<T>::AABB(const Vec3<T>& _min, const Vec3<T>& _max)
	: min(_min)
	, max(_max)
{
	assert(_min.x <= _max.x && _min.y <= _max.y && _min.z <= _max.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool AABB<T>::operator==(const AABB<T>& rhs) const
{
	return min == rhs.min && max == rhs.max;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool AABB<T>::operator!=(const AABB<T>& rhs) const
{
	return min != rhs.min || max != rhs.max;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool AABB<T>::IsEquivalent(const AABB<T>& rhs, T epsilon) const
{
	return min.IsEquivalent(rhs.min, epsilon) && max.IsEquivalent(rhs.max, epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void AABB<T>::MergePoint(const Vec3<T>& v)
{
	min.SelectMinBetween(v);
	max.SelectMaxBetween(v);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void AABB<T>::MergeAABB(const AABB<T>& aabb)
{
	min.SelectMinBetween(aabb.min);
	max.SelectMaxBetween(aabb.max);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void AABB<T>::Move(const Vec3<T>& offset)
{
	min += offset;
	max += offset;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline AABB<T> AABB<T>::CreateAABBFromPair(const Vec3<T>& a, const Vec3<T>& b)
{
	AABB<T> aabb(a);
	aabb.MergePoint(b);
	return aabb;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline AABB<T> AABB<T>::CreateAABBFromPoints(const Vec3<T> points[], size_t numPoints)
{
	assert(numPoints > 0);
	AABB<T> aabb(points[0]);
	for (size_t i = 1; i < numPoints; ++i)
	{
		aabb.MergePoint(points[i]);
	}

	return aabb;
}

///////////////////////////////////////////////////////////////////////
typedef AABB<fworld> AABBw;
typedef AABB<flocal> AABBl;
