/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#include "matrix_tests.h"

#include <cassert>

#include "math/matrix22.h"
#include "math/matrix33.h"
#include "math/matrix43.h"
#include "math/matrix44.h"
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
		TEST("m22: m * m2 == m3", (m * m2) == m3 && m != m3);
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
		Vec3<T> axis(T(0.267), T(-0.535), T(0.802));
		axis.Normalize();
		Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(radians, axis);
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
		TEST("m33: (1,1,1).Project((1,1,1).Normalized())", v2.IsEquivalent(v3, epsilon<T>() * 10));
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
		TEST("m33: orthonormalize", m2 == m3);
	}

	{
		Matrix33<T> m(T(0.3), 0, 0, 0, T(0.8), 0, 0, 0, T(0.76));
		Matrix33<T> m2 = m.Orthonormalized();
		Matrix33<T> m3(1, 0, 0, 0, 1, 0, 0, 0, 1);
		TEST("m33: orthonormalize", m2 == m3);
	}

	{
		Matrix33<T> m(T(0.3), 0, 0, 0, T(0.8), 0, 0, 0, T(0.76));
		Matrix33<T> m2 = m.Orthonormalized_Safe();
		Matrix33<T> m3(1, 0, 0, 0, 1, 0, 0, 0, 1);
		TEST("m33: orthonormalize safe", m2 == m3);
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrthonormalBasisRotation(Vec3<T>::Forward, Vec3<T>::Up);
		Vec3<T> v(666, 1337, kPi<T>);
		Vec3<T> v2 = v * m;
		TEST("m33: ((right) (up) (forward)) * v == v", v == v2);
	}

	{
		Vec3<T> up(0, T(0.9), T(0.1));
		up.Normalize();
		Matrix33<T> m = Matrix33<T>::CreateOrthonormalBasisRotation_FixedForward(Vec3<T>::Forward, up);
		Vec3<T> v(666, 1337, kPi<T>);
		Vec3<T> v2 = v * m;
		TEST("m33: ((right) (up) (forward)) * v == v", v == v2);
	}

	{
		Vec3<T> forward(0, T(0.1), T(0.9));
		forward.Normalize();
		Matrix33<T> m = Matrix33<T>::CreateOrthonormalBasisRotation_FixedUp(forward, Vec3<T>::Up);
		Vec3<T> v(666, 1337, kPi<T>);
		Vec3<T> v2 = v * m;
		TEST("m33: ((right) (up) (forward)) * v == v", v == v2);
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateRotationZ(kHalfPi<T>);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v2 = Vec3<T>::Up * m;
		Vec3<T> v3 = Vec3<T>::Forward * m;
		TEST("m33: CreateRotation Z Right", v.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
		TEST("m33: CreateRotation Z Up", v2.IsEquivalent(Vec3<T>::Left, epsilon<T>()));
		TEST("m33: CreateRotation Z Forward", v3.IsEquivalent(Vec3<T>::Forward, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrthonormalBasisRotation(Vec3<T>::Up, Vec3<T>::Back);
		Vec3<T> v = Vec3<T>::Up * m;
		Vec3<T> v2 = Vec3<T>::Forward * m;
		Vec3<T> v3 = Vec3<T>::Right * m;
		TEST("m33: CreateOrthonormalBasisRotation Up Back", v.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
		TEST("m33: CreateOrthonormalBasisRotation Up Back", v2.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
		TEST("m33: CreateOrthonormalBasisRotation Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrthonormalBasisRotation_FixedForward(Vec3<T>::Up, Vec3<T>::Back);
		Vec3<T> v = Vec3<T>::Up * m;
		Vec3<T> v2 = Vec3<T>::Forward * m;
		Vec3<T> v3 = Vec3<T>::Right * m;
		TEST("m33: CreateOrthonormalBasisRotation_FixedForward Up Back", v.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
		TEST("m33: CreateOrthonormalBasisRotation_FixedForward Up Back", v2.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
		TEST("m33: CreateOrthonormalBasisRotation_FixedForward Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Up, Vec3<T>::Back, 0);
		Vec3<T> v = Vec3<T>::Up * m;
		Vec3<T> v2 = Vec3<T>::Forward * m;
		Vec3<T> v3 = Vec3<T>::Right * m;
		TEST("m33: CreateOrientation Up Back", v.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
		TEST("m33: CreateOrientation Up Back", v2.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
		TEST("m33: CreateOrientation Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Up, Vec3<T>::Back, 0);
		Vec3<T> v = Vec3<T>::Up * m * m;
		Vec3<T> v2 = Vec3<T>::Forward * m * m;
		Vec3<T> v3 = Vec3<T>::Right * m * m;
		TEST("m33: CreateOrientation Up Back", v.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
		TEST("m33: CreateOrientation Up Back", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
		TEST("m33: CreateOrientation Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kPi<T> * 2);
		Vec3<T> v = Vec3<T>::Up * m;
		Vec3<T> v2 = Vec3<T>::Forward;
		Vec3<T> v3 = Vec3<T>::Right * m;
		TEST("m33: CreateOrientation Up Back", v.IsEquivalent(Vec3<T>::Up, epsilon<T>() * 4));
		TEST("m33: CreateOrientation Up Back", v2.IsEquivalent(Vec3<T>::Forward, epsilon<T>() * 4));
		TEST("m33: CreateOrientation Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Up, Vec3<T>::Back, kPi<T> * 2);
		Vec3<T> v = Vec3<T>::Up * m * m;
		Vec3<T> v2 = Vec3<T>::Forward * m * m;
		Vec3<T> v3 = Vec3<T>::Right * m * m;
		TEST("m33: CreateOrientation Up Back", v.IsEquivalent(Vec3<T>::Down, epsilon<T>() * 4));
		TEST("m33: CreateOrientation Up Back", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>() * 4));
		TEST("m33: CreateOrientation Up Back", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kPi<T>);
		Matrix33<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, 0);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v2 = Vec3<T>::Right * m2;
		Vec3<T> v3 = Vec3<T>::Up * m;
		Vec3<T> v4 = Vec3<T>::Up * m2;
		TEST("m33: CreateOrientation Forward Up", v.IsEquivalent(Vec3<T>::Left, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v2.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v3.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v4.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kHalfPi<T>);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v2 = Vec3<T>::Up * m;
		TEST("m33: CreateOrientation Forward Up", v.IsEquivalent(Vec3<T>::Up, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v2.IsEquivalent(Vec3<T>::Left, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kHalfPi<T>);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v2 = Vec3<T>::Up * m;
		Vec3<T> v3 = v * m;
		Vec3<T> v4 = v2 * m;
		TEST("m33: CreateOrientation Forward Up", v3.IsEquivalent(Vec3<T>::Left, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v4.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kHalfPi<T>);
		Matrix33<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kPi<T>);
		Matrix33<T> m3 = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, kPi<T> * 2);
		Matrix33<T> m4 = Matrix33<T>::CreateOrientation(Vec3<T>::Forward, Vec3<T>::Up, 0);
		Vec3<T> v1 = Vec3<T>::Right * m * m * m * m;
		Vec3<T> v2 = Vec3<T>::Right * m2 * m2;
		Vec3<T> v3 = Vec3<T>::Right * m3;
		Vec3<T> v4 = Vec3<T>::Right * m4;
		TEST("m33: CreateOrientation rotating identities", v1.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
		TEST("m33: CreateOrientation rotating identities", v2.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
		TEST("m33: CreateOrientation rotating identities", v3.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
		TEST("m33: CreateOrientation rotating identities", v4.IsEquivalent(Vec3<T>::Right, epsilon<T>() * 4));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Up, Vec3<T>::Right, kHalfPi<T>);
		Matrix33<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Up, Vec3<T>::Right, 0);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v2 = Vec3<T>::Up * m;
		Vec3<T> v3 = Vec3<T>::Right * m2;
		Vec3<T> v4 = Vec3<T>::Up * m2;
		TEST("m33: CreateOrientation Forward Up", v.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v3.IsEquivalent(Vec3<T>::Forward, epsilon<T>()));
		TEST("m33: CreateOrientation Forward Up", v4.IsEquivalent(Vec3<T>::Right, epsilon<T>()));
	}

	{
		Matrix33<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix33<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Vec3<T> v = Vec3<T>::Right * m;
		Vec3<T> v1 = m * Vec3<T>::Right;
		Vec3<T> v2 = Vec3<T>::Right * m2;
		TEST("m33: CreateOrientation Right Up", v.IsEquivalent(Vec3<T>::Forward, epsilon<T>()));
		TEST("m33: CreateOrientation Right Up", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
	}

	{
		Vec3<T> v = Vec3<T>::Right;
		Vec3<T> v2 = Vec3<T>::Forward;
		T vDot = v.Dot(v2);
		T angle = std::acos(vDot);
		Vec3<T> c = v.Cross(v2);
		Matrix33<T> m = Matrix33<T>::CreateRotationAA(angle, c);
		Vec3<T> v3 = v * m;
		TEST("m33: v.Cross(v2)", v3.IsEquivalent(v2, epsilon<T>()));
	}

	{
		Vec3<T> v(1, 666, -1337);
		v.Normalize();
		Vec3<T> v2 = Vec3<T>::Forward;
		T vDot = v.Dot(v2);
		T angle = std::acos(vDot);
		Vec3<T> c = v.Cross(v2);
		c.Normalize();
		Matrix33<T> m = Matrix33<T>::CreateRotationAA(angle, c);
		Vec3<T> v3 = v * m;
		TEST("m33: v.Cross(v2)", v3.IsEquivalent(v2, epsilon<T>() * 10));
	}

	{
		Vec3<T> v(1, 666, -1337);
		v.Normalize();
		Vec3<T> v2 = Vec3<T>::Forward;
		T vDot = v.Dot(v2);
		T angle = std::acos(vDot);
		Vec3<T> c = v.Cross(v2);
		c.Normalize();
		Matrix33<T> m = Matrix33<T>::CreateRotationAA(angle, c);
		Vec3<T> v3 = v * m;
		Matrix33<T> m2 = m.Inverted_Rotation();
		Matrix33<T> m3 = m.Transposed();
		Vec3<T> v4 = v3 * m.Inverted_Rotation();
		TEST("m33: Inverted_Rotation == Transpose", m2 == m3 && v.IsEquivalent(v4, epsilon<T>()));
	}
}

template<typename T>
void RunMatrix4x3Tests()
{
	{
		TEST("m43: 0 == 0", Matrix43<T>(EZero::Constructor) == Matrix43<T>(EZero::Constructor));
	}

	{
		TEST("m43: 0 isEquivalent 0", Matrix43<T>(EZero::Constructor).IsEquivalent(Matrix43<T>(EZero::Constructor), epsilon<T>()));
	}

	{
		Matrix43<T> m(EIdentity::Constructor);
		TEST("m43: Identity * Identity == Identity", (m * m) == m);
	}

	{
		Matrix43<T> m(EIdentity::Constructor);
		Matrix43<T> m2(EZero::Constructor);
		TEST("m43: Identity * Zero == Zero", (m * m2) == m2);
	}

	{
		Matrix43<T> m(EIdentity::Constructor);
		Matrix44<T> m2 = m.Transposed();
		Matrix44<T> m3(EIdentity::Constructor);
		TEST("m43: Identity.Transposed() == m44.Identity", m2 == m3);
	}

	{
		Matrix43<T> m(EIdentity::Constructor);
		Matrix43<T> m2 = m.Inverted();
		TEST("m43: Identity.Invert() == Identity", m == m2);
	}

	{
		Matrix43<T> m(EIdentity::Constructor);
		Vec4<T> v(2, 4, 8, 1);
		Vec4<T> v2 = v * m;
		Vec3<T> v3(1, 2, 3);
		Vec3<T> v4 = v3 * m;
		TEST("m44: identity * v == v", v == v2 && v3 == v4);
	}

	{
		Matrix43<T> m = Matrix43<T>::CreateTranslation(Vec3<T>(10, 20, 30));
		Vec3<T> v(2, 4, 8);
		Vec3<T> v2 = v * m;
		Vec3<T> v3(12, 24, 38);
		Vec3<T> v4(1, 2, 3);
		Vec3<T> v5 = v3 * m;
		Vec3<T> v6(11, 22, 33);
		TEST("m44: translation * v", v2 == v3 && v5 == v5);
	}


	{
		Matrix43<T> m = Matrix43<T>::CreateTranslation(Vec3<T>(10, 20, 30));
		Vec3<T> v(2, 4, 8);
		Vec3<T> v2 = v * m;
		Vec3<T> v3 = v2 * m.Inverted();
		Vec3<T> v4(1, 2, 3);
		Vec3<T> v5 = v4 * m;
		Vec3<T> v6 = v5 * m.Inverted();
		TEST("m44: translation * v * translation.inverted() == v", v.IsEquivalent(v3, epsilon<T>()));
		TEST("m44: translation * v * translation.inverted() == v", v4.IsEquivalent(v6, epsilon<T>()));
	}

	{
		Matrix43<T> m = Matrix33<T>::CreateRotationAA(kHalfPi<T>, Vec3<T>(1, 0, 0));
		Vec3<T> v(1, 2, 3);
		Vec3<T> v2 = v * m;
		Vec3<T> v3 = v2 * m.Inverted();
		TEST("m43: (v * m) * m.Inverted() == v", v.IsEquivalent(v3, epsilon<T>() * 4));
	}

	{
		Matrix43<T> m = Matrix33<T>::CreateRotationAA(kHalfPi<T>, Vec3<T>(1, 0, 0));
		m = m * Matrix43<T>::CreateTranslation(Vec3<T>(10, 20, 30));
		Vec3<T> v(1, 2, 4);
		Vec3<T> v2 = v * m;
		Vec3<T> v3 = v2 * m.Inverted();
		TEST("m43: (v * m) * m.Inverted() == v", v.IsEquivalent(v3, epsilon<T>() * 100));
	}

	{
		Matrix43<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix43<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Matrix43<T> rightM = Matrix43<T>::CreateTranslation(Vec3<T>::Right);
		Matrix43<T> upM = Matrix43<T>::CreateTranslation(Vec3<T>::Up);
		Vec3<T> right = Vec3<T>(EZero::Constructor) * rightM;
		Vec3<T> v =  Vec3<T>(EZero::Constructor) * (rightM * m);
		Vec3<T> v2 = right * m2;
		Vec3<T> v3 = v * m.Inverted();
		Vec3<T> v4 = v2 * m2.Inverted();
		TEST("m43: CreateOrientation Right Up with translation", v3.IsEquivalent(right, epsilon<T>()));
		TEST("m43: CreateOrientation Right Up with translation", v4.IsEquivalent(right, epsilon<T>()));
	}

	{
		Matrix43<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix43<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Matrix43<T> rightM = Matrix43<T>::CreateTranslation(Vec3<T>::Right);
		Matrix43<T> upM = Matrix43<T>::CreateTranslation(Vec3<T>::Up);
		Vec3<T> right = Vec3<T>(EZero::Constructor) * rightM;
		Vec3<T> v =  Vec3<T>(EZero::Constructor) * (rightM * m);
		Vec3<T> v2 = right * m2;
		Vec3<T> v3 = v * m.Inverted_Safe();
		Vec3<T> v4 = v2 * m2.Inverted_Safe();
		TEST("m43: CreateOrientation Right Up with translation", v3.IsEquivalent(right, epsilon<T>()));
		TEST("m43: CreateOrientation Right Up with translation", v4.IsEquivalent(right, epsilon<T>()));
	}

	{
		Matrix43<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix43<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Matrix43<T> rightM = Matrix43<T>::CreateTranslation(Vec3<T>::Right);
		Vec3<T> right = Vec3<T>(EZero::Constructor) * rightM;
		Vec3<T> v = Vec3<T>(EZero::Constructor) * (rightM * m);
		Vec3<T> v2 = Vec3<T>(EZero::Constructor) * (rightM * m2);
		TEST("m43: CreateOrientation Right Up", v.IsEquivalent(Vec3<T>::Forward, epsilon<T>()));
		TEST("m43: CreateOrientation Right Up", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
	}

	{
		Matrix43<T> m(
			T(0.9), 0, 0, 
			0, T(0.89), 0,
			0, 0, T(0.92),
			0, 0, 0);
		Matrix43<T> m2 = m.Orthonormalized();
		Vec4<T> v(1, 2, 3, 4);
		Vec4<T> v2 = v * m2;
		TEST("m43: almostIdentity.Orthonormalize() == Identity", v == v2);
	}

	{
		Matrix43<T> m(
			T(0.9), 0, 0, 
			0, T(0.89), 0,
			0, 0, T(0.92),
			10, 20, 30);
		Matrix43<T> m2 = m.Orthonormalized();
		Vec4<T> v(1, 2, 3, 1);
		Vec4<T> v2 = v * m2;
		Vec4<T> v3(11, 22, 33, 1);
		TEST("m43: almostIdentity.Orthonormalize() == Identity", v2 == v3);
	}

	{
		Matrix43<T> m(
			T(0.9), 0, 0, 
			0, T(0.89), 0,
			0, 0, T(0.92),
			10, 20, 30);
		Matrix43<T> m2 = m.Orthonormalized();
		Vec3<T> v(1, 2, 3);
		Vec3<T> v2 = v * m2;
		Vec3<T> v3(11, 22, 33);
		TEST("m43: almostIdentity.Orthonormalize() == Identity", v2 == v3);
	}
}

template<typename T>
void RunMatrix4x4Tests()
{
	{
		TEST("m44: 0 == 0", Matrix44<T>(EZero::Constructor) == Matrix44<T>(EZero::Constructor));
	}

	{
		TEST("m44: 0 isEquivalent 0", Matrix44<T>(EZero::Constructor).IsEquivalent(Matrix44<T>(EZero::Constructor), epsilon<T>()));
	}

	{
		Matrix44<T> m(EIdentity::Constructor);
		Matrix44<T> m0(EZero::Constructor);
		Matrix44<T> mi0 = m0 * m;
		TEST("m44: identity * 0 == 0", mi0 == m0);
	}

	{
		Matrix44<T> m(EIdentity::Constructor);
		Matrix44<T> mii = m * m;
		TEST("m44: identity * identity == identity", mii == m);
	}

	{
		Matrix44<T> m(EIdentity::Constructor);
		Vec4<T> v(1, 0, 0, 1);
		Vec4<T> v2 = v * m;
		TEST("m44: v * identity == v", v == v2);
	}

	{
		Matrix44<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix44<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Vec4<T> right(1, 0, 0, 1);
		Vec4<T> forward(0, 0, 1, 1);
		Vec4<T> back(0, 0, -1, 1);
		Vec4<T> v = right * m;
		Vec4<T> v2 = right * m2;
		TEST("m44: CreateOrientation Right Up", v.IsEquivalent(forward, epsilon<T>()));
		TEST("m44: CreateOrientation Right Up", v2.IsEquivalent(back, epsilon<T>()));
	}

	{
		Matrix44<T> m(EIdentity::Constructor);
		Matrix44<T> m2 = m.Inverted();
		TEST("m44: Identity.inverted() == identity", m == m2);
	}

	{
		Matrix44<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix44<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Vec4<T> right(1, 0, 0, 1);
		Vec4<T> forward(0, 0, 1, 1);
		Vec4<T> back(0, 0, -1, 1);
		Vec4<T> v = right * m;
		Vec4<T> v2 = right * m2;
		Vec4<T> v3 = v * m.Inverted();
		Vec4<T> v4 = v2 * m2.Inverted();
		TEST("m44: CreateOrientation Right Up", v3.IsEquivalent(right, epsilon<T>()));
		TEST("m44: CreateOrientation Right Up", v4.IsEquivalent(right, epsilon<T>()));
	}

	{
		Matrix44<T> m = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, kPi<T>);
		Matrix44<T> m2 = Matrix33<T>::CreateOrientation(Vec3<T>::Right, Vec3<T>::Up, 0);
		Vec4<T> right(1, 0, 0, 1);
		Vec4<T> forward(0, 0, 1, 1);
		Vec4<T> back(0, 0, -1, 1);
		Vec4<T> v = right * m;
		Vec4<T> v2 = right * m2;
		Vec4<T> v3 = v * m.Inverted_Safe();
		Vec4<T> v4 = v2 * m2.Inverted_Safe();
		TEST("m44: CreateOrientation Right Up", v3.IsEquivalent(right, epsilon<T>()));
		TEST("m44: CreateOrientation Right Up", v4.IsEquivalent(right, epsilon<T>()));
	}

	{
		Matrix44<T> m(EIdentity::Constructor);
		Matrix44<T> m2 = m.Orthonormalized();
		TEST("m44: Identity.Orthonormalize() == Identity", m == m2);
	}

	{
		Matrix44<T> m(
			T(0.9), 0, 0, 0,
			0, T(0.89), 0, 0,
			0, 0, T(0.92), 0,
			0, 0, 0, T(0.99123));
		Matrix44<T> m2 = m.Orthonormalized();
		Vec4<T> v(1, 2, 3, 4);
		Vec4<T> v2 = v * m2;
		TEST("m44: Identity.Orthonormalize() == Identity", v == v2);
	}

	{
		Matrix44<T> m = Matrix44<T>::CreatePerspectiveProjection(2);
		Vec4<T> v(2, 4, 8, 1);
		Vec4<T> v2 = v * m;
		Vec4<T> v3(2, 4, 8, 4);
		TEST("m44: Perspective * V", v2 == v3);
	}

	{
		Matrix43<T> m = Matrix43<T>::CreateTranslation(Vec3<T>(10, 20, 30));
		Matrix44<T> m2(m);
		Vec3<T> v(1, 2, 3);
		Vec3<T> v2 = v * m;
		Vec4<T> v3 = v * m2;
		Vec3<T> v4(v3.x, v3.y, v3.z);
		TEST("m44: m44(m43) * v == v * m43", v2 == v4);
	}
}

void RunMatrixTests()
{ 
	RunMatrix2x2Tests<fworld>();
	RunMatrix2x2Tests<flocal>();

	RunMatrix3x3Tests<fworld>();
	RunMatrix3x3Tests<flocal>();

	RunMatrix4x3Tests<fworld>();
	RunMatrix4x3Tests<flocal>();

	RunMatrix4x4Tests<fworld>();
	RunMatrix4x4Tests<flocal>();
}
