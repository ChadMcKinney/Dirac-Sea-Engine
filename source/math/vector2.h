/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

#include <cmath>

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
  inline bool operator==(const Vec2& rhs) const;
  inline bool operator!=(const Vec2& rhs) const;

  inline bool IsEquivalent(const Vec2& rhs, T epsilon) const;
  inline bool IsZero(T epsilon) const;
  inline bool IsUnit(T epsilon) const;

  inline void Scale(T s);
  inline Vec2 Scaled(T s) const;

  inline void Negate();
  inline Vec2 Negated() const;

  inline T Magnitude() const;
  inline T SqrMagnitude() const;

  inline T Distance(const Vec2<T>& rhs) const;
  inline T SqrDistance(const Vec2<T>& rhs) const;

  inline void Normalize();
  inline void SafeNormalize();
  inline Vec2<T> Normalized() const;
  inline Vec2<T> SafeNormalized() const;

  inline T Dot(const Vec2& rhs) const;

  inline T Angle(const Vec2& rhs) const;
  inline T Angle_Safe(const Vec2& rhs) const;

  inline Vec2<T> ParallelProjection(const Vec2& rhs) const;
  inline Vec2<T> ParallelProjection_Safe(const Vec2& rhs) const;
  inline Vec2<T> PerpendicularProjection(const Vec2& rhs) const;
  inline Vec2<T> PerpendicularProjection_Safe(const Vec2& rhs) const;

  static const Vec2<T> Zero;
  static const Vec2<T> Up;
  static const Vec2<T> Down;
  static const Vec2<T> Left;
  static const Vec2<T> Right;

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
  : x(0)
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
inline bool Vec2<T>::operator==(const Vec2& rhs) const
{
  return x == rhs.x && y == rhs.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec2<T>::operator!=(const Vec2& rhs) const
{
  return x != rhs.x || y != rhs.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec2<T>::IsEquivalent(const Vec2& rhs, T epsilon) const
{
  return (abs(x - rhs.x) < epsilon) && (abs(y - rhs.y) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec2<T>::IsZero(T epsilon) const
{
  return abs(x) < epsilon && abs(y) < epsilon;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec2<T>::IsUnit(T epsilon) const
{
  return !IsZero(epsilon) && IsEquivalent(Normalized(), epsilon);
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
inline void Vec2<T>::Negate()
{
  x = -x;
  y = -y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Vec2<T>::Negated() const
{
  return Vec2(-x, -y);
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
inline T Vec2<T>::Angle(const Vec2& rhs) const
{
  return std::acos(Normalized().Dot(rhs.Normalized()));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec2<T>::Angle_Safe(const Vec2& rhs) const
{
  return std::acos(SafeNormalized().Dot(rhs.SafeNormalized()));
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
template <typename T>
const Vec2<T> Vec2<T>::Zero(EZero::Constructor);
template <typename T>
const Vec2<T> Vec2<T>::Up(0, 1);
template <typename T>
const Vec2<T> Vec2<T>::Down(0, -1);
template <typename T>
const Vec2<T> Vec2<T>::Left(-1, 0);
template <typename T>
const Vec2<T> Vec2<T>::Right(1, 0);

///////////////////////////////////////////////////////////////////////
typedef Vec2<fworld> Vec2w;
typedef Vec2<flocal> Vec2l;
typedef Vec2<int32_t> Vec2i;
typedef Vec2<uint32_t> Vec2u;

///////////////////////////////////////////////////////////////////////
typedef Vec2w Point2w;
typedef Vec2l Point2l;
typedef Vec2i Point2i;
typedef Vec2u Point2u;

