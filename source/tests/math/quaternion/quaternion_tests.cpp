/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "quaternion_tests.h"

#include "math/quaternion.h"
#include "tests/test_framework.h"

template <typename T>
void RunQuaternionTest_tpl()
{
  {
    Quaternion<T> q(EZero::Constructor);
    Quaternion<T> q2(EZero::Constructor);
    TEST("quaternion: ZERO == ZERO", q == q2);
  }

  {
    Quaternion<T> q(EIdentity::Constructor);
    Quaternion<T> q2(EIdentity::Constructor);
    TEST("quaternion: IDENTITY == IDENTITY", q == q2);
  }

  {
    Quaternion<T> q(EIdentity::Constructor);
    Quaternion<T> q2(0, 0, 0, 1 - epsilon<T>());
    TEST("quaternion: IDENTITY == IDENTITY - epsilon", q.IsEquivalent(q2, epsilon<T>() * 2));
  }

  {
    Quaternion<T> q = Quaternion<T>::CreateAxisAngle(Vec3<T>(1, 0, 0), 0);
    Quaternion<T> q2(EIdentity::Constructor);
    TEST("quaternion: IDENTITY == AA_ZERO", q == q2);
  }
}

void RunQuaternionTests()
{
  RunQuaternionTest_tpl<flocal>();
  RunQuaternionTest_tpl<fworld>();
}
