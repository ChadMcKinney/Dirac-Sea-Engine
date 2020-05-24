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
}

void RunQuaternionTests()
{
	RunQuaternionTest_tpl<flocal>();
	RunQuaternionTest_tpl<fworld>();
}
