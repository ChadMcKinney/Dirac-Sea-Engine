/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "matrix33.h"
#include "types.h"
#include "vector4.h"

///////////////////////////////////////////////////////////////////////
// Matrix 4x4
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Matrix44
{
  Matrix44();
  Matrix44(EZero);
  Matrix44(EIdentity);
  Matrix44(EUninitialized);
  Matrix44(
    T _m11, T _m12, T _m13, T _m14,
    T _m21, T _m22, T _m23, T _m24,
    T _m31, T _m32, T _m33, T _m34,
    T _m41, T _m42, T _m43, T _m44);
	Matrix44(const Matrix33<T>& matrix33);

	inline bool operator==(const Matrix44& rhs) const;
	inline bool operator!=(const Matrix44& rhs) const;
	inline bool IsEquivalent(const Matrix44& rhs, T epsilon) const;

	inline void operator*=(const Matrix44& rhs);
	inline Matrix44<T> operator*(const Matrix44& rhs) const;

  inline void Transpose();
  inline Matrix44<T> Transposed() const;

  inline T Determinant() const;
  
  inline void Invert();
  inline Matrix44<T> Inverted() const;

  inline void Invert_Safe();
  inline Matrix44<T> Inverted_Safe() const;

  inline void Orthonormalize();
  inline Matrix44<T> Orthonormalized() const;

  inline void Orthonormalize_Safe();
  inline Matrix44<T> Orthonormalized_Safe() const;

  inline Vec4<T> GetRow1() const;
  inline void SetRow1(const Vec4<T>& r);
  inline Vec4<T> GetRow2() const;
  inline void SetRow2(const Vec4<T>& r);
  inline Vec4<T> GetRow3() const;
  inline void SetRow3(const Vec4<T>& r);
  inline Vec4<T> GetRow4() const;
  inline void SetRow4(const Vec4<T>& r);

  inline Vec4<T> GetColumn1() const;
  inline void SetColumn1(const Vec4<T>& c);
  inline Vec4<T> GetColumn2() const;
  inline void SetColumn2(const Vec4<T>& c);
  inline Vec4<T> GetColumn3() const;
  inline void SetColumn3(const Vec4<T>& c);
  inline Vec4<T> GetColumn4() const;
  inline void SetColumn4(const Vec4<T>& c);

  // TODO: add w normalizing for depth buffer precision
  // TODO: add FOV for X/Y
  inline void SetPerspectiveProjection(T d);
  inline static Matrix44<T> CreatePerspectiveProjection(T d);

  inline Matrix33<T> Minor11() const;
  inline Matrix33<T> Minor12() const;
  inline Matrix33<T> Minor13() const;
  inline Matrix33<T> Minor14() const;
  inline Matrix33<T> Minor21() const;
  inline Matrix33<T> Minor22() const;
  inline Matrix33<T> Minor23() const;
  inline Matrix33<T> Minor24() const;
  inline Matrix33<T> Minor31() const;
  inline Matrix33<T> Minor32() const;
  inline Matrix33<T> Minor33() const;
  inline Matrix33<T> Minor34() const;
  inline Matrix33<T> Minor41() const;
  inline Matrix33<T> Minor42() const;
  inline Matrix33<T> Minor43() const;
  inline Matrix33<T> Minor44() const;

	T m11, m12, m13, m14;
	T m21, m22, m23, m24;
	T m31, m32, m33, m34;
	T m41, m42, m43, m44;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44()
	: m11(T{}), m12(T{}), m13(T{}), m14(T{})
	, m21(T{}), m22(T{}), m23(T{}), m24(T{})
	, m31(T{}), m32(T{}), m33(T{}), m34(T{})
	, m41(T{}), m42(T{}), m43(T{}), m44(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44(EZero)
	: m11(0), m12(0), m13(0), m14(0)
	, m21(0), m22(0), m23(0), m24(0)
	, m31(0), m32(0), m33(0), m34(0)
	, m41(0), m42(0), m43(0), m44(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44(EIdentity)
	: m11(1), m12(0), m13(0), m14(0)
	, m21(0), m22(1), m23(0), m24(0)
	, m31(0), m32(0), m33(1), m34(0)
	, m41(0), m42(0), m43(0), m44(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44(
	T _m11, T _m12, T _m13, T _m14,
	T _m21, T _m22, T _m23, T _m24,
	T _m31, T _m32, T _m33, T _m34,
	T _m41, T _m42, T _m43, T _m44)
	: m11(_m11), m12(_m12), m13(_m13), m14(_m14)
	, m21(_m21), m22(_m22), m23(_m23), m24(_m24)
	, m31(_m31), m32(_m32), m33(_m33), m34(_m34)
	, m41(_m41), m42(_m42), m43(_m43), m44(_m44)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix44<T>::Matrix44(const Matrix33<T>& matrix33)
	: m11(matrix33.m11), m12(matrix33.m12), m13(matrix33.m13), m14(0)
	, m21(matrix33.m21), m22(matrix33.m22), m23(matrix33.m23), m24(0)
	, m31(matrix33.m31), m32(matrix33.m32), m33(matrix33.m33), m34(0)
	, m41(0),						 m42(0),						m43(0),						 m44(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix44<T>::operator==(const Matrix44& rhs) const
{
	return
		m11 == rhs.m11 && m12 == rhs.m12 && m13 == rhs.m13 && m14 == rhs.m14 &&
		m21 == rhs.m21 && m22 == rhs.m22 && m23 == rhs.m23 && m24 == rhs.m24 &&
		m31 == rhs.m31 && m32 == rhs.m32 && m33 == rhs.m33 && m34 == rhs.m34 &&
		m41 == rhs.m41 && m42 == rhs.m42 && m43 == rhs.m43 && m44 == rhs.m44;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix44<T>::operator!=(const Matrix44& rhs) const
{
	return
		m11 != rhs.m11 && m12 != rhs.m12 && m13 != rhs.m13 && m14 != rhs.m14 &&
		m21 != rhs.m21 && m22 != rhs.m22 && m23 != rhs.m23 && m24 != rhs.m24 &&
		m31 != rhs.m31 && m32 != rhs.m32 && m33 != rhs.m33 && m34 != rhs.m34 &&
		m41 != rhs.m41 && m42 != rhs.m42 && m43 != rhs.m43 && m44 != rhs.m44;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Matrix44<T>::IsEquivalent(const Matrix44& rhs, T epsilon) const
{
	return
		(abs(m11 - rhs.m11) < epsilon) && (abs(m12 - rhs.m12) < epsilon) && (abs(m13 - rhs.m13) < epsilon) && (abs(m14 - rhs.m14) < epsilon) &&
		(abs(m21 - rhs.m21) < epsilon) && (abs(m22 - rhs.m22) < epsilon) && (abs(m23 - rhs.m23) < epsilon) && (abs(m24 - rhs.m24) < epsilon) &&
		(abs(m31 - rhs.m31) < epsilon) && (abs(m32 - rhs.m32) < epsilon) && (abs(m33 - rhs.m33) < epsilon) && (abs(m34 - rhs.m34) < epsilon) &&
		(abs(m41 - rhs.m41) < epsilon) && (abs(m42 - rhs.m42) < epsilon) && (abs(m43 - rhs.m43) < epsilon) && (abs(m44 - rhs.m44) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::operator*=(const Matrix44& rhs)
{
	*this = *this * rhs;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::operator*(const Matrix44& rhs) const
{
	Matrix44<T> m(EUninitialized::Constructor);
  m.m11 = (m11 * rhs.m11) + (m12 * rhs.m21) + (m13 * rhs.m31) + (m14 * rhs.m41);
  m.m12 = (m11 * rhs.m12) + (m12 * rhs.m22) + (m13 * rhs.m32) + (m14 * rhs.m42);
  m.m13 = (m11 * rhs.m13) + (m12 * rhs.m23) + (m13 * rhs.m33) + (m14 * rhs.m43);
  m.m14 = (m11 * rhs.m14) + (m12 * rhs.m24) + (m13 * rhs.m34) + (m14 * rhs.m44);

  m.m21 = (m21 * rhs.m11) + (m22 * rhs.m21) + (m23 * rhs.m31) + (m24 * rhs.m41);
  m.m22 = (m21 * rhs.m12) + (m22 * rhs.m22) + (m23 * rhs.m32) + (m24 * rhs.m42);
  m.m23 = (m21 * rhs.m13) + (m22 * rhs.m23) + (m23 * rhs.m33) + (m24 * rhs.m43);
  m.m24 = (m21 * rhs.m14) + (m22 * rhs.m24) + (m23 * rhs.m34) + (m24 * rhs.m44);

  m.m31 = (m31 * rhs.m11) + (m32 * rhs.m21) + (m33 * rhs.m31) + (m34 * rhs.m41);
  m.m32 = (m31 * rhs.m12) + (m32 * rhs.m22) + (m33 * rhs.m32) + (m34 * rhs.m42);
  m.m33 = (m31 * rhs.m13) + (m32 * rhs.m23) + (m33 * rhs.m33) + (m34 * rhs.m43);
  m.m34 = (m31 * rhs.m14) + (m32 * rhs.m24) + (m33 * rhs.m34) + (m34 * rhs.m44);

  m.m41 = (m41 * rhs.m11) + (m42 * rhs.m21) + (m43 * rhs.m31) + (m44 * rhs.m41);
  m.m42 = (m41 * rhs.m12) + (m42 * rhs.m22) + (m43 * rhs.m32) + (m44 * rhs.m42);
  m.m43 = (m41 * rhs.m13) + (m42 * rhs.m23) + (m43 * rhs.m33) + (m44 * rhs.m43);
  m.m44 = (m41 * rhs.m14) + (m42 * rhs.m24) + (m43 * rhs.m34) + (m44 * rhs.m44);
	return m;
}

///////////////////////////////////////////////////////////////////////
// Post multiply -> Matrix44 x Column Vector
template <typename T>
inline Vec4<T> operator*(const Matrix44<T>& m, const Vec4<T>& v)
{
  Vec4<T> v2(EUninitialized::Constructor);
  v2.x = (m.m11 * v.x) + (m.m12 * v.y) + (m.m13 * v.z) + (m14 * v.w);
  v2.y = (m.m21 * v.x) + (m.m22 * v.y) + (m.m23 * v.z) + (m24 * v.w);
  v2.z = (m.m31 * v.x) + (m.m32 * v.y) + (m.m33 * v.z) + (m34 * v.w);
  v2.w = (m.m41 * v.x) + (m.m42 * v.y) + (m.m43 * v.z) + (m44 * v.w);
  return v2;
}

///////////////////////////////////////////////////////////////////////
// Pre multiply -> Row Vector * Matrix44
template <typename T>
inline Vec4<T> operator*(const Vec4<T>& v, const Matrix44<T>& m)
{
  Vec4<T> v2(EUninitialized::Constructor);
  v2.x = (v.x * m.m11) + (v.y * m.m21) + (v.z * m.m31) + (v.w * m.m41);
  v2.y = (v.x * m.m12) + (v.y * m.m22) + (v.z * m.m32) + (v.w * m.m42);
  v2.z = (v.x * m.m13) + (v.y * m.m23) + (v.z * m.m33) + (v.w * m.m43);
  v2.w = (v.x * m.m14) + (v.y * m.m24) + (v.z * m.m34) + (v.w * m.m44);
  return v2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> operator*(T s, const Matrix44<T>& m)
{
  Matrix44<T> m2(EUninitialized::Constructor);
  m2.m11 = m.m11 * s;
  m2.m12 = m.m12 * s;
  m2.m13 = m.m13 * s;
  m2.m14 = m.m14 * s;

  m2.m21 = m.m21 * s;
  m2.m22 = m.m22 * s;
  m2.m23 = m.m23 * s;
  m2.m24 = m.m24 * s;

  m2.m31 = m.m31 * s;
  m2.m32 = m.m32 * s;
  m2.m33 = m.m33 * s;
  m2.m34 = m.m34 * s;

  m2.m41 = m.m41 * s;
  m2.m42 = m.m42 * s;
  m2.m43 = m.m43 * s;
  m2.m44 = m.m44 * s;
  return m2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> operator*(const Matrix44<T>& m, T s)
{
  Matrix44<T> m2(EUninitialized::Constructor);
  m2.m11 = m.m11 * s;
  m2.m12 = m.m12 * s;
  m2.m13 = m.m13 * s;
  m2.m14 = m.m14 * s;

  m2.m21 = m.m21 * s;
  m2.m22 = m.m22 * s;
  m2.m23 = m.m23 * s;
  m2.m24 = m.m24 * s;

  m2.m31 = m.m31 * s;
  m2.m32 = m.m32 * s;
  m2.m33 = m.m33 * s;
  m2.m34 = m.m34 * s;

  m2.m41 = m.m41 * s;
  m2.m42 = m.m42 * s;
  m2.m43 = m.m43 * s;
  m2.m44 = m.m44 * s;
  return m2;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::Transpose()
{
  // const T _m11 = m11; // no change
  const T _m12 = m21;
  const T _m13 = m31;
  const T _m14 = m41;

  const T _m21 = m12;
  // const T _m22 = m22; // no change
  const T _m23 = m32;
  const T _m24 = m42;

  const T _m31 = m13;
  const T _m32 = m23;
  // const T _m33 = m33; // no change
  const T _m34 = m43;

  const T _m41 = m14;
  const T _m42 = m24;
  const T _m43 = m34;
  // const T _m44 = m44; // no change

  // m11 = _m11; // no change
  m12 = _m12;
  m13 = _m13;
  m14 = _m14;

  m21 = _m21;
  // m22 = _m22; // no change
  m23 = _m23;
  m24 = _m24;

  m31 = _m31;
  m32 = _m32;
  // m33 = _m33; // no change
  m34 = _m34;

  m41 = _m41;
  m42 = _m42;
  m43 = _m43;
  // m44 = _m44; // no change
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::Transposed() const
{
  Matrix44<T> m(*this);
  m.Transpose();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Matrix44<T>::Determinant() const
{
  return (m11 * m22 * m33 * m44) + (m12 * m23 * m34 * m41) + (m13 * m24 * m31 * m42) + (m14 * m21 * m32 * m43) -
				 (m11 * m24 * m33 * m42) - (m12 * m21 * m34 * m43) - (m13 * m22 * m31 * m44) - (m14 * m23 * m32 * m41);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::Invert()
{
  const T d = Determinant();
  assert(abs(d) > epsilon<T>());
  const T recipD = 1 / d;
  const T negRecipD = -recipD;

  const T c11D = Minor11().Determinant() * recipD;
  const T c12D = Minor12().Determinant() * negRecipD;
  const T c13D = Minor13().Determinant() * recipD;
  const T c14D = Minor14().Determinant() * negRecipD;

  const T c21D = Minor21().Determinant() * negRecipD;
  const T c22D = Minor22().Determinant() * recipD;
  const T c23D = Minor23().Determinant() * negRecipD;
  const T c24D = Minor24().Determinant() * recipD;

  const T c31D = Minor31().Determinant() * recipD;
  const T c32D = Minor32().Determinant() * negRecipD;
  const T c33D = Minor33().Determinant() * recipD;
  const T c34D = Minor34().Determinant() * negRecipD;

  const T c41D = Minor41().Determinant() * negRecipD;
  const T c42D = Minor42().Determinant() * recipD;
  const T c43D = Minor43().Determinant() * negRecipD;
  const T c44D = Minor44().Determinant() * recipD;

  m11 = c11D;
  m12 = c21D;
  m13 = c31D;
  m14 = c41D;

  m21 = c12D;
  m22 = c22D;
  m23 = c32D;
  m24 = c42D;

  m31 = c13D;
  m32 = c23D;
  m33 = c33D;
  m34 = c43D;

  m41 = c14D;
  m42 = c24D;
  m43 = c34D;
  m44 = c44D;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::Inverted() const
{
  Matrix44<T> m(*this);
  m.Invert();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::Invert_Safe()
{
  const T d = Determinant();
  if (d > epsilon<T>())
  {
		const T recipD = 1 / d;

		const T c11D = Minor11().Determinant() * recipD;
		const T c12D = Minor12().Determinant() * recipD * -1;
		const T c13D = Minor13().Determinant() * recipD;
		const T c14D = Minor14().Determinant() * recipD * -1;

		const T c21D = Minor21().Determinant() * recipD;
		const T c22D = Minor22().Determinant() * recipD * -1;
		const T c23D = Minor23().Determinant() * recipD;
		const T c24D = Minor24().Determinant() * recipD * -1;

		const T c31D = Minor31().Determinant() * recipD;
		const T c32D = Minor32().Determinant() * recipD * -1;
		const T c33D = Minor33().Determinant() * recipD;
		const T c34D = Minor34().Determinant() * recipD * -1;

		const T c41D = Minor41().Determinant() * recipD;
		const T c42D = Minor42().Determinant() * recipD * -1;
		const T c43D = Minor43().Determinant() * recipD;
		const T c44D = Minor44().Determinant() * recipD * -1;

		m11 = c11D;
		m12 = c21D;
		m13 = c31D;
		m14 = c41D;

		m21 = c12D;
		m22 = c22D;
		m23 = c32D;
		m24 = c42D;

		m31 = c13D;
		m32 = c23D;
		m33 = c33D;
		m34 = c43D;

		m41 = c14D;
		m42 = c24D;
		m43 = c34D;
		m44 = c44D;
  }
  else
  {
    *this = Matrix44<T>(EZero::Constructor);
  }
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::Inverted_Safe() const
{
  Matrix44<T> m(*this);
  m.Invert_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::Orthonormalize()
{
  Vec4<T> r1 = GetRow1();
  r1.Normalize();

  Vec4<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r1.Dot(r2));
  r2.Normalize();

  Vec4<T> r3 = GetRow3();
  r3 = r3 - r2.Scaled(r2.Dot(r3));
  r3.Normalize();

  Vec4<T> r4 = GetRow4();
  r4 = r4 - r3.Scaled(r3.Dot(r4));
  r4.Normalize();

  SetRow1(r1);
  SetRow2(r2);
  SetRow3(r3);
  SetRow4(r4);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::Orthonormalized() const
{
  Matrix44<T> m(*this);
  m.Orthonormalize();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::Orthonormalize_Safe()
{
  Vec4<T> r1 = GetRow1();
  r1.SafeNormalize();

  Vec4<T> r2 = GetRow2();
  r2 = r2 - r1.Scaled(r1.Dot(r2));
  r2.SafeNormalize();

  Vec4<T> r3 = GetRow3();
  r3 = r3 - r2.Scaled(r2.Dot(r3));
  r3.SafeNormalize();

  Vec4<T> r4 = GetRow4();
  r4 = r4 - r3.Scaled(r3.Dot(r4));
  r4.SafeNormalize();

  SetRow1(r1);
  SetRow2(r2);
  SetRow3(r3);
  SetRow4(r4);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::Orthonormalized_Safe() const
{
  Matrix44<T> m(*this);
  m.Orthonormalize_Safe();
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetRow1() const
{
  return Vec4<T>(m11, m12, m13, m14);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetRow1(const Vec4<T>& r)
{
  m11 = r.x;
  m12 = r.y;
  m13 = r.z;
  m14 = r.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetRow2() const
{
  return Vec4<T>(m21, m22, m23, m24);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetRow2(const Vec4<T>& r)
{
  m21 = r.x;
  m22 = r.y;
  m23 = r.z;
  m24 = r.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetRow3() const
{
  return Vec4<T>(m31, m32, m33, m34);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetRow3(const Vec4<T>& r)
{
  m31 = r.x;
  m32 = r.y;
  m33 = r.z;
  m34 = r.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetRow4() const
{
  return Vec4<T>(m41, m42, m43, m44);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetRow4(const Vec4<T>& r)
{
  m41 = r.x;
  m42 = r.y;
  m43 = r.z;
  m44 = r.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetColumn1() const
{
  return Vec4<T>(m11, m21, m31, m41);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetColumn1(const Vec4<T>& c)
{
  m11 = c.x;
  m21 = c.y;
  m31 = c.z;
  m41 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetColumn2() const
{
  return Vec4<T>(m12, m22, m32, m42);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetColumn2(const Vec4<T>& c)
{
  m12 = c.x;
  m22 = c.y;
  m32 = c.z;
  m42 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetColumn3() const
{
  return Vec4<T>(m13, m23, m33, m43);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetColumn3(const Vec4<T>& c)
{
  m13 = c.x;
  m23 = c.y;
  m33 = c.z;
  m43 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec4<T> Matrix44<T>::GetColumn4() const
{
  return Vec4<T>(m14, m24, m34, m44);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetColumn4(const Vec4<T>& c)
{
  m14 = c.x;
  m24 = c.y;
  m34 = c.z;
  m44 = c.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Matrix44<T>::SetPerspectiveProjection(T d)
{
  assert(d > epsilon<T>());
  m11 = 1;
  m12 = 0;
  m13 = 0;
  m14 = 0;

  m21 = 0;
  m22 = 1;
  m23 = 0;
  m24 = 0;

  m31 = 0;
  m32 = 0;
  m33 = 1;
  m34 = 1 / d;

  m41 = 0;
  m42 = 0;
  m43 = 0;
  m44 = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix44<T> Matrix44<T>::CreatePerspectiveProjection(T d)
{
  Matrix44<T> m(EUninitialized::Constructor);
  m.SetPerspectiveProjection(d);
  return m;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor11() const
{
  return Matrix33<T>(
    m22, m23, m24,
    m32, m33, m34,
    m42, m43, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor12() const
{
  return Matrix33<T>(
    m21, m23, m24,
    m31, m33, m34,
    m41, m43, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor13() const
{
  return Matrix33<T>(
    m21, m22, m24,
    m31, m32, m34,
    m41, m42, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor14() const
{
  return Matrix33<T>(
    m21, m22, m23,
    m31, m32, m33,
    m41, m42, m43
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor21() const
{
  return Matrix33<T>(
    m12, m13, m14,
    m32, m33, m34,
    m42, m43, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor22() const
{
  return Matrix33<T>(
    m11, m13, m14,
    m31, m33, m34,
    m41, m43, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor23() const
{
  return Matrix33<T>(
    m11, m12, m14,
    m31, m32, m34,
    m41, m42, m44
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor24() const
{
  return Matrix33<T>(
    m11, m12, m13,
    m31, m32, m33,
    m41, m42, m43
	);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor31() const
{
  return Matrix33<T>(
    m12, m13, m14,
    m22, m23, m24,
    m42, m43, m44
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor32() const
{
  return Matrix33<T>(
    m11, m13, m14,
    m21, m23, m24,
    m41, m43, m44
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor33() const
{
  return Matrix33<T>(
    m11, m12, m14,
    m21, m22, m24,
    m41, m42, m44
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor34() const
{
  return Matrix33<T>(
    m11, m12, m13,
    m21, m22, m23,
    m41, m42, m43
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor41() const
{
  return Matrix33<T>(
    m12, m13, m14,
    m22, m23, m24,
    m32, m33, m34
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor42() const
{
  return Matrix33<T>(
    m11, m13, m14,
    m21, m23, m24,
    m31, m33, m34
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor43() const
{
  return Matrix33<T>(
    m11, m12, m14,
    m21, m22, m24,
    m31, m32, m34
  );
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Matrix33<T> Matrix44<T>::Minor44() const
{
  return Matrix33<T>(
    m11, m12, m13,
    m21, m22, m23,
    m31, m32, m33
  );
}
