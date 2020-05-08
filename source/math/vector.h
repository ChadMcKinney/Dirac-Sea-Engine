/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

#include <cmath>

// TODO: Cross Product etc...

// TO DO: SIMD SUPPORT!

///////////////////////////////////////////////////////////////////////
// Vec2
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Vec2
{
  Vec2();
  Vec2(EZero);
  Vec2(EUninitialized);
  Vec2(T _x, T _y);

  inline Vec2 operator+(const Vec2& rhs) const;
  inline Vec2 operator-(const Vec2& rhs) const;

  inline void Scale(T s);
  inline Vec2 Scaled(T s) const;

  inline T Magnitude() const;
  inline T SqrMagnitude() const;

  inline T Distance(const Vec2<T>& rhs) const;
  inline T SqrDistance(const Vec2<T>& rhs) const;

  inline void Normalize();
  inline void SafeNormalize();
  inline Vec2<T> Normalized() const;
  inline Vec2<T> SafeNormalized() const;

  inline T Dot(const Vec2& rhs) const;

  inline Vec2<T> ParallelProjection(const Vec2& rhs) const;
  inline Vec2<T> ParallelProjection_Safe(const Vec2& rhs) const;
  inline Vec2<T> PerpendicularProjection(const Vec2& rhs) const;
  inline Vec2<T> PerpendicularProjection_Safe(const Vec2& rhs) const;

	T x, y;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec2<T>::Vec2()
  : x(T{})
  , y(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec2<T>::Vec2(T _x, T _y)
	: x(_x)
	, y(_y)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec2<T>::Vec2(EZero)
  : x(0),
  , y(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec2<T>::Vec2(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const
{
  return Vec2(x + rhs.x, y + rhs.y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const
{
  return Vec2(x - rhs.x, y - rhs.y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec2<T>::Scale(T s)
{
  x *= s;
  y *= s;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::Scaled(T s) const
{
  return Vec2<T>(x * s, y * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::Magnitude() const
{
  return std::sqrt((x * x) + (y * y));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::SqrMagnitude() const
{
  return (x * x) + (y * y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::Distance(const Vec2<T>& rhs) const
{
  return std::sqrt(SqrDistance(rhs));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::SqrDistance(const Vec2<T>& rhs) const
{
  Vec2<T> d(rhs.x - x, rhs.y - y);
  return d.SqrMagnitude();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec2<T>::Normalize()
{
  const T magnitude = Magnitude();
  x /= magnitude;
  y /= magnitude;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec2<T>::SafeNormalize()
{
	const T magnitude = Magnitude();
	if (magnitude > epsilon<T>())
	{
		x /= magnitude;
		y /= magnitude;
	}
	else
	{
    x = y = 0;
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::Normalized() const
{
  Vec2<T> v(x, y);
  v.Normalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::SafeNormalized() const
{
  Vec2<T> v(x, y);
  v.SafeNormalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::Dot(const Vec2& rhs) const
{
  return (x * rhs.x) + (y * rhs.y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::ParallelProjection(const Vec2& rhs) const
{
  Vec2 n = Normalized();
  const T l = n.Dot(rhs);
  n.Scale(l);
  return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::ParallelProjection_Safe(const Vec2& rhs) const
{
  Vec2 n = SafeNormalized();
  const T l = n.Dot(rhs);
  n.Scale(l);
  return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::PerpendicularProjection(const Vec2& rhs) const
{
  return rhs - ParallelProjection(rhs);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::PerpendicularProjection_Safe(const Vec2& rhs) const
{
  return rhs - ParallelProjection_Safe(rhs);
}

///////////////////////////////////////////////////////////////////////
typedef Vec2<fworld> Vec2w;
typedef Vec2<flocal> Vec2l;
typedef Vec2<int32_t> Vec2i;
typedef Vec2<uint32_t> Vec2u;

///////////////////////////////////////////////////////////////////////
// Vec3
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Vec3
{
  Vec3();
  Vec3(EZero);
  Vec3(EUninitialized);
  Vec3(T _x, T _y, T _z);

  inline Vec3 operator+(const Vec3& rhs) const;
  inline Vec3 operator-(const Vec3& rhs) const;

  inline void Scale(T s);
  inline Vec3 Scaled(T s) const;

  inline T Magnitude() const;
  inline T SqrMagnitude() const;

  inline T Distance(const Vec3<T>& rhs) const;
  inline T SqrDistance(const Vec3<T>& rhs) const;

  inline void Normalize();
  inline void SafeNormalize();
  inline Vec3<T> Normalized() const;
  inline Vec3<T> SafeNormalized() const;

  inline T Dot(const Vec3& rhs) const;

  inline Vec3<T> ParallelProjection(const Vec3& rhs) const;
  inline Vec3<T> ParallelProjection_Safe(const Vec3& rhs) const;
  inline Vec3<T> PerpendicularProjection(const Vec3& rhs) const;
  inline Vec3<T> PerpendicularProjection_Safe(const Vec3& rhs) const;

  T x, y, z;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3() 
  : x(T{})
  , y(T{})
  , z(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(EZero)
	: x(0)
	, y(0)
	, z(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(T _x, T _y, T _z)
	: x(_x)
	, y(_y)
	, z(_z)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const
{
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::Scale(T s)
{
  x *= s;
  y *= s;
  z *= s;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Scaled(T s) const
{
  return Vec3<T>(x * s, y * s, z * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Magnitude() const
{
  return std::sqrt((x * x) + (y * y) + (z * z));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::SqrMagnitude() const
{
  return (x * x) + (y * y) + (z * z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Distance(const Vec3<T>& rhs) const
{
  return std::sqrt(SqrDistance(rhs));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::SqrDistance(const Vec3<T>& rhs) const
{
  Vec3<T> d(rhs.x - x, rhs.y - y, rhs.z - z);
  return d.SqrMagnitude();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::Normalize()
{
  const T magnitude = Magnitude();
  x /= magnitude;
  y /= magnitude;
  z /= magnitude;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::SafeNormalize()
{
	const T magnitude = Magnitude();
	if (magnitude > epsilon<T>())
	{
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}
	else
	{
		x = y = z = 0;
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Normalized() const
{
  Vec3<T> v(x, y, z);
  v.Normalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::SafeNormalized() const
{
  Vec3<T> v(x, y, z);
  v.SafeNormalize();
  return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Dot(const Vec3& rhs) const
{
  return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::ParallelProjection(const Vec3& rhs) const
{
  Vec3 n = Normalized();
  const T l = n.Dot(rhs);
  n.Scale(l);
  return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::ParallelProjection_Safe(const Vec3& rhs) const
{
  Vec3 n = SafeNormalized();
  const T l = n.Dot(rhs);
  n.Scale(l);
  return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::PerpendicularProjection(const Vec3& rhs) const
{
  return rhs - ParallelProjection(rhs);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::PerpendicularProjection_Safe(const Vec3& rhs) const
{
  return rhs - ParallelProjection_Safe(rhs);
}

///////////////////////////////////////////////////////////////////////
typedef Vec3<fworld> Vec3w;
typedef Vec3<flocal> Vec3l;
typedef Vec3<int32_t> Vec3i;
typedef Vec3<uint32_t> Vec3u;
