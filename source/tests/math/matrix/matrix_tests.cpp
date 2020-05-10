
/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "matrix_tests.h"

#include <cassert>

#include "math/matrix22.h"
#include "math/matrix33.h"
#include "tests/test_framework.h"

template <typename T>
void RunMatrix2x2Tests()
{
  {
		Matrix22<T> m;
		TEST("m22: m() == 0 0 0 0", m.m11 == 0 && m.m12 == 0 && m.m21 == 0 && m.m22 == 0);
  }

  {
    Matrix22<T> m(1, 2, 3, 4);
    m.Transpose();
    TEST("m22: ((1, 2) (3, 4)).Transpose()", m.m11 == 1 && m.m12 == 3 && m.m21 == 2 && m.m22 == 4);
  }

  {
    Matrix22<T> m(1, 2, 3, 4);
    Matrix22<T> m2(m);
    Matrix22<T> m3 = m2.Transposed();
    m.Transpose();
    TEST("m22: ((1, 2) (3, 4)).Transpose() == Transposed", m == m3);
  }

  {
    Matrix22<T> m(1, 2, 3, 4);
    Matrix22<T> m2 = m.Transposed().Transposed();
    TEST("m22: m.Transposed().Transposed() == m", m == m2);
  }

  {
    Matrix22<T> m(-3, 0, 5, T(0.5));
    Matrix22<T> m2(-7, 2, 4, 6);
    Matrix22<T> m3(21, -6, -33, 13);
    TEST("m22: m * m2 == m3", (m * m2) == m3 &&  m != m3);
  }

  {
    Matrix22<T> m(-3, 0, 5, T(0.5));
    Matrix22<T> m2(-7, 2, 4, 6);
    TEST("m22: (m * m2)T == (m2T * mT)", (m * m2).Transposed() == (m2.Transposed() * m.Transposed()));
  }

  {
    Matrix22<T> m(-3, 0, 5, T(0.5));
    Vec2<T> v(-1, 1);
    TEST("m22: (m * v) != (v * m)", (m * v) != (v * m));
  }

  {
    Matrix22<T> m(-3, 0, 5, T(0.5));
    Vec2<T> v(-1, 1);
    TEST("m22: (m * v) == (v * m.Transposed())", (m * v) == (v * m.Transposed()));
  }

  {
    Vec2<T> v(3, 3);
    Matrix22<T> m(6, -7, -4, 5);
    Vec2<T> v2 = v * m;
    Vec2<T> v3(6, -6);
    TEST("m22: v * m == v3", v2 == v3);
  }

  {
    T two(2);
    Matrix22<T> m(11, 12, 21, 22);
    Matrix22<T> m2(22, 24, 42, 44);
    TEST("m22: 2 * m == m * 2", (m * two) == (two * m) && (two * m) == m2);
  }

  {
    Matrix22<T> m = Matrix22<T>::CreateScale(Vec2<T>(2, 2));
    Vec2<T> v(3, 3);
    Vec2<T> v2(6, 6);
    TEST("m22: (m.Scale(2, 2) * v) == v2", (v * m) == v2);
  }

  {
    Matrix22<T> m = Matrix22<T>::CreateScale(Vec2<T>(2, 3));
    Vec2<T> v(3, 3);
    Vec2<T> v2(6, 9);
    TEST("m22: non-uniform scale", (v * m) == v2);
  }

  {
    Matrix22<T> m(1, 2, 3, 4);
    Matrix22<T> m2 = m.Inverted();
    Matrix22<T> m3 = m2.Inverted();
    TEST("m22: m.Inverted().Inverted() == m", m == m3);
  }

  {
    Matrix22<T> m(EIdentity::Constructor);
    Matrix22<T> m2 = m.Inverted();
    TEST("m22: Identity == Identity.Inverted()", m == m2);
  }

  {
    Matrix22<T> m(1, 2, 3, 4);
    TEST("m22: m.Transposed().Inverted() == m.Inverted().Transposed()", m.Transposed().Inverted() == m.Inverted().Transposed());
  }

  {
    Matrix22<T> m(1, 0, 0, T(0.8));
    Matrix22<T> m2 = m.Orthonormalized();
    TEST("m22: ((1, 0) (0, 0.8)).Orthonormalize() == ((1, 0) (0, 1))", m2 == Matrix22<T>(1, 0, 0, 1));
  }

  {
    Matrix22<T> m(T(0.8), 0, 0, 1);
    Matrix22<T> m2 = m.Orthonormalized();
    TEST("m22: ((1, 0) (0, 0.8)).Orthonormalize() == ((1, 0) (0, 1))", m2 == Matrix22<T>(1, 0, 0, 1));
  }
}

template <typename T>
void RunMatrix3x3Tests()
{
  {
    Matrix33<T> m;
    Matrix33<T> m2;
    TEST("m33: m == m2", m == m2);
  }

  {
    Matrix33<T> m(11, 12, 13, 21, 22, 23, 31, 32, 33);
    Matrix33<T> m2 = m * Matrix33<T>(EIdentity::Constructor);
    TEST("m33: (m * Identity) == m", m2 == m);
  }

  {
    Matrix33<T> m(11, 12, 13, 21, 22, 23, 31, 32, 33);
    Matrix33<T> mz(EZero::Constructor);
		Matrix33<T> m2 = m * mz;
    TEST("m33: (m * zero) == zero", m2 == mz);
  }

  {
    Matrix33<T> m(11, 12, 13, 21, 22, 23, 31, 32, 33);
    TEST("m33: m.Transposed().Transposed() == m", m == m.Transposed().Transposed());
  }

  {
    Matrix33<T> m(11, 12, 13, 21, 22, 23, 31, 32, 33);
    Matrix33<T> m2(11, 21, 31, 12, 22, 32, 13, 23, 33);
    TEST("m33: m.Transposed() == m2", m.Transposed() == m2);
  }

  {
    Matrix33<T> m(11, 12, 13, 21, 22, 23, 31, 32, 33);
    Matrix33<T> m2(111, 122, 133, 211, 222, 233, 311, 322, 333);
    TEST("m33: (m * m2)T == (m2T * mT)", (m * m2).Transposed() == (m2.Transposed() * m.Transposed()));
  }

  {
    const T radians = DegreesToRadians(T(-22.0));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationX(radians);
    Matrix33<T> m2(1, 0, 0, 0, cosTheta, sinTheta, 0, -sinTheta, cosTheta);
    TEST("m33: RotationX == m2", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(30));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationY(radians);
    Matrix33<T> m2(cosTheta, 0, -sinTheta, 0, 1, 0, sinTheta, 0, cosTheta);
    TEST("m33: RotationZ == m2", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(30));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationZ(radians);
    Matrix33<T> m2(cosTheta, sinTheta, 0, -sinTheta, cosTheta, 0, 0, 0, 1);
    TEST("m33: RotationZ == m2", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(-22.0));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationX(radians);
    Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(radians, Vec3<T>(1, 0, 0));
    TEST("m33: RotationX == AA", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(30));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationY(radians);
    Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(radians, Vec3<T>(0, 1, 0));
    TEST("m33: RotationZ == AA", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(30));
    const T cosTheta = std::cos(radians);
    const T sinTheta = std::sin(radians);
    Matrix33<T> m = Matrix33<T>::CreateRotationZ(radians);
    Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(radians, Vec3<T>(0, 0, 1));
    TEST("m33: RotationZ == AA", m == m2);
  }

  {
    const T radians = DegreesToRadians(T(-15));
    Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(radians, Vec3<T>(T(0.267), T(-0.535), T(0.802)));
    Matrix33<T> m(T(0.968), T(-0.212), T(-0.131), T(0.203), T(0.976), T(-0.084), T(0.146), T(0.054), T(0.988));
    TEST("m33: AA == AA", m.IsEquivalent(m2, T(0.0005)));
  }

  {
    Matrix33<T> m(EIdentity::Constructor);
    Matrix33<T> m2 = m * T(666);
    Matrix33<T> m3(666, 0, 0, 0, 666, 0, 0, 0, 666);
    TEST("m33: m * 666 == m2", m2 == m3 && m3 == (T(2) * m * T(333)));
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateScale(Vec3<T>(2, 2, 3));
    Vec3<T> v(2, 2, 2);
    Vec3<T> v2(4, 4, 6);
    TEST("m33: m * v == (4,4,6)", (v * m) == v2);
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateRotationZ(kPi<T>);
    Vec3<T> v(1, 0, 0);
    Vec3<T> v2(-1, 0, 0);
    Vec3<T> v3 = v * m;
    TEST("m33: m * v == (0, -1, 0)", v2.IsEquivalent(v3, epsilon<T>()));
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateReflection(Vec3<T>(0, 1, 0));
    Vec3<T> v(1, 1, 1);
    Vec3<T> v2(1, -1, 1);
    Vec3<T> v3 = v * m;
    TEST("m33: (1, 0, 0) * reflection == (-1, 0, 0)", v2 == v3);
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateXYPlaneProjection();
    Vec3<T> v(1, 1, 1);
    Vec3<T> v2(1, 1, 0);
    Vec3<T> v3 = v * m;
    TEST("m33: (1, 1, 1) * XY Project == (1, 1, 0)", v2 == v3);
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateYZPlaneProjection();
    Vec3<T> v(1, 1, 1);
    Vec3<T> v2(0, 1, 1);
    Vec3<T> v3 = v * m;
    TEST("m33: (1, 1, 1) * YZ Project == (1, 1, 0)", v2 == v3);
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateZXPlaneProjection();
    Vec3<T> v(1, 1, 1);
    Vec3<T> v2(1, 0, 1);
    Vec3<T> v3 = v * m;
    TEST("m33: (1, 1, 1) * ZX Project == (1, 1, 0)", v2 == v3);
  }

  {
    Vec3<T> planeNormal = Vec3<T>(1, 1, 1);
    planeNormal.Normalize();
    Matrix33<T> m = Matrix33<T>::CreatePlaneProjection(planeNormal);
    Vec3<T> v(1, 1, 1);
    v.Normalize();
    v.Scale(6);
    Vec3<T> v2(EZero::Constructor);
    Vec3<T> v3 = v * m;
    TEST("m33: (1,1,1).Project((1,1,1).Normalized())", v2.IsEquivalent(v3, epsilon<T>()*10));
  }

  {
    TEST("m33: Identity.Determinant == 1", Matrix33<T>(EIdentity::Constructor).Determinant() == T(1));
  }

  {
    TEST("m33: Identity.Determinant == 1", Matrix33<T>::CreateXYPlaneProjection().Determinant() == T(0));
  }

  {
    Matrix33<T> m(EIdentity::Constructor);
    Matrix33<T> m2 = m.Inverted();
    TEST("m33: Identity.Inverted() == Identity", m == m2);
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateReflection(Vec3<T>(-1, T(0.3), kPi<T>).Normalized());
    Matrix33<T> m2 = m.Inverted();
    Matrix33<T> m3 = m2.Inverted();
    TEST("m33: m.Inverted().Inverted() == m", m.IsEquivalent(m3, epsilon<T>() * 10));
  }

  {
    Matrix33<T> m = Matrix33<T>::CreateRotationAA(kPi<T>, Vec3<T>(1, 0, 0));
    Matrix33<T> m2 = m.Inverted();
    Matrix33<T> m3 = m2.Inverted();
    TEST("m33: m.Inverted().Inverted() == m", m == m3);
  }

  {
    Matrix33<T> m(1, 0, 0, 0, T(0.8), 0, 0, 0, T(0.75));
    Matrix33<T> m2 = m.Orthonormalized();
    Matrix33<T> m3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    TEST("m22: orthonormalize", m2 == m3);
  }

  {
    Matrix33<T> m(T(0.3), 0, 0, 0, T(0.8), 0, 0, 0, T(0.76));
    Matrix33<T> m2 = m.Orthonormalized();
    Matrix33<T> m3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    TEST("m22: orthonormalize", m2 == m3);
  }


  {
    Matrix33<T> m(T(0.3), 0, 0, 0, T(0.8), 0, 0, 0, T(0.76));
    Matrix33<T> m2 = m.Orthonormalized_Safe();
    Matrix33<T> m3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    TEST("m22: orthonormalize safe", m2 == m3);
  }
}

void RunMatrixTests()
{
	puts("=====================================================");
  puts("Running Matrix2x2<fworld> tests");
  RunMatrix2x2Tests<fworld>();

	puts("=====================================================");
  puts("Running Matrix2x2<flocal> tests");
  RunMatrix2x2Tests<flocal>();

	puts("=====================================================");
  puts("Running Matrix3x3<fworld> tests");
  RunMatrix3x3Tests<fworld>();

	puts("=====================================================");
  puts("Running Matrix3x3<flocal> tests");
  RunMatrix3x3Tests<flocal>();
}
