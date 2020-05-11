/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "matrix33.h"
#include "matrix44.h"
#include "types.h"
#include "vector3.h"
#include "vector4.h"

///////////////////////////////////////////////////////////////////////
// Matrix 4x3
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Matrix43
{
  Matrix43();
  Matrix43(EZero);
  Matrix43(EIdentity);
  Matrix43(EUninitialized);
  Matrix43(
    T _m11, T _m12, T _m13,
    T _m21, T _m22, T _m23,
    T _m31, T _m32, T _m33,
    T _m41, T _m42, T _m43);
  Matrix43(const Matrix33<T>& matrix33);

	inline bool operator==(const Matrix43& rhs) const;
	inline bool operator!=(const Matrix43& rhs) const;
	inline bool IsEquivalent(const Matrix43& rhs, T epsilon) const;

  inline void operator*=(const Matrix43& rhs);
  inline Matrix43<T> operator*(const Matrix43& rhs) const;

  inline struct Matrix44<T> Transposed() const;
  inline Matrix33<T> WithoutTranslation() const;

  inline void SetTranslation(const Vec3<T>& t);
  inline static Matrix43<T> CreateTranslation(const Vec3<T>& t);

  inline T Determinant() const;
  
  inline void Invert();
  inline Matrix43<T> Inverted() const;

  inline void Invert_Safe();
  inline Matrix43<T> Inverted_Safe() const;

  // Doesn't affect translation, only removes scale
  inline void Orthonormalize();
  inline Matrix43<T> Orthonormalized() const;

  // Doesn't affect translation, only removes scale
  inline void Orthonormalize_Safe();
  inline Matrix43<T> Orthonormalized_Safe() const;

  inline Vec3<T> GetRow1() const;
  inline void SetRow1(const Vec3<T>& r);
  inline Vec3<T> GetRow2() const;
  inline void SetRow2(const Vec3<T>& r);
  inline Vec3<T> GetRow3() const;
  inline void SetRow3(const Vec3<T>& r);
  inline Vec3<T> GetRow4() const;
  inline void SetRow4(const Vec3<T>& r);

  inline Vec4<T> GetColumn1() const;
  inline void SetColumn1(const Vec4<T>& c);
  inline Vec4<T> GetColumn2() const;
  inline void SetColumn2(const Vec4<T>& c);
  inline Vec4<T> GetColumn3() const;
  inline void SetColumn3(const Vec4<T>& c);

  inline Matrix33<T> Minor14() const;
  inline Matrix33<T> Minor24() const;
  inline Matrix33<T> Minor34() const;

  T m11, m12, m13;
  T m21, m22, m23;
  T m31, m32, m33;
  T m41, m42, m43;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43()
  : m11(T{}), m12(T{}), m13(T{})
  , m21(T{}), m22(T{}), m23(T{})
  , m31(T{}), m32(T{}), m33(T{})
  , m41(T{}), m42(T{}), m43(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43(EZero)
  : m11(0), m12(0), m13(0)
  , m21(0), m22(0), m23(0)
  , m31(0), m32(0), m33(0)
  , m41(0), m42(0), m43(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43(EIdentity)
  : m11(1), m12(0), m13(0)
  , m21(0), m22(1), m23(0)
  , m31(0), m32(0), m33(1)
  , m41(0), m42(0), m43(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43(
  T _m11, T _m12, T _m13,
  T _m21, T _m22, T _m23,
  T _m31, T _m32, T _m33,
  T _m41, T _m42, T _m43)
  : m11(_m11), m12(_m12), m13(_m13)
  , m21(_m21), m22(_m22), m23(_m23)
  , m31(_m31), m32(_m32), m33(_m33)
  , m41(_m41), m42(_m42), m43(_m43)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix43<T>::Matrix43(const Matrix33<T>& matrix33)
  : m11(matrix33.m11), m12(matrix33.m12), m13(matrix33.m13)
  , m21(matrix33.m21), m22(matrix33.m22), m23(matrix33.m23)
  , m31(matrix33.m31), m32(matrix33.m32), m33(matrix33.m33)
  , m41(0), m42(0), m43(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix43<T>::operator==(const Matrix43& rhs) const
{
  return
		m11 == rhs.m11 && m12 == rhs.m12 && m13 == rhs.m13 &&
		m21 == rhs.m21 && m22 == rhs.m22 && m23 == rhs.m23 &&
		m31 == rhs.m31 && m32 == rhs.m32 && m33 == rhs.m33 &&
		m41 == rhs.m41 && m42 == rhs.m42 && m43 == rhs.m43;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix43<T>::operator!=(const Matrix43& rhs) const
{
  return
		m11 == rhs.m11 && m12 == rhs.m12 && m13 == rhs.m13 &&
		m21 == rhs.m21 && m22 == rhs.m22 && m23 == rhs.m23 &&
		m31 == rhs.m31 && m32 == rhs.m32 && m33 == rhs.m33 &&
		m41 == rhs.m41 && m42 == rhs.m42 && m43 == rhs.m43;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix43<T>::IsEquivalent(const Matrix43& rhs, T epsilon) const
{
  return
    (abs(m11 - rhs.m11) < epsilon) && (abs(m12 - rhs.m12) < epsilon) && (abs(m13 - rhs.m13) < epsilon) &&
    (abs(m21 - rhs.m21) < epsilon) && (abs(m22 - rhs.m22) < epsilon) && (abs(m23 - rhs.m23) < epsilon) &&
    (abs(m31 - rhs.m31) < epsilon) && (abs(m32 - rhs.m32) < epsilon) && (abs(m33 - rhs.m33) < epsilon) &&
    (abs(m41 - rhs.m41) < epsilon) && (abs(m42 - rhs.m42) < epsilon) && (abs(m43 - rhs.m43) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::operator*=(const Matrix43& rhs)
{
  *this = *this * rhs;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::operator*(const Matrix43& rhs) const
{
	Matrix43<T> m(EUninitialized::Constructor);
  m.m11 = (m11 * rhs.m11) + (m12 * rhs.m21) + (m13 * rhs.m31);
  m.m12 = (m11 * rhs.m12) + (m12 * rhs.m22) + (m13 * rhs.m32);
  m.m13 = (m11 * rhs.m13) + (m12 * rhs.m23) + (m13 * rhs.m33);

  m.m21 = (m21 * rhs.m11) + (m22 * rhs.m21) + (m23 * rhs.m31);
  m.m22 = (m21 * rhs.m12) + (m22 * rhs.m22) + (m23 * rhs.m32);
  m.m23 = (m21 * rhs.m13) + (m22 * rhs.m23) + (m23 * rhs.m33);

  m.m31 = (m31 * rhs.m11) + (m32 * rhs.m21) + (m33 * rhs.m31);
  m.m32 = (m31 * rhs.m12) + (m32 * rhs.m22) + (m33 * rhs.m32);
  m.m33 = (m31 * rhs.m13) + (m32 * rhs.m23) + (m33 * rhs.m33);

  m.m41 = (m41 * rhs.m11) + (m42 * rhs.m21) + (m43 * rhs.m31) + rhs.m41;
  m.m42 = (m41 * rhs.m12) + (m42 * rhs.m22) + (m43 * rhs.m32) + rhs.m42;
  m.m43 = (m41 * rhs.m13) + (m42 * rhs.m23) + (m43 * rhs.m33) + rhs.m43;
	return m;
}

///////////////////////////////////////////////////////////////////////
// Vec4 Post multiply -> Matrix43 x Column Vector
template <typename T>
inline Vec4<T> operator*(const Matrix44<T>& m, const Vec3<T>& v)
{
  Vec4<T> v2(EUninitialized::Constructor);
  v2.x = (m.m11 * v.x) + (m.m12 * v.y) + (m.m13 * v.z);
  v2.y = (m.m21 * v.x) + (m.m22 * v.y) + (m.m23 * v.z);
  v2.z = (m.m31 * v.x) + (m.m32 * v.y) + (m.m33 * v.z);
  v2.w = (m.m41 * v.x) + (m.m42 * v.y) + (m.m43 * v.z) + v.w;
  return v2;
}

///////////////////////////////////////////////////////////////////////
// Vec4 Pre multiply -> Row Vector * Matrix43
template <typename T>
inline Vec4<T> operator*(const Vec4<T>& v, const Matrix43<T>& m)
{
  Vec4<T> v2(EUninitialized::Constructor);
  v2.x = (v.x * m.m11) + (v.y * m.m21) + (v.z * m.m31) + (v.w * m.m41);
  v2.y = (v.x * m.m12) + (v.y * m.m22) + (v.z * m.m32) + (v.w * m.m42);
  v2.z = (v.x * m.m13) + (v.y * m.m23) + (v.z * m.m33) + (v.w * m.m43);
  v2.w = v.w;
  return v2;
}

///////////////////////////////////////////////////////////////////////
// No Vec3 Post multiply as it discards translation without the 4th column

///////////////////////////////////////////////////////////////////////
// Vec3 Pre multiply -> Row Vector * Matrix43
template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v, const Matrix43<T>& m)
{
  Vec3<T> v2(EUninitialized::Constructor);
  v2.x = (v.x * m.m11) + (v.y * m.m21) + (v.z * m.m31) + m.m41;
  v2.y = (v.x * m.m12) + (v.y * m.m22) + (v.z * m.m32) + m.m42;
  v2.z = (v.x * m.m13) + (v.y * m.m23) + (v.z * m.m33) + m.m43;
  return v2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix43<T>::Transposed() const
{
  Matrix44<T> m(EUninitialized::Constructor);
  m.m11 = m11;
  m.m12 = m21;
  m.m13 = m31;
  m.m14 = m41;

  m.m21 = m12;
  m.m22 = m22;
  m.m23 = m32;
  m.m24 = m42;

  m.m31 = m13;
  m.m32 = m23;
  m.m33 = m33;
  m.m34 = m43;

  m.m41 = 0;
  m.m42 = 0;
  m.m43 = 0;
  m.m44 = 1;
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix43<T>::WithoutTranslation() const
{
  return Matrix33<T>(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetTranslation(const Vec3<T>& t)
{
  m11 = 1;
  m12 = 0;
  m13 = 0;
  
  m21 = 0;
  m22 = 1;
  m23 = 0;

  m31 = 0;
  m32 = 0;
  m33 = 1;

  m41 = t.x;
  m42 = t.y;
  m43 = t.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::CreateTranslation(const Vec3<T>& t)
{
  Matrix43<T> m(EUninitialized::Constructor);
  m.SetTranslation(t);
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Matrix43<T>::Determinant() const
{
  // Simplified determinant as fourth column makes a more precise one ambiguous
  return (m11 * m11) + (m12 * m21) + (m31 * m13);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::Invert()
{
  const T d = Determinant();
  assert(abs(d) > epsilon<T>());
  const T recipD = 1 / d;
  const T negRecipD = -recipD;

  Matrix33<T> matrix33 = WithoutTranslation();
  matrix33.Invert();

  const T c14D = Minor14().Determinant() * negRecipD;
  const T c24D = Minor24().Determinant() * recipD;
  const T c34D = Minor34().Determinant() * negRecipD;

	m11 = matrix33.m11;
	m12 = matrix33.m12;
	m13 = matrix33.m13;

	m21 = matrix33.m21;
	m22 = matrix33.m22;
	m23 = matrix33.m23;

	m31 = matrix33.m31;
	m32 = matrix33.m32;
	m33 = matrix33.m33;

  m41 = c14D;
  m42 = c24D;
  m43 = c34D;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::Inverted() const
{
  Matrix43<T> m(*this);
  m.Invert();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::Invert_Safe()
{
  const T d = Determinant();
  if (d > epsilon<T>())
  {
		const T d = Determinant();
		assert(abs(d) > epsilon<T>());
		const T recipD = 1 / d;
		const T negRecipD = -recipD;

		Matrix33<T> matrix33 = WithoutTranslation();
		matrix33.Invert();

		const T c14D = Minor14().Determinant() * negRecipD;
		const T c24D = Minor24().Determinant() * recipD;
		const T c34D = Minor34().Determinant() * negRecipD;

		m11 = matrix33.m11;
		m12 = matrix33.m12;
		m13 = matrix33.m13;

		m21 = matrix33.m21;
		m22 = matrix33.m22;
		m23 = matrix33.m23;

		m31 = matrix33.m31;
		m32 = matrix33.m32;
		m33 = matrix33.m33;

		m41 = c14D;
		m42 = c24D;
		m43 = c34D;
	}
	else
	{
		*this = Matrix43<T>(EZero::Constructor);
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::Inverted_Safe() const
{
  Matrix43<T> m(*this);
  m.Invert_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::Orthonormalize()
{
  Vec3<T> r1 = GetRow1();
  r1.Normalize();

  Vec3<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r1.Dot(r2));
  r2.Normalize();

  Vec3<T> r3 = GetRow3();
  r3 = r3 - r2.Scaled(r2.Dot(r3));
  r3.Normalize();

  SetRow1(r1);
  SetRow2(r2);
  SetRow3(r3);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::Orthonormalized() const
{
  Matrix43<T> m(*this);
  m.Orthonormalize();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::Orthonormalize_Safe()
{
  Vec3<T> r1 = GetRow1();
  r1.SafeNormalize();

  Vec3<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r1.Dot(r2));
  r2.SafeNormalize();

  Vec3<T> r3 = GetRow3();
  r3 = r3 - r2.Scaled(r2.Dot(r3));
  r3.SafeNormalize();

  SetRow1(r1);
  SetRow2(r2);
  SetRow3(r3);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix43<T> Matrix43<T>::Orthonormalized_Safe() const
{
  Matrix43<T> m(*this);
  m.Orthonormalize_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix43<T>::GetRow1() const
{
  return Vec3<T>(m11, m12, m13);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetRow1(const Vec3<T>& r)
{
  m11 = r.x;
  m12 = r.y;
  m13 = r.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix43<T>::GetRow2() const
{
  return Vec3<T>(m21, m22, m23);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetRow2(const Vec3<T>& r)
{
  m21 = r.x;
  m22 = r.y;
  m23 = r.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix43<T>::GetRow3() const
{
  return Vec3<T>(m31, m32, m33);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetRow3(const Vec3<T>& r)
{
  m31 = r.x;
  m32 = r.y;
  m33 = r.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Matrix43<T>::GetRow4() const
{
  return Vec3<T>(m41, m42, m43);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetRow4(const Vec3<T>& r)
{
  m41 = r.x;
  m42 = r.y;
  m43 = r.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix43<T>::GetColumn1() const
{
  return Vec3<T>(m11, m21, m31, m41);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetColumn1(const Vec4<T>& c)
{
  m11 = c.x;
  m21 = c.y;
  m31 = c.z;
  m41 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix43<T>::GetColumn2() const
{
  return Vec3<T>(m12, m22, m32, m42);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetColumn2(const Vec4<T>& c)
{
  m12 = c.x;
  m22 = c.y;
  m32 = c.z;
  m42 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix43<T>::GetColumn3() const
{
  return Vec3<T>(m13, m23, m33, m43);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix43<T>::SetColumn3(const Vec4<T>& c)
{
  m13 = c.x;
  m23 = c.y;
  m33 = c.z;
  m43 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix43<T>::Minor14() const
{
  return Matrix33<T>(
    m21, m22, m23,
    m31, m32, m33,
    m41, m42, m43
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix43<T>::Minor24() const
{
  return Matrix33<T>(
    m11, m12, m13,
    m31, m32, m33,
    m41, m42, m43
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix43<T>::Minor34() const
{
  return Matrix33<T>(
    m11, m12, m13,
    m21, m22, m23,
    m41, m42, m43
  );
}
