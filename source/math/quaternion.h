/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "matrix33.h"
#include "vector3.h"
#include "types.h"

///////////////////////////////////////////////////////////////////////
// Quaternion
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Quaternion
{
	Quaternion();
	Quaternion(EZero);
	Quaternion(EIdentity);
	Quaternion(EUninitialized);
	Quaternion(T _x, T _y, T _z, T _w);
	explicit Quaternion(const Matrix33<T>& m); // Assumes orthonormalized matrix

	inline bool operator==(const Quaternion& rhs) const;
	inline bool operator!=(const Quaternion& rhs) const;
	inline bool IsEquivalent(const Quaternion& rhs, T epsilon) const;

	inline void operator*=(const Quaternion& rhs);
	inline Quaternion operator*(const Quaternion& rhs) const;

	inline void operator/=(const Quaternion& rhs);
	inline Quaternion operator/(const Quaternion& rhs) const;

	inline T Dot(const Quaternion& rhs) const;

	inline static Vec3<T> Log(const Quaternion& q); // Real part is always 0
	inline static Quaternion Exp(const Vec3<T>& v);

	inline void ScalarExponentiate(T s);
	inline Quaternion ScalarExponentiated(T s) const;

	inline void SetNLerp(const Quaternion& q1, const Quaternion& q2, T t);
	inline static Quaternion CreateNLerp(const Quaternion& q1, const Quaternion& q2, T t);

	inline void SetSlerp(Quaternion q1, const Quaternion& q2, T t);
	inline static Quaternion CreateSlerp(const Quaternion& q1, const Quaternion& q2, T t);

	inline T Magnitude() const;
	inline bool IsUnit(T epsilon) const;

	inline void Normalize();
	inline Quaternion Normalized() const;

	inline void Normalize_Safe();
	inline Quaternion Normalized_Safe() const;

	inline void Negate();
	inline Quaternion Negated() const;

	inline void Invert(); // Assumes unit quaternion!
	inline Quaternion Inverted() const; // Assumes unit quaternion!

	inline Vec3<T> ExtractAxis() const;

	inline void SetAxisAngle(const Vec3<T>& axis, T radians);
	inline static Quaternion CreateAxisAngle(const Vec3<T>& axis, T radians);

	inline void SetRotationX(T radians);
	inline static Quaternion CreateRotationX(T radians);

	inline void SetRotationY(T radians);
	inline static Quaternion CreateRotationY(T radians);

	inline void SetRotationZ(T radians);
	inline static Quaternion CreateRotationZ(T radians);

	T x, y, z, w;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion()
	: x(T{})
	, y(T{})
	, z(T{})
	, w(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EZero)
	: x(0)
	, y(0)
	, z(0)
	, w(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EIdentity)
	: x(0)
	, y(0)
	, z(0)
	, w(1)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(T _x, T _y, T _z, T _w)
	: x(_x)
	, y(_y)
	, z(_z)
	, w(_w)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Quaternion<T>::Quaternion(const Matrix33<T>& m)
{
	assert(m.IsEquivalent(m.Orthonormalized_Safe(), epsilon<T>() * 4));

	const T fourWSquaredMinus1 = m.m11 + m.m22 + m.m33;
	const T fourXSquaredMinus1 = m.m11 - m.m22 - m.m33;
	const T fourYSquaredMinus1 = m.m22 - m.m11 - m.m33;
	const T fourZSquaredMinus1 = m.m33 - m.m11 - m.m22;

	enum EIndex : uint8_t
	{
		eIndex_X = 0,
		eIndex_Y = 1,
		eIndex_Z = 2,
		eIndex_W = 3,
	};

	// Determine which of w, x, y, or z has the largest absolute value
	int biggestIndex = eIndex_W;
	T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = eIndex_X;
	}

	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = eIndex_Y;
	}

	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = eIndex_Z;
	}

	// Perform square root and division
	const T biggestValue = sqrt(fourBiggestSquaredMinus1 + T(1)) * T(0.5);
	const T mult = T(0.25) / biggestValue;

	// Apply table to computer quaternion values
	switch (biggestIndex)
	{
	case eIndex_X:
		x = biggestValue;
		y = (m.m12 + m.m21) * mult;
		z = (m.m31 + m.m13) * mult;
		w = (m.m23 - m.m32) * mult;
		break;

	case eIndex_Y:
		x = (m.m12 + m.m21) * mult;
		y = biggestValue;
		z = (m.m23 + m.m32) * mult;
		w = (m.m31 - m.m13) * mult;
		break;

	case eIndex_Z:
		x = (m.m31 + m.m13) * mult;
		y = (m.m23 + m.m32) * mult;
		z = biggestValue;
		w = (m.m12 - m.m21) * mult;
		break;

	case eIndex_W:
		x = (m.m23 - m.m32) * mult;
		y = (m.m31 - m.m13) * mult;
		z = (m.m12 - m.m21) * mult;
		w = biggestValue;
		break;

	default:
		assert(false); // should never reach here!
		break;
	}

	assert(IsUnit(epsilon<T>() * 4));
}


///////////////////////////////////////////////////////////////////////
template <typename T>
Matrix33<T>::Matrix33(const Quaternion<T>& q)
{
	m11 = 1 - (2 * q.y * q.y) - (2 * q.z * q.z);
	m12 = (2 * q.x * q.y) + (2 * q.w * q.z);
	m13 = (2 * q.x * q.z) - (2 * q.w * q.y);

	m21 = (2 * q.x * q.y) - (2 * q.w * q.z);
	m22 = 1 - (2 * q.x * q.x) - (2 * q.z * q.z);
	m23 = (2 * q.y * q.z) + (2 * q.w * q.x);

	m31 = (2 * q.x * q.z) + (2 * q.w * q.y);
	m32 = (2 * q.y * q.z) - (2 * q.w * q.x);
	m33 = 1 - (2 * q.x * q.x) - (2 * q.y * q.y);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::operator==(const Quaternion& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::operator!=(const Quaternion& rhs) const
{
	return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::IsEquivalent(const Quaternion& rhs, T epsilon) const
{
	return
		(abs(x - rhs.x) < epsilon) &&
		(abs(y - rhs.y) < epsilon) &&
		(abs(z - rhs.z) < epsilon) &&
		(abs(w - rhs.w) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Quaternion<T>::Magnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::ScalarExponentiate(T s)
{
	// do nothing for identity
	if (std::abs(w) < (1 - epsilon<T>()))
	{
		const T halfTheta = std::acos(w);
		const T newHalfTheta = halfTheta * s;
		w = std::cos(newHalfTheta);

		// scale xyz
		const T mult = std::sin(newHalfTheta) / std::sin(halfTheta);
		x *= mult;
		y *= mult;
		z *= mult;
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::ScalarExponentiated(T s) const
{
	Quaternion<T> q(*this);
	q.ScalarExponentiate(s);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetNLerp(const Quaternion& q1, const Quaternion& q2, T t)
{
	const T t1 = T(1) - t;
	x = (q1.x * t1) + (q2.x * t);
	y = (q1.y * t1) + (q2.y * t);
	z = (q1.z * t1) + (q2.z * t);
	w = (q1.w * t1) + (q2.w * t);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateNLerp(const Quaternion& q1, const Quaternion& q2, T t)
{
	Quaternion q(EUninitialized::Constructor);
	q.SetNLerp(q1, q2, t);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetSlerp(Quaternion q1, const Quaternion& q2, T t)
{
	assert(q1.IsUnit(epsilon<T>() * 4));
	assert(q2.IsUnit(epsilon<T>() * 4));
	T cosOmega = q1.Dot(q2);

	// If cosOmega is negative then negate one of the quaternions to take the shortest path
	if (cosOmega < -epsilon<T>())
	{
		q1.Negate();
		cosOmega = -cosOmega;
	}

	static constexpr T threshold = T(1) - (epsilon<T>() * 4);
	if (cosOmega > threshold)
	{
		SetNLerp(q1, q2, t);
	}
	else
	{
		// Compute the sin of the angle using
		// trig identity sin^2(omega) + cos^2(omega) = 1
		const T sinOmega = std::sqrt(T(1) - (cosOmega * cosOmega));
		const T omega = std::atan2(sinOmega, cosOmega);
		const T recipSinOmega = T(1) / sinOmega;
		const T t1 = std::sin((T(1) - t) * omega) * recipSinOmega;
		const T t2 = std::sin(t * omega) * recipSinOmega;

		x = (q1.x * t1) + (q2.x * t2);
		y = (q1.y * t1) + (q2.y * t2);
		z = (q1.z * t1) + (q2.z * t2);
		w = (q1.w * t1) + (q2.w * t2);
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateSlerp(const Quaternion& q1, const Quaternion& q2, T s)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetSlerp(q1, q2, s);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::operator*=(const Quaternion& rhs)
{
	*this = *this * rhs;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.x = (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
	q.y = (w * rhs.y) + (y * rhs.w) + (z * rhs.x) - (x * rhs.z);
	q.z = (w * rhs.z) + (z * rhs.w) + (x * rhs.y) - (y * rhs.x);
	q.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::operator/=(const Quaternion& rhs)
{
	*this = *this / rhs;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::operator/(const Quaternion& rhs) const
{
	return rhs * Inverted();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Quaternion<T>::Dot(const Quaternion& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
}

///////////////////////////////////////////////////////////////////////
// The real part is always 0, so just returning x,y,z
template <typename T>
inline Vec3<T> Quaternion<T>::Log(const Quaternion& q)
{
	const T halfTheta = std::acos(q.w);
	return q.ExtractAxis().Scaled(halfTheta);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Exp(const Vec3<T>& v)
{
	const T sqrMagnitude = v.SqrMagnitude();
	if (sqrMagnitude > epsilon<T>())
	{
		const T magnitude = std::sqrt(sqrMagnitude);
		const T sinM = std::sin(magnitude) / magnitude;
		const T cosM = std::cos(magnitude);
		return Quaternion<T>(v.x * sinM, v.y * sinM, v.z * sinM, cosM);
	}
	return Quaternion<T>(EIdentity::Constructor);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Quaternion<T>::IsUnit(T epsilon) const
{
	return abs(T(1) - Magnitude()) < epsilon;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Normalize()
{
	const T magnitude = Magnitude();
	assert(magnitude > epsilon<T>());
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	w /= magnitude;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Normalized() const
{
	Quaternion<T> q(*this);
	q.Normalize();
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Normalize_Safe()
{
	const T magnitude = Magnitude();
	if (magnitude > epsilon<T>())
	{
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
	}
	else
	{
		x = y = z = 0;
		w = 1;
	}
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Normalized_Safe() const
{
	Quaternion<T> q(*this);
	q.Normalize_Safe();
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Negated() const
{
	return Quaternion<T>(-x, -y, -z, -w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::Invert()
{
	assert(IsUnit(epsilon<T>()));
	x = -x;
	y = -y;
	z = -z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::Inverted() const
{
	assert(IsUnit(epsilon<T>() * T(4)));
	return Quaternion<T>(-x, -y, -z, w);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetAxisAngle(const Vec3<T>& axis, T radians)
{
	assert(axis.IsUnit(epsilon<T>()));
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = axis.x * sinHalfTheta;
	y = axis.y * sinHalfTheta;
	z = axis.z * sinHalfTheta;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Quaternion<T>::ExtractAxis() const
{
	return Vec3<T>(x, y, z).SafeNormalized();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateAxisAngle(const Vec3<T>& axis, T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetAxisAngle(axis, radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationX(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = sinHalfTheta;
	y = 0;
	z = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationX(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationX(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationY(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = 0;
	y = sinHalfTheta;
	z = 0;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationY(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationY(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Quaternion<T>::SetRotationZ(T radians)
{
	const T halfTheta = radians * T(0.5);
	const T sinHalfTheta = std::sin(halfTheta);
	w = std::cos(halfTheta);
	x = 0;
	y = 0;
	z = sinHalfTheta;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> Quaternion<T>::CreateRotationZ(T radians)
{
	Quaternion<T> q(EUninitialized::Constructor);
	q.SetRotationZ(radians);
	return q;
}

///////////////////////////////////////////////////////////////////////
// Post-Multiply: rotate V by q rotation
template <typename T>
inline Vec3<T> operator*(const Quaternion<T>& q, const Vec3<T>& v)
{
	const Quaternion<T> qv(v.x, v.y, v.z, 0);
	const Quaternion<T> qv2 = q * qv * q.Inverted();
	return Vec3<T>(qv2.x, qv2.y, qv2.z);
}

///////////////////////////////////////////////////////////////////////
// Pre-Multiply: rotate V by inverstion of q rotation!
template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v, const Quaternion<T>& q)
{
	const Quaternion<T> q2 = q.Inverted();
	const Quaternion<T> qv(v.x, v.y, v.z, 0);
	const Quaternion<T> qv2 = q2 * qv * q;
	return Vec3<T>(qv2.x, qv2.y, qv2.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> operator*(const Quaternion<T>& q, T s)
{
	return Quaternion<T>(q.x * s, q.y * s, q.z * s, q.w * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Quaternion<T> operator*(T s, const Quaternion<T>& q)
{
	return Quaternion<T>(q.x * s, q.y * s, q.z * s, q.w * s);
}

///////////////////////////////////////////////////////////////////////
typedef Quaternion<fworld> Quaternionw;
typedef Quaternion<flocal> Quaternionl;
