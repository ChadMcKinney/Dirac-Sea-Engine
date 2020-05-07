/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <stdint.h>
#include <limits>

///////////////////////////////////////////////////////////////////////
// FWorld
typedef double fworld;
static constexpr fworld kFWorldMax = std::numeric_limits<fworld>::max();
static constexpr fworld kFWorldMin = std::numeric_limits<fworld>::lowest();
static constexpr fworld kFWorldEpsilon = std::numeric_limits<fworld>::epsilon();

///////////////////////////////////////////////////////////////////////
// FLocal
typedef float flocal;
static constexpr flocal kFLocalMax = std::numeric_limits<flocal>::max();
static constexpr flocal kFLocalMin = std::numeric_limits<flocal>::lowest();
static constexpr flocal kFLocalEpsilon = std::numeric_limits<flocal>::epsilon();

///////////////////////////////////////////////////////////////////////
// epsilon
template <typename T>
constexpr T epsilon()
{
	return std::numeric_limits<T>::epsilon();
}

///////////////////////////////////////////////////////////////////////
// Construction types
enum class EZero : uint8_t { Constructor };
enum class EIdentity : uint8_t { Constructor };
enum class EUninitialized : uint8_t { Constructor };
