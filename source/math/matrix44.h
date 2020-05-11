/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "matrix33.h"
#include "types.h"
#include "vector.h"

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

	inline bool operator==(const Matrix44& rhs) const;
	inline bool operator!=(const Matrix44& rhs) const;
	inline bool IsEquivalent(const Matrix44& rhs, T epsilon) const;

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
