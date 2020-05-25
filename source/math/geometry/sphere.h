/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// Sphere
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Sphere
{
  Sphere();
  Sphere(EZero);
  Sphere(EUninitialized);
  Sphere(const Vec3<T>& _center, T _radius);

  inline T Diameter() const;
  inline T SurfaceArea() const;
  inline T Volume() const;

  Vec3<T> center;
  T radius;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Sphere<T>::Sphere()
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Sphere<T>::Sphere(EZero)
  : center(EZero::Constructor)
  , radius(EZero::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Sphere<T>::Sphere(EUninitialized)
  : center(EUninitialized::Constructor)
  , radius(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Sphere<T>::Sphere(const Vec3<T>& _center, T _radius)
  : center(_center)
  , radius(_radius)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Sphere<T>::Diameter() const
{
  return radius * 2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Sphere<T>::SurfaceArea() const
{
  static constexpr T kFourPi = kPi<T> * T(4);
  return kFourPi * radius * radius;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Sphere<T>::Volume() const
{
  static constexpr T kFourThirdsPi = kPi<T> * T(4) / T(3);
  return kFourThirdsPi * radius * radius * radius;
}

///////////////////////////////////////////////////////////////////////
typedef Sphere<fworld> Spherew;
typedef Sphere<flocal> Spherel;
