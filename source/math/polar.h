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
