
/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "matrix_tests.h"

#include <cassert>

#include "tests/test_framework.h"
#include "matrix22.h"

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
}

void RunMatrixTests()
{
	puts("=====================================================");
  puts("Running Matrix2x2<fworld> tests");
  RunMatrix2x2Tests<fworld>();

	puts("=====================================================");
  puts("Running Matrix2x2<flocal> tests");
  RunMatrix2x2Tests<flocal>();
}
