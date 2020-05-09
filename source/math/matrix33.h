/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"
#include "vector.h"

///////////////////////////////////////////////////////////////////////
// Matrix 3x3
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Matrix33
{
  Matrix33();
  Matrix33(EZero);
  Matrix33(EIdentity);
  Matrix33(EUninitialized);
  Matrix33(
    T _m11, T _m12, T _m13, 
    T _m21, T _m22, T _m23,
    T _m31, T _m32, T _m33);

  inline bool operator==(const Matrix33& rhs) const;
  inline bool operator!=(const Matrix33& rhs) const;

  T m11, m12, m13;
  T m21, m22, m23;
  T m31, m32, m33;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33()
  : m11(T{}), m12(T{}), m13(T{})
  , m21(T{}), m22(T{}), m23(T{})
  , m31(T{}), m32(T{}), m33(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33(EZero)
  : m11(0), m12(0), m13(0)
  , m21(0), m22(0), m23(0)
  , m31(0), m32(0), m33(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33(EIdentity)
  : m11(1), m12(0), m13(0)
  , m21(0), m22(1), m23(0)
  , m31(0), m32(0), m33(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33(
  T _m11, T _m12, T _m13,
  T _m21, T _m22, T _m23,
  T _m31, T _m32, T _m33)
  : m11(_m11), m12(_m12), m13(_m13)
  , m21(_m21), m22(_m22), m23(_m23)
  , m31(_m31), m32(_m32), m33(_m33)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix33<T>::operator==(const Matrix33& rhs) const
{
  return 
    m11 == rhs.m11 && m12 == rhs.m12 && m13 == rhs.m13 &&
    m21 == rhs.m21 && m22 == rhs.m22 && m23 == rhs.m23 &&
    m31 == rhs.m31 && m32 == rhs.m32 && m33 == rhs.m33;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix33<T>::operator!=(const Matrix33& rhs) const
{
  return 
    m11 != rhs.m11 || m12 != rhs.m12 || m13 != rhs.m13 ||
    m21 != rhs.m21 || m22 != rhs.m22 || m23 != rhs.m23 ||
    m31 != rhs.m31 || m32 != rhs.m32 || m33 != rhs.m33;
}
