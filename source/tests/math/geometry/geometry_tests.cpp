/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "geometry_tests.h"

#include "geometry/aabb.h"
#include "geometry/line.h"
#include "geometry/plane.h"
#include "geometry/ray.h"
#include "geometry/sphere.h"
#include "geometry/triangle.h"
#include "test_framework.h"

template <typename T>
void RunLineTests()
{
}

template <typename T>
void RunLineSegmentTests()
{
}

template <typename T>
void RunRayTests()
{
}

template <typename T>
void RunSphereTests()
{
}

template <typename T>
void RunAABBTests()
{
    {
        AABB<T> aabb(EEmpty::Constructor);
        TEST("aabb: empty min > max", aabb.min.x > aabb.max.x && aabb.min.y > aabb.max.y && aabb.min.z > aabb.max.z);
    }

    {
        AABB<T> aabb = AABB<T>::CreateAABBFromPair(Vec3<T>(-1, 2, -3), Vec3<T>(-4, 3, -7));
        AABB<T> aabb2(Vec3<T>(-4, 2, -7), Vec3<T>(-1, 3, -3));
        TEST("aabb: create from pair", aabb == aabb2);
    }

    {
        Vec3<T> points[] = { Vec3<T>(-1, 2, -3), Vec3<T>(-4, 3, -7) };
        AABB<T> aabb = AABB<T>::CreateAABBFromPoints(points, 2);
        AABB<T> aabb2(Vec3<T>(-4, 2, -7), Vec3<T>(-1, 3, -3));
        TEST("aabb: create from points", aabb == aabb2);
    }

    {
        AABB<T> a(Vec3<T>(1, 2, 3), Vec3<T>(4, 5, 6));
        AABB<T> b(Vec3<T>(-1, -2, -3), Vec3<T>(7, 8, 9));
        a.MergeAABB(b);
        TEST("aabb: merge aabbs", a == b);
    }
}

template <typename T>
void RunPlaneTests()
{
    {
        Vec3<T> a(1, 2, -2);
        Vec3<T> n = Vec3<T>(11, 16, 14).Normalized();
        Plane<T> p(a, Vec3<T>(3, -2, 1), Vec3<T>(5, 1, -4));
        Plane<T> p2(n, a.Dot(n));
        TEST("plane: from 3 points", p.IsEquivalent(p2, epsilon<T>()));
    }

    {
        Plane<T> p(Vec3<T>(1, 2, -2), Vec3<T>(3, -2, 1), Vec3<T>(5, 1, -4));
        Vec3<T> v(666, 1337, 4);
        T d = p.Distance(v);
        TEST("plane: from 3 points", (abs(d - T(1201.4244606838558)) < 0.001));
    }
}

template <typename T>
void RunTriangleTests()
{
    {
        for (size_t i = 0; i < 255; ++i)
        {
            T frac = T(i) / T(256);

            Vec3<T> v1(1, 2, -2);
            Vec3<T> v2(3, -2, 1);
            Vec3<T> v3(5, 1, -4);
            T b1 = frac;
            T b2 = b1 < 0.5 ? T(0.5) - b1 : 0;
            T b3 = 1 - (b1 + b2);

            Vec3<T> p = triangle::GetCartesianFromBarycentric(v1, v2, v3, b1, b2, b3);

            T b12;
            T b22;
            T b32;
            triangle::GetBarycentricFromCartesian(v1, v2, v3, p, b12, b22, b32);
            TEST("triangle: barycentric <-> cartesian", Vec3<T>(b1, b2, b3).IsEquivalent(Vec3<T>(b12, b22, b32), epsilon<T>() * 4));

            Vec3<T> p2 = triangle::GetCartesianFromBarycentric(v1, v2, v3, b12, b22, b32);
            TEST("triangle: barycentric <-> cartesian", p.IsEquivalent(p2, epsilon<T>() * 8));
        }
    }

    {
        Vec3<T> v1(1, 2, -2);
        Vec3<T> v2(3, -2, 1);
        Vec3<T> v3(5, 1, -4);
        Vec3<T> p = v1.Scaled(T(0.5)) + v2.Scaled(T(0.25)) + v3.Scaled(T(0.25));

        T b1, b2, b3;
        triangle::GetBarycentricFromCartesian(v1, v2, v3, p, b1, b2, b3);
        Vec3<T> p2 = triangle::GetCartesianFromBarycentric(v1, v2, v3, b1, b2, b3);
        TEST("triangle: cartesion <-> barycentric", p.IsEquivalent(p2, epsilon<T>() * 4));
    }
}

void RunGeometryTests()
{
    RunLineTests<flocal>();
    RunLineTests<fworld>();

    RunLineSegmentTests<flocal>();
    RunLineSegmentTests<fworld>();

    RunRayTests<flocal>();
    RunRayTests<fworld>();

    RunSphereTests<flocal>();
    RunSphereTests<fworld>();

    RunAABBTests<flocal>();
    RunAABBTests<fworld>();

    RunPlaneTests<flocal>();
    RunPlaneTests<fworld>();

    RunTriangleTests<flocal>();
    RunTriangleTests<fworld>();
}
