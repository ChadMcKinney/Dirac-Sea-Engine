/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

namespace triangle
{

template <typename T>
void GetBarycentricFromCartesian(
    const Vec3<T>& v1, const Vec3<T>& v2, const Vec3<T>& v3,
    const Vec3<T>& p,
    T& b1, T& b2, T& b3)
{
    const Vec3<T> e1 = v3 - v2;
    const Vec3<T> e2 = v1 - v3;
    const Vec3<T> e3 = v2 - v1;
    const Vec3<T> cross = e1.Cross(e2);
    const Vec3<T> normal = cross.Normalized();
    assert(normal.IsUnit(epsilon<T>()));

    const T areaT = cross.Dot(normal);
    const T recipAreaT = T(1) / areaT;
    b1 = e1.Cross(p - v3).Dot(normal) * recipAreaT;
    b2 = e2.Cross(p - v1).Dot(normal) * recipAreaT;
    b3 = e3.Cross(p - v2).Dot(normal) * recipAreaT;
}

template <typename T>
Vec3<T> GetCartesianFromBarycentric(
    const Vec3<T>& v1, const Vec3<T>& v2, const Vec3<T>& v3, T b1, T b2, T b3)
{
    return v1.Scaled(b1) + v2.Scaled(b2) + v3.Scaled(b3);
}

} // triangle namespace
