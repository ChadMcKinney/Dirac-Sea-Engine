/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "vector_tests.h"

#include <cassert>

#include "tests/test_framework.h"
#include "vector.h"

template <typename T>
void RunVec2Tests()
{
	{
		Vec2<T> a(1, 2);
		Vec2<T> b(4, 5);
		Vec2<T> c = a + b;
		TEST("Vec2: (1,2) + (4,5)", c.x == T(5) && c.y == T(7));
	}

	{
		Vec2<T> a(5, 6);
		Vec2<T> b(4, 5);
		Vec2<T> c = a - b;
		TEST("Vec2: (5,6) - (4,5)", c.x == T(1) && c.y == T(1));
	}
}

template <typename T>
void RunVec3Tests()
{
	{
		Vec3<T> a(1, 2, 3);
		Vec3<T> b(4, 5, 6);
		Vec3<T> c = a + b;
		TEST("Vec3: (1,2,3) + (4,5,6)", c.x == T(5) && c.y == T(7) && c.z == T(9));
	}

	{
		Vec3<T> a(5, 6, 7);
		Vec3<T> b(4, 5, 6);
		Vec3<T> c = a - b;
		TEST("Vec3: (5,6,7) - (4,5,6)", c.x == T(1) && c.y == T(1) && c.z == T(1));
	}
}

void RunVec2Tests()
{
  puts("Running Vec2w tests");
  RunVec2Tests<fworld>();

  puts("Running Vec2l tests");
  RunVec2Tests<flocal>();

  puts("Running Vec2i tests");
  RunVec2Tests<int32_t>();

  puts("Running Vec2u tests");
  RunVec2Tests<uint32_t>();
}

void RunVec3Tests()
{
  puts("Running Vec3w tests");
  RunVec3Tests<fworld>();

  puts("Running Vec3l tests");
  RunVec3Tests<flocal>();

  puts("Running Vec3i tests");
  RunVec3Tests<int32_t>();

  puts("Running Vec3u tests");
  RunVec3Tests<uint32_t>();
}

void RunVectorTests()
{
  RunVec2Tests();
  RunVec3Tests();
}
