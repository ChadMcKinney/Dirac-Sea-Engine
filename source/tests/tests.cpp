/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "tests.h"

#include "tests/math/quaternion/quaternion_tests.h"
#include "tests/math/matrix/matrix_tests.h"
#include "tests/math/vector/vector_tests.h"

void RunTests()
{
  RunVectorTests();
  RunMatrixTests();
  RunQuaternionTests();
}