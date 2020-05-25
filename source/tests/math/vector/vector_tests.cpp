/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "vector_tests.h"

#include <cassert>

#include "tests/test_framework.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"

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

	{
		Vec2<T> a(1, 1);
		a.Scale(666);
		TEST("Vec2: (1,1).Scale", a.x == 666 && a.y == 666);
	}

	{
		Vec2<T> a(1, 1);
		Vec2<T> b = a.Scaled(666);
		TEST("Vec2: (1,1).Scaled", a.x == 1 && a.y == 1 && b.x == 666 && b.y == 666);
	}

	{
		Vec2<T> a(1, 3);
		Vec2<T> b(1, 3);
		TEST("Vec2: (1,3) == (1, 3)", a == b);
	}

	{
		Vec2<T> a(1, 3);
		Vec2<T> b(0, 3);
		TEST("Vec2: (1,3) != (0, 3)", a != b);
	}

	{
		Vec2<T> a(1, 3);
		Vec2<T> b(1, 0);
		TEST("Vec2: (1,3) != (1, 0)", a != b);
	}

	{
		Vec2<T> a(1, 2);
		Vec2<T> b(3, 4);
		TEST("Vec2: (1,2) + (3,4) == (3,4) + (1, 2)", (a + b) == (b + a));
	}

	{
		Vec2<T> a(1, 2);
		Vec2<T> b(3, 4);
		Vec2<T> c(5, 6);
		TEST("Vec2: ((1,2) + (3,4)) + (5,6) == (1,2) + ((3,4) + (5,6))", (a + b) + c == a + (b + c));
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

	{
		Vec2<T> a(1, 2);
		Vec2<T> b(4, 5);
		TEST("Vec2: (1,2) - (4,5) == (1,2) + -(4,5)", (a - b) == (a + b.Scaled(T(-1))));
	}

	{
		Vec2<T> a(1, 2);
		TEST("Vec2: 2 * (3 * (1,2)) == 2 * 3 * (1,2)", a.Scaled(2).Scaled(3) == a.Scaled(T(2) * T(3)));
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

	{
		T angle = Vec2<T>::Up.Angle(Vec2<T>::Right);
		TEST("Vec2: (0, 1).Angle(1, 0)", angle == kHalfPi<T>);
	}

	{
		T angle = Vec2<T>::Left.Angle(Vec2<T>::Right);
		TEST("Vec3: (-1, 0).Angle(1, 0)", angle == kPi<T>);
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

	{
		Vec3<T> a(1, 1, 1);
		a.Scale(666);
		TEST("Vec3: (1,1,1).Scale", a.x == 666 && a.y == 666 && a.z == 666);
	}

	{
		Vec3<T> a(1, 1, 1);
		Vec3<T> b = a.Scaled(666);
		TEST("Vec3: (1,1,1).Scaled", a.x == 1 && a.y == 1 && a.z == 1 && b.x == 666 && b.y == 666 && b.z == 666);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(1, 3, 4);
		TEST("Vec3: (1,3,4) == (1, 3, 4)", a == b);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(0, 3, 4);
		TEST("Vec3: (1,3,4) != (0, 3, 4)", a != b);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(1, 0, 4);
		TEST("Vec3: (1,3,4) != (1, 0, 4)", a != b);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(1, 3, 0);
		TEST("Vec3: (1,3,4) != (1, 3, 0)", a != b);
	}

	{
		Vec3<T> a(1, 2, 3);
		Vec3<T> b(4, 5, 6);
		TEST("Vec3: (1,2,3) + (4,5,6) == (4,5,6) + (1,2,3)", (a + b) == (b + a));
	}

	{
		Vec3<T> a(1, 2, 3);
		Vec3<T> b(3, 4, 5);
		Vec3<T> c(5, 6, 7);
		TEST("Vec3: ((1,2,3) + (3,4,5)) + (5,6,7) == (1,2,3) + ((3,4,5) + (5,6,7))", (a + b) + c == a + (b + c));
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

	{
		Vec3<T> a(1, 2, 3);
		Vec3<T> b(4, 5, 6);
		TEST("Vec3: (1,2,3) - (4,5,6) == (1,2,3) + -(4,5,6)", (a - b) == (a + b.Scaled(T(-1))));
	}

	{
		Vec3<T> a(1, 2, 3);
		TEST("Vec3: 2 * (3 * (1,2,3)) == 2 * 3 * (1,2,3)", a.Scaled(2).Scaled(3) == a.Scaled(T(2) * T(3)));
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

	{
		Vec3<T> a(4, 0, 1);
		Vec3<T> b(3, 5, -2);
		Vec3<T> c = a.ParallelProjection(b);
		TEST("Vec3: (4, 0, 1).ParallelProjection(3, 5, -2)", abs(c.x - (T(40) / T(17))) <= (epsilon<T>()*2) && c.y == 0 && abs(c.z - (T(10) / T(17))) <= epsilon<T>());
	}


	{
		Vec3<T> a(4, 0, 1);
		Vec3<T> b(3, 5, -2);
		Vec3<T> c = a.PerpendicularProjection(b);
		TEST("Vec3: (4, 0, 1).PerpendicularProjection(3, 5, -2)", abs(c.x - (T(11) / T(17))) <= (epsilon<T>()*3) && c.y == 5 && abs(c.z - (T(-44) / T(17))) <= epsilon<T>());
	}

	{
		T angle = Vec3<T>::Up.Angle(Vec3<T>::Right);
		TEST("Vec3: (0, 1, 0).Angle(1, 0, 0)", angle == kHalfPi<T>);
	}

	{
		T angle = Vec3<T>::Forward.Angle(Vec3<T>::Back);
		TEST("Vec3: (0, 0, 1).Angle(0, 0, -1)", angle == kPi<T>);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(2, -5, 8);
		Vec3<T> c = a.Cross(b);
		TEST("Vec3: (1,3,4).Cross(2,-5,8)", c.x == 44 && c.y == 0 && c.z == -11);
	}

	{
		Vec3<T> a(1, 3, 4);
		Vec3<T> b(EZero::Constructor);
		Vec3<T> c = a.Cross(b);
		TEST("Vec3: (1,3,4).Cross(0, 0, 0)", c == b);
	}
}

template <typename T>
void RunVec4Tests()
{
	{
		Vec4<T> a(T(0.5), T(4) / T(6), T(-3.3166247903554 / 6.0), 1);
		a.Normalize();
		Vec4<T> b(T(-0.5), -(T(4) / T(6)), T(3.3166247903554 / 6.0), -1);
		b.Normalize();
		T c = a.Dot(b);
		TEST("Vec4: (0.5, 0.666666666..., -1).Dot(-0.5, -0.66666..., 1)", c < 0 && abs(c - -1) < epsilon<T>() * 2);
	}

	{
		Vec4<T> a(T(0.5), T(4) / T(6), T(-3.3166247903554 / 6.0), 0);
		T magnitude = a.Magnitude();
		TEST("Vec4: (0.5, 0.666666666..., -1).Magnitude()", magnitude == T(1));
	}

	{
		Vec4<T> a(epsilon<T>() * T(0.1), epsilon<T>() * T(0.1), epsilon<T>() * T(0.1), epsilon<T>() * T(0.1));
		Vec4<T> aNormalized = a.SafeNormalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec2: (epsilon, epslion).Magnitude()", magnitude == T(0));
	}
	
	{
		Vec4<T> a(T(1.337), 666, 9999999, -6);
		Vec4<T> aNormalized = a.Normalized();
		T magnitude = aNormalized.Magnitude();
		TEST("Vec4: (1.337, 666, 9999999).SafeNormalized().Magnitude()", magnitude == T(1));
	}

	{
		Vec4<T> a(T(3), T(4), T(-3.3166247903554), 1);
		T sqrMagnitude = a.SqrMagnitude();
		TEST("Vec4: (3, 4, -3.3166247903554).SqrMagnitude()", sqrMagnitude == T(37));
	}

	{
		Vec4<T> a(0, 5, 6, -7);
		Vec4<T> b(0, -1, 8, 1337);
		T d = a.Distance(b);
		TEST("Vec4: (5,6,7).Distance(-1,8,1337)", d == std::sqrt(T(1806376)));
	}
}

void RunVec2Tests()
{
  RunVec2Tests<fworld>();
	RunSignedVec2Tests<fworld>();
	RunFloatingVec2Tests<fworld>();

  RunVec2Tests<flocal>();
	RunSignedVec2Tests<flocal>();
	RunFloatingVec2Tests<flocal>();

  RunVec2Tests<int32_t>();
	RunSignedVec2Tests<int32_t>();

  RunVec2Tests<uint32_t>();
}

void RunVec3Tests()
{
  RunVec3Tests<fworld>();
	RunSignedVec3Tests<fworld>();
	RunFloatingVec3Tests<fworld>();

  RunVec3Tests<flocal>();
	RunSignedVec3Tests<flocal>();
	RunFloatingVec3Tests<flocal>();

  RunVec3Tests<int32_t>();
	RunSignedVec3Tests<int32_t>();

  RunVec3Tests<uint32_t>();
  RunVec4Tests<fworld>();

  RunVec4Tests<flocal>();
}

void RunVectorTests()
{
  RunVec2Tests();
  RunVec3Tests();
}
