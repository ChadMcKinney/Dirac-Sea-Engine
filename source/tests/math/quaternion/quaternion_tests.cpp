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

	{
		Quaternion<T> q(EIdentity::Constructor);
		Quaternion<T> q2 = q * q;
		TEST("quaternion: IDENTITY * IDENTITY == IDENTITY", q2 == q);
	}

	{
		Quaternion<T> q(EIdentity::Constructor);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * v;
		TEST("quaternion:: IDENTITY * v == v", v == v2);
	}

	{
		Quaternion<T> q(EIdentity::Constructor);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = v * q;
		TEST("quaternion:: v * IDENTITY == v", v == v2);
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateAxisAngle(Vec3<T>::Forward, kPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * v;
		TEST("quaternion:: AA * v", v2.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateAxisAngle(Vec3<T>::Forward, kPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * q * v;
		TEST("quaternion:: AA * AA * v", v2.IsEquivalent(Vec3<T>::Up, epsilon<T>() * 2));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateAxisAngle(Vec3<T>::Forward, kHalfPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q.Inverted() * q * v;
		TEST("quaternion:: AA.Inverted() * AA * v", v2.IsEquivalent(Vec3<T>::Up, epsilon<T>() * 4));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateAxisAngle(Vec3<T>::Forward, kHalfPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * v;
		Vec3<T> v3 = v * q.Inverted();
		TEST("quaternion:: AA * v == v * AA.Inverted()", v2.IsEquivalent(v3, epsilon<T>() * 4));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateRotationX(kPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * v;
		TEST("quaternion:: RotationX * v", v2.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateRotationY(kPi<T>);
		Vec3<T> v = Vec3<T>::Forward;
		Vec3<T> v2 = q * v;
		TEST("quaternion:: RotationY * v", v2.IsEquivalent(Vec3<T>::Back, epsilon<T>()));
	}

	{
		Quaternion<T> q = Quaternion<T>::CreateRotationZ(kPi<T>);
		Vec3<T> v = Vec3<T>::Up;
		Vec3<T> v2 = q * v;
		TEST("quaternion:: RotationZ * v", v2.IsEquivalent(Vec3<T>::Down, epsilon<T>()));
	}

	{
		Vec3<T> axis(T(0.31415), T(0.9), T(-0.666));
		axis.Normalize();
		for (uint8_t i = 0; i < 255; ++i)
		{
			T angle = -kPi<T> + kTwoPi<T> * (i / T(255));
			Matrix33<T> m = Matrix33<T>::CreateRotationAA(angle, axis);
			Quaternion<T> q(m);
			Vec3<T> v = Vec3<T>::Forward * m;
			Vec3<T> v2 = q * Vec3<T>::Forward;
			TEST("quaternion: q(m) * v == m * v", v.IsEquivalent(v2, epsilon<T>() * 4));
		}
	}

	{
		Vec3<T> axis(T(0.31415), T(0.9), T(-0.666));
		axis.Normalize();
		for (uint8_t i = 0; i < 255; ++i)
		{
			T angle = -kPi<T> + kTwoPi<T> * (i / T(255));
			Matrix33<T> m = Matrix33<T>::CreateRotationAA(angle, axis);
			Matrix33<T> m2 = Matrix33<T>::CreateRotationAA(angle * T(0.5), axis);
			Quaternion<T> q(m);
			Quaternion<T> q2(m);
			Vec3<T> v = q * Vec3<T>::Forward;
			Vec3<T> v2 = ((q / q2) * q2) * Vec3<T>::Forward;
			TEST("quaternion: q(m) * v == m * v", v.IsEquivalent(v2, epsilon<T>() * 8));
		}
	}
}

void RunQuaternionTests()
{
	RunQuaternionTest_tpl<flocal>();
	RunQuaternionTest_tpl<fworld>();
}
