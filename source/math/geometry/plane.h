/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// Plane
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Plane
{
	Plane();
	Plane(EZero);
	Plane(EUninitialized);
	Plane(const Vec3<T>& _point, const Vec3<T>& _normal);
	Plane(const Vec3<T>& _normal, T _distance);
	Plane(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c); // assumes clockwise ordering

	inline bool operator==(const Plane& rhs) const;
	inline bool operator!=(const Plane& rhs) const;
	inline bool IsEquivalent(const Plane& rhs, T epsilon) const;

	inline T Distance(const Vec3<T>& v) const;

	Vec3<T> normal;
	T distance;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane()
	: distance(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane(EZero)
	: normal(EZero::Constructor)
	, distance(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane(EUninitialized)
	: normal(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane(const Vec3<T>& _normal, T _distance)
	: normal(_normal)
	, distance(_distance)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane(const Vec3<T>& _point, const Vec3<T>& _normal)
	: normal(_normal)
{
	distance = point.Dot(normal);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Plane<T>::operator==(const Plane& rhs) const
{
	return normal == rhs.normal && distance == rhs.distance;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Plane<T>::operator!=(const Plane& rhs) const
{
	return normal != rhs.normal || distance != rhs.distance;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Plane<T>::IsEquivalent(const Plane& rhs, T epsilon) const
{
	return normal.IsEquivalent(rhs.normal, epsilon) && (abs(distance - rhs.distance) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Plane<T>::Distance(const Vec3<T>& v) const
{
	return v.Dot(normal) - distance;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Plane<T>::Plane(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c)
{
	const Vec3<T> dBA = b - a;
	const Vec3<T> dCB = c - b;
	normal = dBA.Cross(dCB).SafeNormalized();
	distance = a.Dot(normal);
}

///////////////////////////////////////////////////////////////////////
typedef Plane<fworld> Planew;
typedef Plane<flocal> Planel;
