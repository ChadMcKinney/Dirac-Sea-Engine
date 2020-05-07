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

	{
		Vec2<T> a(1, 1);
		Vec2<T> b(1, 1);
		T c = a.Dot(b);
		TEST("Vec2: (1, 1).Dot(1, 1)", c == T(2));
	}
}

template <typename T>
void RunSignedVec2Tests()
{
	{
		Vec2<T> a(1, -1);
		Vec2<T> b(-1, 1);
		T c = a.Dot(b);
		TEST("Vec2: (1, -1).Dot(-1, 1)", c == T(-2));
	}

	{
		Vec2<T> a(3, -2);
		Vec2<T> b(0, 4);
		T c = a.Dot(b);
		TEST("Vec2: (3, -2).Dot(0, 4)", c == T(-8));
	}
}

template <typename T>
void RunFloatingVec2Tests()
{
	{
		Vec2<T> a(T(0.6), T(0.8));
		Vec2<T> b(T(-0.6), T(-0.8));
		T c = a.Dot(b);
		TEST("Vec2: (0.6, 0.8).Dot(-0.6, -0.8)", c == T(-1));
	}

	{
		Vec2<T> a(3, 4);
		T magnitude = a.Magnitude();
		TEST("Vec2: (3, 4).Magnitude()", magnitude == T(5));
	}

	{
		Vec2<T> a(epsilon<T>() * T(0.1), epsilon<T>() * T(0.1));
		Vec2<T> aNormalized = a.SafeNormalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec2: (epsilon * 0.1, epslion * 0.1).SafeNormalized().Magnitude()", magnitude == T(0));
	}

	{
		Vec2<T> a(T(1.337), T(666));
		Vec2<T> aNormalized = a.Normalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec2: (1.337, 666).SafeNormalized().Magnitude()", magnitude == T(1));
	}


	{
		Vec2<T> a(T(3), T(4));
		T sqrMagnitude = a.SqrMagnitude();
		TEST("Vec3: (3, 4).SqrMagnitude()", sqrMagnitude == T(25));
	}

	{
		Vec2<T> a(5, 0);
		Vec2<T> b(-1, 8);
		T d = a.Distance(b);
		TEST("Vec2: (5, 0).Distance(-1, 8)", d == 10);
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

	{
		Vec3<T> a(1, 1, 1);
		Vec3<T> b(1, 1, 1);
		T c = a.Dot(b);
		TEST("Vec3: (1, 1, 1).Dot(1, 1, 1)", c == T(3));
	}
}

template <typename T>
void RunSignedVec3Tests()
{
	{
		Vec3<T> a(-1, 1, -1);
		Vec3<T> b(1, -1, 1);
		T c = a.Dot(b);
		TEST("Vec3: (-1, 1, -1).Dot(1, -1, 1)", c == T(-3));
	}

	{
		Vec3<T> a(3, -2, 7);
		Vec3<T> b(0, 4, -1);
		T c = a.Dot(b);
		TEST("Vec3: (3, -2, 7).Dot(0, 4, -1)", c == T(-15));
	}
}

template <typename T>
void RunFloatingVec3Tests()
{
	{
		Vec3<T> a(T(0.5), T(4) / T(6), T(-3.3166247903554 / 6.0));
		Vec3<T> b(T(-0.5), -(T(4) / T(6)), T(3.3166247903554 / 6.0));
		T c = a.Dot(b);
		TEST("Vec3: (0.5, 0.666666666..., -1).Dot(-0.5, -0.66666..., 1)", c == T(-1));
	}

	{
		Vec3<T> a(T(0.5), T(4) / T(6), T(-3.3166247903554 / 6.0));
		T magnitude = a.Magnitude();
		TEST("Vec3: (0.5, 0.666666666..., -1).Magnitude()", magnitude == T(1));
	}

	{
		Vec3<T> a(epsilon<T>() * T(0.1), epsilon<T>() * T(0.1), epsilon<T>() * T(0.1));
		Vec3<T> aNormalized = a.SafeNormalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec2: (epsilon, epslion).Magnitude()", magnitude == T(0));
	}
	
	{
		Vec3<T> a(T(1.337), 666, 9999999);
		Vec3<T> aNormalized = a.Normalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec3: (1.337, 666, 9999999).SafeNormalized().Magnitude()", magnitude == T(1));
	}

	{
		Vec3<T> a(T(3), T(4), T(-3.3166247903554));
		T sqrMagnitude = a.SqrMagnitude();
		TEST("Vec3: (3, 4, -3.3166247903554).SqrMagnitude()", sqrMagnitude == T(36));
	}

	{
		Vec3<T> a(5, 6, -7);
		Vec3<T> b(-1, 8, 1337);
		T d = a.Distance(b);
		TEST("Vec3: (5,6,7).Distance(-1,8,1337)", d == std::sqrt(T(1806376)));
	}
}

void RunVec2Tests()
{
	puts("=====================================================");
  puts("Running Vec2w tests");
  RunVec2Tests<fworld>();
	RunSignedVec2Tests<fworld>();
	RunFloatingVec2Tests<fworld>();

	puts("=====================================================");
  puts("Running Vec2l tests");
  RunVec2Tests<flocal>();
	RunSignedVec2Tests<flocal>();
	RunFloatingVec2Tests<flocal>();

	puts("=====================================================");
  puts("Running Vec2i tests");
  RunVec2Tests<int32_t>();
	RunSignedVec2Tests<int32_t>();

	puts("=====================================================");
  puts("Running Vec2u tests");
  RunVec2Tests<uint32_t>();
}

void RunVec3Tests()
{
	puts("=====================================================");
  puts("Running Vec3w tests");
  RunVec3Tests<fworld>();
	RunSignedVec3Tests<fworld>();
	RunFloatingVec3Tests<fworld>();

	puts("=====================================================");
  puts("Running Vec3l tests");
  RunVec3Tests<flocal>();
	RunSignedVec3Tests<flocal>();
	RunFloatingVec3Tests<flocal>();

	puts("=====================================================");
  puts("Running Vec3i tests");
  RunVec3Tests<int32_t>();
	RunSignedVec3Tests<int32_t>();

	puts("=====================================================");
  puts("Running Vec3u tests");
  RunVec3Tests<uint32_t>();
}

void RunVectorTests()
{
  RunVec2Tests();
  RunVec3Tests();
}
