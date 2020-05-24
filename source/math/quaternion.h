/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "vector3.h"
#include "types.h"

 ///////////////////////////////////////////////////////////////////////
 // Quaternion
 ///////////////////////////////////////////////////////////////////////
template <typename T>
struct Quaternion
{
	Quaternion();
	Quaternion(EZero);
	Quaternion(EIdentity);
	Quaternion(EUninitialized);
	Quaternion(T _x, T _y, T _z, T _w);

	inline bool operator==(const Quaternion& rhs) const;
	inline bool operator!=(const Quaternion& rhs) const;
	inline bool IsEquivalent(const Quaternion& rhs, T epsilon) const;

	inline void operator*=(const Quaternion& rhs);
	inline Quaternion operator*(const Quaternion& rhs) const;

	inline T Magnitude() const;
	inline bool IsUnit(T epsilon) const;

	inline void Negate();
	inline Quaternion Negated() const;

	inline void Invert(); // Assumes unit quaternion!
	inline Quaternion Inverted() const; // Assumes unit quaternion!

	inline void SetAxisAngle(const Vec3<T>& axis, T radians);
	inline static Quaternion CreateAxisAngle(const Vec3<T> axis, T radians);

	inline void SetRotationX(T radians);
	inline static Quaternion CreateRotationX(T radians);

	inline void SetRotationY(T radians);
	inline static Quaternion CreateRotationY(T radians);

	inline void SetRotationZ(T radians);
	inline static Quaternion CreateRotationZ(T radians);

	T x, y, z, w;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion()
	: x(T{})
	, y(T{})
	, z(T{})
	, w(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EZero)
	: x(0)
	, y(0)
	, z(0)
	, w(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EIdentity)
	: x(0)
	, y(0)
	, z(0)
	, w(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(T _x, T _y, T _z, T _w)
	: x(_x)
	, y(_y)
	, z(_z)
	, w(_w)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::operator==(const Quaternion& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::operator!=(const Quaternion& rhs) const
{
	return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::IsEquivalent(const Quaternion& rhs, T epsilon) const
{
	return
		(abs(x - rhs.x) < epsilon) &&
		(abs(y - rhs.y) < epsilon) &&
		(abs(z - rhs.z) < epsilon) &&
		(abs(w - rhs.w) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Quaternion<T>::Magnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::operator*=(const Quaternion& rhs)
{
	*this = *this * rhs;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.x = (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
	q.y = (w * rhs.y) + (y * rhs.w) + (z * rhs.x) - (x * rhs.z);
	q.z = (w * rhs.z) + (z * rhs.w) + (x * rhs.y) - (y * rhs.x);
	q.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::IsUnit(T epsilon) const
{
	return (T(1) - Magnitude()) < epsilon;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Negated() const
{
	return Quaternion<T>(-x, -y, -z, -w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Invert()
{
	assert(IsUnit(epsilon<T>()));
	x = -x;
	y = -y;
	z = -z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Inverted() const
{
	assert(IsUnit(epsilon<T>()));
	return Quaternion<T>(-x, -y, -z, w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetAxisAngle(const Vec3<T>& axis, T radians)
{
	assert(axis.IsUnit(epsilon<T>()));
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = axis.x * sinHalfTheta;
	y = axis.y * sinHalfTheta;
	z = axis.z * sinHalfTheta;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateAxisAngle(const Vec3<T> axis, T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetAxisAngle(axis, radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationX(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = sinHalfTheta;
	y = 0;
	z = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationX(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationX(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationY(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = 0;
	y = sinHalfTheta;
	z = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationY(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationY(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationZ(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = 0;
	y = 0;
	z = sinHalfTheta;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationZ(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationZ(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
// Post-Multiply: rotate V by q rotation
template <typename T>
inline Vec3<T> operator*(const Quaternion<T>& q, const Vec3<T>& v)
{
	const Quaternion<T> qv(v.x, v.y, v.z, 0);
	const Quaternion<T> qv2 = q * qv * q.Inverted();
	return Vec3<T>(qv2.x, qv2.y, qv2.z);
}

///////////////////////////////////////////////////////////////////////
// Pre-Multiply: rotate V by inverstion of q rotation!
template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v, const Quaternion<T>& q)
{
	const Quaternion<T> q2 = q.Inverted();
	const Quaternion<T> qv(v.x, v.y, v.z, 0);
	const Quaternion<T> qv2 = q2 * qv * q;
	return Vec3<T>(qv2.x, qv2.y, qv2.z);
}

///////////////////////////////////////////////////////////////////////
typedef Quaternion<fworld> Quaternionw;
typedef Quaternion<flocal> Quaternionl;
