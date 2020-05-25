/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// Ray - represents a directed line segment, with origin and extent
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Ray
{
  Ray();
  Ray(EZero);
  Ray(EUninitialized);
  Ray(const Vec3<T>& _origin, const Vec3<T>& _extent);

  Vec3<T> origin, extent; // extent is both direction and length, relative to origin
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Ray<T>::Ray()
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Ray<T>::Ray(EZero)
  : origin(EZero::Constructor)
  , extent(EZero::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Ray<T>::Ray(EUninitialized)
  : origin(EUninitialized::Constructor)
  , extent(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Ray<T>::Ray(const Vec3<T>& _origin, const Vec3<T>& _extent)
  : origin(_origin)
  , extent(_extent)
{
}

///////////////////////////////////////////////////////////////////////
typedef Ray<fworld> Rayw;
typedef Ray<flocal> Rayl;
