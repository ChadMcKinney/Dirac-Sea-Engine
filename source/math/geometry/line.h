/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cassert>

#include "types.h"
#include "vector3.h"

///////////////////////////////////////////////////////////////////////
// Line - represents a line extending infinitely in two directions
///////////////////////////////////////////////////////////////////////
template <typename T>
struct Line
{
	Line();
	Line(EZero);
	Line(EUninitialized);
	Line(const Vec3<T>& _a, const Vec3<T>& _b);

	Vec3<T> a, b; // two arbitrary points on line
};

///////////////////////////////////////////////////////////////////////
template <typename T>
Line<T>::Line()
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Line<T>::Line(EZero)
	: a(EZero::Constructor)
	, b(EZero::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Line<T>::Line(EUninitialized)
	: a(EUninitialized::Constructor)
	, b(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
Line<T>::Line(const Vec3<T>& _a, const Vec3<T>& _b)
	: a(_a)
	, b(_b)
{
}

///////////////////////////////////////////////////////////////////////
typedef Line<fworld> Linew;
typedef Line<flocal> Linel;


///////////////////////////////////////////////////////////////////////
// LineSegment - represents an undirected segment of a line
///////////////////////////////////////////////////////////////////////
template <typename T>
struct LineSegment
{
	LineSegment();
	LineSegment(EZero);
	LineSegment(EUninitialized);
	LineSegment(const Vec3<T>& _a, const Vec3<T>& _b);

	Vec3<T> a, b;
};

///////////////////////////////////////////////////////////////////////
template <typename T>
LineSegment<T>::LineSegment()
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
LineSegment<T>::LineSegment(EZero)
	: a(EZero::Constructor)
	, b(EZero::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
LineSegment<T>::LineSegment(EUninitialized)
	: a(EUninitialized::Constructor)
	, b(EUninitialized::Constructor)
{
}

///////////////////////////////////////////////////////////////////////
template <typename T>
LineSegment<T>::LineSegment(const Vec3<T>& _a, const Vec3<T>& _b)
	: a(_a)
	, b(_b)
{
}

///////////////////////////////////////////////////////////////////////
typedef LineSegment<fworld> LineSegmentw;
typedef LineSegment<flocal> LineSegmentl;
