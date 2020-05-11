/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "vector2.h"
#include "vector3.h"

template <typename T>
Vec2<T>  transform_vector_from_basis_2D(const Vec2<T>& v_localSpace, const Vec2<T> basisVectors[2], const Vec2<T>& system_origin)
{
  return system_origin + (v_localSpace.x * basisVectors[0]) + (v_localSpace.y * basisVectors[1]);
}

template <typename T>
Vec3<T>  transform_vector_from_basis_3D(const Vec3<T>& v_localSpace, const Vec3<T> basisVectors[3], const Vec3<T>& system_origin)
{
  return system_origin + (v_localSpace.x * basisVectors[0]) + (v_localSpace.y * basisVectors[1]) + (v_localSpace.z & basisVectors[2]);
}