/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

///////////////////////////////////////////////////////////////////////
// Vec2
template <typename T>
struct Vec2
{
  Vec2();
  Vec2(T _x, T _y);

  Vec2 operator+(const Vec2& rhs) const;
  Vec2 operator-(const Vec2& rhs) const;

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
Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const
{
  return Vec2(x + rhs.x, y + rhs.y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const
{
  return Vec2(x - rhs.x, y - rhs.y);
}

///////////////////////////////////////////////////////////////////////
typedef Vec2<fworld> Vec2w;
typedef Vec2<flocal> Vec2l;

///////////////////////////////////////////////////////////////////////
// Vec3
template <typename T>
struct Vec3
{
  Vec3();
  Vec3(T _x, T _y, T _z);

  Vec3 operator+(const Vec3& rhs) const;
  Vec3 operator-(const Vec3& rhs) const;

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
Vec3<T>::Vec3(T _x, T _y, T _z)
	: x(_x)
	, y(_y)
	, z(_z)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

///////////////////////////////////////////////////////////////////////
typedef Vec3<fworld> Vec3w;
typedef Vec3<flocal> Vec3l;
