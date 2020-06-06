/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

#include <cmath>

///////////////////////////////////////////////////////////////////////
// Vec4
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Vec4
{
  Vec4();
  Vec4(EZero);
  Vec4(EUninitialized);
  Vec4(T _x, T _y, T _z, T _w);

  inline Vec4 operator+(const Vec4& rhs) const;
  inline Vec4 operator-(const Vec4& rhs) const;
  inline bool operator==(const Vec4& rhs) const;
  inline bool operator!=(const Vec4& rhs) const;

  inline bool IsEquivalent(const Vec4& rhs, T epsilon) const;
  inline bool IsZero(T epsilon) const;
  inline bool IsUnit(T epsilon) const;

  inline void Scale(T s);
  inline Vec4 Scaled(T s) const;

  inline void Negate();
  inline Vec4 Negated() const;

  inline T Magnitude() const;
  inline T SqrMagnitude() const;

  inline T Distance(const Vec4<T>& rhs) const;
  inline T SqrDistance(const Vec4<T>& rhs) const;

  inline void Normalize();
  inline void SafeNormalize();
  inline Vec4<T> Normalized() const;
  inline Vec4<T> SafeNormalized() const;

  inline T Dot(const Vec4& rhs) const;

  T x, y, z, w;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4()
  : x(T{})
  , y(T{})
  , z(T{})
  , w(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(EZero)
  : x(0)
  , y(0)
  , z(0)
  , w(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec4<T>::Vec4(T _x, T _y, T _z, T _w)
  : x(_x)
  , y(_y)
  , z(_z)
  , w(_w)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::operator+(const Vec4& rhs) const
{
  return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::operator-(const Vec4& rhs) const
{
  return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec4<T>::operator==(const Vec4& rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec4<T>::operator!=(const Vec4& rhs) const
{
  return x != rhs.x && y != rhs.y && z != rhs.z && w != rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec4<T>::IsEquivalent(const Vec4& rhs, T epsilon) const
{
  return (abs(x - rhs.x) < epsilon) && 
         (abs(y - rhs.y) < epsilon) &&
         (abs(z - rhs.z) < epsilon) &&
         (abs(w - rhs.w) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec4<T>::IsZero(T epsilon) const
{
  return x == 0 && y == 0 && z == 0 && w == 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec4<T>::IsUnit(T epsilon) const
{
  return !IsZero() && IsEquivalent(Normalized(), epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec4<T>::Scale(T s)
{
  x *= s;
  y *= s;
  z *= s;
  w *= s;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::Scaled(T s) const
{
  return Vec4<T>(x * s, y * s, z * s, w * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec4<T>::Negate()
{
  x = -x;
  y = -y;
  z = -z;
  w = -w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::Negated() const
{
  return Vec4<T>(-x, -y, -z, -w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec4<T>::Magnitude() const
{
  return std::sqrt(SqrMagnitude());
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec4<T>::SqrMagnitude() const
{
  return (x * x) + (y * y) + (z * z) + (w * w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec4<T>::Distance(const Vec4<T>& rhs) const
{
  return std::sqrt(SqrDistance(rhs));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec4<T>::SqrDistance(const Vec4<T>& rhs) const
{
  Vec4<T> d(rhs.x - x, rhs.y - y, rhs.z - z, rhs.w - w);
  return d.SqrMagnitude();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec4<T>::Normalize()
{
  T magnitude = Magnitude();
  x /= magnitude;
  y /= magnitude;
  z /= magnitude;
  w /= magnitude;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec4<T>::SafeNormalize()
{
  T magnitude = Magnitude();
  if (magnitude > epsilon<T>())
  {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
  }
  else
  {
    x = y = z = w = 0;
  }
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::Normalized() const
{
  Vec4<T> v(*this);
  v.Normalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Vec4<T>::SafeNormalized() const
{
  Vec4<T> v(*this);
  v.SafeNormalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec4<T>::Dot(const Vec4& rhs) const
{
  return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
}

///////////////////////////////////////////////////////////////////////
typedef Vec4<fworld> Vec4w;
typedef Vec4<flocal> Vec4l;
typedef Vec4<int32_t> Vec4i;
typedef Vec4<uint32_t> Vec4u;

///////////////////////////////////////////////////////////////////////
typedef Vec4w Point4w;
typedef Vec4l Point4l;
typedef Vec4i Point4i;
typedef Vec4u Point4u;

