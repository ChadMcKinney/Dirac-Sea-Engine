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

  inline void operator*=(const Matrix33& rhs);
  inline Matrix33<T> operator*(const Matrix33& rhs) const;

  inline Vec3<T> GetRow1() const;
  inline void SetRow1(const Vec3<T>& row1);

  inline Vec3<T> GetRow2() const;
  inline void SetRow2(const Vec3<T>& row2);

  inline Vec3<T> GetRow3() const;
  inline void SetRow3(const Vec3<T>& row3);

  inline Vec3<T> GetColumn1() const;
  inline void SetColumn1(const Vec3<T>& column1);

  inline Vec3<T> GetColumn2() const;
  inline void SetColumn2(const Vec3<T>& column2);

  inline Vec3<T> GetColumn3() const;
  inline void SetColumn3(const Vec3<T>& column3);

  inline void Transpose();
  inline Matrix33<T> Transposed() const;

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

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::operator*=(const Matrix33& rhs)
{
  T _m11 = (m11 * rhs.m11) + (m12 * rhs.m21) + (m13 * rhs.m31);
  T _m12 = (m11 * rhs.m12) + (m12 * rhs.m22) + (m13 * rhs.m32);
  T _m13 = (m11 * rhs.m13) + (m12 * rhs.m23) + (m13 * rhs.m33);

  T _m21 = (m21 * rhs.m11) + (m22 * rhs.m21) + (m23 * rhs.m31);
  T _m22 = (m21 * rhs.m12) + (m22 * rhs.m22) + (m23 * rhs.m32);
  T _m23 = (m21 * rhs.m13) + (m22 * rhs.m23) + (m23 * rhs.m33);

  T _m31 = (m31 * rhs.m11) + (m32 * rhs.m21) + (m33 * rhs.m31);
  T _m32 = (m31 * rhs.m12) + (m32 * rhs.m22) + (m33 * rhs.m32);
  T _m33 = (m31 * rhs.m13) + (m32 * rhs.m23) + (m33 * rhs.m33);

  m11 = _m11;
  m12 = _m12;
  m13 = _m13;

  m21 = _m21;
  m22 = _m22;
  m23 = _m23;

  m31 = _m31;
  m32 = _m32;
  m33 = _m33;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix33<T>::operator*(const Matrix33& rhs) const
{
  Matrix33<T> m(*this);
  m *= rhs;
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetRow1() const
{
  return Vec3<T>(m11, m12, m13);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetRow1(const Vec3<T>& row1)
{
  m11 = row1.x;
  m12 = row1.y;
  m13 = row1.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetRow2() const
{
  return Vec3<T>(m21, m22, m33);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetRow2(const Vec3<T>& row2)
{
  m21 = row.x;
  m22 = row.y;
  m23 = row.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetRow3() const
{
  return Vec3<T>(m31, m32, m33);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetRow3(const Vec3<T>& row3)
{
  m31 = row3.x;
  m32 = row3.y;
  m33 = row3.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetColumn1() const
{
  return Vec3<T>(m11, m21, m31);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetColumn1(const Vec3<T>& column1)
{
  m11 = column1.x;
  m21 = column1.y;
  m31 = column1.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetColumn2() const
{
  return Vec3<T>(m12, m22, m32);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetColumn2(const Vec3<T>& column2)
{
  m12 = column2.x;
  m22 = column2.y;
  m32 = column2.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix33<T>::GetColumn3() const
{
  return Vec3<T>(m13, m23, m33);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::SetColumn3(const Vec3<T>& column3)
{
  m13 = column3.x;
  m23 = column3.y;
  m33 = column3.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix33<T>::Transpose()
{
  // T _m11 = m11; // no change
  T _m12 = m21;
  T _m13 = m31;

  T _m21 = m12;
  // T _m22 = m22; // no change
  T _m23 = m32;

  T _m31 = m13;
  T _m32 = m23;
  // T _m33 = m33; // no change

  // m11 = _m11; // no change
  m12 = _m12;
  m13 = _m13;

  m21 = _m21;
  // m22 = _m22; // no change
  m23 = _m23;

  m31 = _m31;
  m32 = _m32;
  // m33 = _m33; // no change
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix33<T>::Transposed() const
{
  Matrix33<T> m(*this);
  m.Transpose();
  return m;
}
