/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// Polar
///////////////////////////////////////////////////////////////////////

template <typename T>
struct Polar
{
  Polar();
  Polar(EZero);
  Polar(EIdentity);
  Polar(EUninitialized);
  Polar(T _yaw, T _pitch);

  inline operator+(const Polar& rhs) const;

  T yaw;
  T pitch;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Polar<T>::Polar()
  : yaw(T{})
  , pitch(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Polar<T>::Polar(EZero)
  : yaw(0)
  , pitch(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Polar<T>::Polar(EIdentity)
  : yaw(T{})
  , pitch(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Polar<T>::Polar(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Polar<T>::Polar(T _yaw, T _pitch)
  : yaw(_yaw)
  , pitch(_pitch)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Polar<T>::ToCartesian() const
{

}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Polar<T> Polar<T>::CreateFromCartesion(const Vec3<T>& v)
{

}
