/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "vector_tests.h"

#include <cassert>

#include "tests/test_framework.h"
#include "vector.h"

void RunVec2Tests()
{
    Vec2w a(1, 2);
    Vec2w b(4, 5);
    Vec2w c = a + b;
    TEST("Vec2: (1,2) + (4,5)", c.x == fworld(5) && c.y == fworld(7));
}

void RunVec3Tests()
{
    Vec3w a(1, 2, 3);
    Vec3w b(4, 5, 6);
    Vec3w c = a + b;
    TEST("Vec3: (1,2,3) + (4,5,6)", c.x == fworld(5) && c.y == fworld(7) && c.z == fworld(9));
}

void RunVectorTests()
{
  RunVec2Tests();
  RunVec3Tests();
}
