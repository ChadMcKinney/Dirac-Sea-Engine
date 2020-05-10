/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"
#include "vector.h"

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
  inline bool operator!=(const Matrix22& rhs) const;

  inline void operator*=(const Matrix22& rhs);
  inline Matrix22<T> operator*(const Matrix22& rhs) const;

  inline void Transpose();
  inline Matrix22<T> Transposed() const;

  inline void SetRotationZ(T radians); // Rotation around origin (through z-axis)
  inline static Matrix22<T> CreateRotationZ(T radians); // Rotation around origin (through z-axis)

  inline void SetScale(const Vec2<T>& s);
  inline static Matrix22<T> CreateScale(const Vec2<T>& s);

  inline Vec2<T> GetRow1() const;
  inline void SetRow1(const Vec2<T>& row1);
  inline Vec2<T> GetRow2() const;
  inline void SetRow2(const Vec2<T>& row2);

  inline Vec2<T> GetColumn1() const;
  inline void SetColumn1(const Vec2<T>& column1);
  inline Vec2<T> GetColumn2() const;
  inline void SetColumn2(const Vec2<T>& column2);

  inline T Determinant() const;
  
  inline void Invert();
  inline Matrix22<T> Inverted() const;

  inline void Invert_Safe();
  inline Matrix22<T> Inverted_Safe() const;

  inline void Orthonormalize();
  inline Matrix22<T> Orthonormalized() const;

  inline void Orthonormalize_Safe();
  inline Matrix22<T> Orthonormalized_Safe() const;

  T m11, m12;
  T m21, m22;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22()
  : m11(T{}) , m12(T{})
  , m21(T{}) , m22(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(EZero)
  : m11(0), m12(0)
  , m21(0), m22(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix22<T>::Matrix22(EIdentity)
  : m11(1), m12(0)
  , m21(0), m22(1)
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
  : m11(_m11), m12(_m12)
  , m21(_m21), m22(_m22)
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
inline bool Matrix22<T>::operator!=(const Matrix22& rhs) const
{
  return m11 != rhs.m11 || m12 != rhs.m12 || m21 != rhs.m21 || m22 != rhs.m22;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::operator*=(const Matrix22& rhs)
{
  const T _m11 = (m11 * rhs.m11) + (m12 * rhs.m21);
  const T _m12 = (m11 * rhs.m12) + (m12 * rhs.m22);
  const T _m21 = (m21 * rhs.m11) + (m22 * rhs.m21);
  const T _m22 = (m21 * rhs.m12) + (m22 * rhs.m22);
  m11 = _m11;
  m12 = _m12;
  m21 = _m21;
  m22 = _m22;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::operator*(const Matrix22& rhs) const
{
  Matrix22<T> m(EUninitialized::Constructor);
  m.m11 = (m11 * rhs.m11) + (m12 * rhs.m21);
  m.m12 = (m11 * rhs.m12) + (m12 * rhs.m22);
  m.m21 = (m21 * rhs.m11) + (m22 * rhs.m21);
  m.m22 = (m21 * rhs.m12) + (m22 * rhs.m22);
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Transpose()
{
  const T _m12 = m12;
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
template <typename T>
inline void Matrix22<T>::SetRotationZ(T radians)
{
  const T cosTheta = std::cos(radians);
  const T sinTheta = std::sin(radians);
  m11 = cosTheta;
  m12 = sinTheta;
  m21 = -sinTheta;
  m22 = cosTheta;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::CreateRotationZ(T radians)
{
  Matrix22<T> m(EUninitialized::Constructor);
  m.SetRotationZ(radians);
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::SetScale(const Vec2<T>& s)
{
  m11 = s.x;
  m22 = s.y;
  m12 = m21 = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::CreateScale(const Vec2<T>& s)
{
  Matrix22<T> m(EUninitialized::Constructor);
  m.SetScale(s);
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Matrix22<T>::GetRow1() const
{
  return Vec2<T>(m11, m12);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::SetRow1(const Vec2<T>& row1)
{
  m11 = row1.x;
  m12 = row2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Matrix22<T>::GetRow2() const
{
  return Vec2<T>(m21, m22);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::SetRow2(const Vec2<T>& row2)
{
  m21 = row2.x;
  m22 = row2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Matrix22<T>::GetColumn1() const
{
  return Vec2<T>(m11, m21);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::SetColumn1(const Vec2<T>& column1)
{
  m11 = row1.x;
  m21 = row2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec2<T> Matrix22<T>::GetColumn2() const
{
  return Vec2<T>(m12, m22);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::SetColumn2(const Vec2<T>& column2)
{
  m12 = row2.x;
  m22 = row2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Matrix22<T>::Determinant() const
{
  return (m11 * m22) - (m12 * m21);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Invert()
{
  const T d = Determinant();
  assert(abs(d) > epsilon<T>());
  const T recipD = 1 / d;
  // Classical Adjoint / Determinant
  const T _m11 = m22 * recipD;
  const T _m12 = -m21 * recipD;
  const T _m21 = -m12 * recipD;
  const T _m22 = m11 * recipD;
  m11 = _m11;
  m12 = _m12;
  m21 = _m21;
  m22 = _m22;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::Inverted() const
{
  Matrix22<T> m(*this);
  m.Invert();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Invert_Safe()
{
  const T d = Determinant();
  if (abs(d) > epsilon<T>())
  {
		// Classical Adjoint / Determinant
		const T _m11 = m22 * recipD;
		const T _m12 = m21 * recipD;
		const T _m21 = m12 * recipD;
		const T _m22 = m11 * recipD;
		m11 = _m11;
		m12 = _m12;
		m21 = _m21;
		m22 = _m22;
	}
  else
  {
    m11 = m12 = m21 = m22 = 0;
  }
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::Inverted_Safe() const
{
  Matrix22<T> m(*this);
  m.Invert_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Orthonormalize()
{
  Vec2<T> r1 = GetRow1();
  r1.Normalize();
  Vec2<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r2.Dot(r1)); // set second row as perpendicular to the first row
  r2.Normalize();
  m11 = r1.x;
  m12 = r1.y;
  m21 = r2.x;
  m22 = r2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::Orthonormalized() const
{
  Matrix22<T> m(*this);
  m.Orthonormalize();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix22<T>::Orthonormalize_Safe()
{
  Vec2<T> r1 = GetRow1();
  r1.SafeNormalize();
  Vec2<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r2.Dot(r1)); // set second row as perpendicular to the first row
  r2.SafeNormalize();
  m11 = r1.x;
  m12 = r1.y;
  m21 = r2.x;
  m22 = r2.y;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> Matrix22<T>::Orthonormalized_Safe() const
{
  Matrix22<T> m(*this);
  m.Orthonormalize_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
// Post multiply -> Matrix22 x Column Vector
template <typename T>
inline Vec2<T> operator*(const Matrix22<T>& m, const Vec2<T>& v)
{
  Vec2<T> v2(EUninitialized::Constructor);
  v2.x = (m.m11 * v.x) + (m.m12 * v.y);
  v2.y = (m.m21 * v.x) + (m.m22 * v.y);
  return v2;
}

///////////////////////////////////////////////////////////////////////
// Pre multiply -> Row Vector * Matrix22
template <typename T>
inline Vec2<T> operator*(const Vec2<T>& v, const Matrix22<T>& m)
{
  Vec2<T> v2(EUninitialized::Constructor);
  v2.x = (v.x * m.m11) + (v.y * m.m21);
  v2.y = (v.x * m.m12) + (v.y * m.m22);
  return v2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> operator*(T s, const Matrix22<T>& m)
{
  return Matrix22<T>(m.m11 * s, m.m12 * s, m.m21 * s, m.m22 * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix22<T> operator*(const Matrix22<T>& m, T s)
{
  return Matrix22<T>(m.m11 * s, m.m12 * s, m.m21 * s, m.m22 * s);
}

///////////////////////////////////////////////////////////////////////
typedef Matrix22<fworld> Matrix22w;
typedef Matrix22<flocal> Matrix22l;

