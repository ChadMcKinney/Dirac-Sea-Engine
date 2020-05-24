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

  inline T Magnitude() const;
  inline bool IsUnit(T epsilon) const;

  inline void Negate();
  inline Quaternion<T> Negated() const;

  inline void Invert(); // Assumes unit quaternion!
  inline Quaternion Inverted() const; // Assumes unit quaternion!

  inline void SetAxisAngle(const Vec3<T>& axis, T radians);
  inline static Quaternion<T> CreateAxisAngle(const Vec3<T> axis, T radians);

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
  return Quaternion<T>(-x, -y, -z, w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetAxisAngle(const Vec3<T>& axis, T radians)
{
  assert(axis.IsUnit(epsilon<T>()));
  T halfTheta = radians * T(0.5);
  T sinHalfTheta = std::sin(halfTheta);
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
inline Vec3<T> operator*(const Quaternion<T>& q, const Vec3<T>& v)
{
  Quaternion q2(v.x, v.y, v.z, 0);
}

///////////////////////////////////////////////////////////////////////
typedef Quaternion<fworld> Quaternionw;
typedef Quaternion<flocal> Quaternionl;
