/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

///////////////////////////////////////////////////////////////////////
// Matrix 2x2
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Matrix22
{
  Matrix22();
  Matrix22(EZero);
  Matrix22(EIdentity);
  Matrix22(EUninitialized);
  Matrix22(T _m11, T _m12, T _m21, T _m22);

  inline bool operator==(const Matrix22& rhs) const;

  inline void Transpose();
  inline Matrix22<T> Transposed() const;

  T m11, m12;
  T m21, m22;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22()
  : m11(T{})
  , m12(T{})
  , m21(T{})
  , m22(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(EZero)
  : m11(0)
  , m12(0)
  , m21(0)
  , m22(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(EIdentity)
  : m11(1)
  , m12(0)
  , m21(0)
  , m22(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(T _m11, T _m12, T _m21, T _m22)
  : m11(_m11)
  , m12(_m12)
  , m21(_m21)
  , m22(_m22)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix22<T>::operator==(const Matrix22& rhs) const
{
  return m11 == rhs.m11 && m12 == rhs.m12 && m21 == rhs.m21 && m22 == rhs.m22;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Transpose()
{
  T _m12 = m12;
  m12 = m21;
  m21 = _m12;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::Transposed() const
{
  return Matrix22<T>(m11, m21, m12, m22);
}

///////////////////////////////////////////////////////////////////////
typedef Matrix22<fworld> Matrix22w;
typedef Matrix22<flocal> Matrix22l;

