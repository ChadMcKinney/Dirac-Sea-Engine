
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
