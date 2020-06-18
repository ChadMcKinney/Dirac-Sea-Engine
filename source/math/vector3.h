/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "types.h"

#include <cmath>

///////////////////////////////////////////////////////////////////////
// Vec3
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Vec3
{
    Vec3();
    Vec3(EZero);
    Vec3(EUninitialized);
    Vec3(T _x, T _y, T _z);

    inline Vec3 operator+(const Vec3& rhs) const;
    inline Vec3 operator-(const Vec3& rhs) const;
    inline bool operator==(const Vec3& rhs) const;
    inline bool operator!=(const Vec3& rhs) const;

    inline bool IsEquivalent(const Vec3& rhs, T epsilon) const;
    inline bool IsZero(T epsilon) const;
    inline bool IsUnit(T epsilon) const;

    inline void Scale(T s);
    inline Vec3 Scaled(T s) const;

    inline void Negate();
    inline Vec3 Negated() const;

    inline T Magnitude() const;
    inline T SqrMagnitude() const;

    inline T Distance(const Vec3<T>& rhs) const;
    inline T SqrDistance(const Vec3<T>& rhs) const;

    inline void Normalize();
    inline void SafeNormalize();
    inline Vec3<T> Normalized() const;
    inline Vec3<T> SafeNormalized() const;

    inline T Dot(const Vec3& rhs) const;
    inline Vec3<T> Cross(const Vec3& rhs) const;

    inline T Angle(const Vec3& rhs) const;
    inline T Angle_Safe(const Vec3& rhs) const;

    inline Vec3<T> ParallelProjection(const Vec3& rhs) const;
    inline Vec3<T> ParallelProjection_Safe(const Vec3& rhs) const;
    inline Vec3<T> PerpendicularProjection(const Vec3& rhs) const;
    inline Vec3<T> PerpendicularProjection_Safe(const Vec3& rhs) const;

    // modifies this vector, selecting the min/max elements between this vector and rhs
    inline void SelectMinBetween(const Vec3<T>& rhs);
    inline void SelectMaxBetween(const Vec3<T>& rhs);

    static const Vec3<T> Zero;
    static const Vec3<T> Up;
    static const Vec3<T> Down;
    static const Vec3<T> Left;
    static const Vec3<T> Right;
    static const Vec3<T> Forward;
    static const Vec3<T> Back;

    T x, y, z;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3()
    : x(T{})
    , y(T{})
    , z(T{})
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(EZero)
    : x(0)
    , y(0)
    , z(0)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(EUninitialized)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Vec3<T>::Vec3(T _x, T _y, T _z)
    : x(_x)
    , y(_y)
    , z(_z)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const
{
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const
{
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec3<T>::operator==(const Vec3& rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec3<T>::operator!=(const Vec3& rhs) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec3<T>::IsEquivalent(const Vec3& rhs, T epsilon) const
{
    return
        (abs(x - rhs.x) < epsilon) &&
        (abs(y - rhs.y) < epsilon) &&
        (abs(z - rhs.z) < epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec3<T>::IsZero(T epsilon) const
{
    return abs(x) < epsilon && abs(y) < epsilon && abs(z) < epsilon;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline bool Vec3<T>::IsUnit(T epsilon) const
{
    return !IsZero(epsilon) && IsEquivalent(Normalized(), epsilon);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::Scale(T s)
{
    x *= s;
    y *= s;
    z *= s;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Scaled(T s) const
{
    return Vec3<T>(x * s, y * s, z * s);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::Negate()
{
    x = -x;
    y = -y;
    z = -z;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Negated() const
{
    return Vec3<T>(-x, -y, -z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Magnitude() const
{
    return std::sqrt((x * x) + (y * y) + (z * z));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::SqrMagnitude() const
{
    return (x * x) + (y * y) + (z * z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Distance(const Vec3<T>& rhs) const
{
    return std::sqrt(SqrDistance(rhs));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::SqrDistance(const Vec3<T>& rhs) const
{
    Vec3<T> d(rhs.x - x, rhs.y - y, rhs.z - z);
    return d.SqrMagnitude();
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::Normalize()
{
    const T magnitude = Magnitude();
    assert(magnitude > epsilon<T>());
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::SafeNormalize()
{
    const T magnitude = Magnitude();
    if (magnitude > epsilon<T>())
    {
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }
    else
    {
        x = y = z = 0;
    }
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Normalized() const
{
    Vec3<T> v(x, y, z);
    v.Normalize();
    return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::SafeNormalized() const
{
    Vec3<T> v(x, y, z);
    v.SafeNormalize();
    return v;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Dot(const Vec3& rhs) const
{
    return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::Cross(const Vec3& rhs) const
{
    Vec3<T> c(EUninitialized::Constructor);
    c.x = (y * rhs.z) - (z * rhs.y);
    c.y = (z * rhs.x) - (x * rhs.z);
    c.z = (x * rhs.y) - (y * rhs.x);
    return c;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Angle(const Vec3& rhs) const
{
    return std::acos(Normalized().Dot(rhs.Normalized()));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline T Vec3<T>::Angle_Safe(const Vec3& rhs) const
{
    return std::acos(SafeNormalized().Dot(rhs.SafeNormalized()));
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::ParallelProjection(const Vec3& rhs) const
{
    Vec3 n = Normalized();
    const T l = n.Dot(rhs);
    n.Scale(l);
    return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::ParallelProjection_Safe(const Vec3& rhs) const
{
    Vec3 n = SafeNormalized();
    const T l = n.Dot(rhs);
    n.Scale(l);
    return n;
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::PerpendicularProjection(const Vec3& rhs) const
{
    return rhs - ParallelProjection(rhs);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline Vec3<T> Vec3<T>::PerpendicularProjection_Safe(const Vec3& rhs) const
{
    return rhs - ParallelProjection_Safe(rhs);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::SelectMinBetween(const Vec3<T>& rhs)
{
    x = std::min(x, rhs.x);
    y = std::min(y, rhs.y);
    z = std::min(z, rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
inline void Vec3<T>::SelectMaxBetween(const Vec3<T>& rhs)
{
    x = std::max(x, rhs.x);
    y = std::max(y, rhs.y);
    z = std::max(z, rhs.z);
}

///////////////////////////////////////////////////////////////////////
template <typename T>
const Vec3<T> Vec3<T>::Zero(EZero::Constructor);
template <typename T>
const Vec3<T> Vec3<T>::Up(0, 1, 0);
template <typename T>
const Vec3<T> Vec3<T>::Down(0, -1, 0);
template <typename T>
const Vec3<T> Vec3<T>::Left(-1, 0, 0);
template <typename T>
const Vec3<T> Vec3<T>::Right(1, 0, 0);
template <typename T>
const Vec3<T> Vec3<T>::Forward(0, 0, -1);
template <typename T>
const Vec3<T> Vec3<T>::Back(0, 0, 1);

///////////////////////////////////////////////////////////////////////
typedef Vec3<fworld> Vec3w;
typedef Vec3<flocal> Vec3l;
typedef Vec3<int32_t> Vec3i;
typedef Vec3<uint32_t> Vec3u;

///////////////////////////////////////////////////////////////////////
typedef Vec3w Point3w;
typedef Vec3l Point3l;
typedef Vec3i Point3i;
typedef Vec3u Point3u;
