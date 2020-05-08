/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <limits>
#include <stdint.h>

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


///////////////////////////////////////////////////////////////////////
// Constants
template <typename T>
static constexpr T kPi = T(3.14159265358979323846);

template <typename T>
static constexpr T kHalfPi = kPi<T> / T(2);

template <typename T>
static constexpr T kQuarterPi = kPi<T> / T(4);

static constexpr fworld kFWorldPi = kPi<fworld>;
static constexpr fworld kFWorldHalfPi = kHalfPi<fworld>;
static constexpr fworld kFWorldQuarterPi = kQuarterPi<fworld>;

static constexpr flocal kFLocalPi = kPi<flocal>;
static constexpr flocal kFLocalHalfPi = flocal(kHalfPi<fworld>);
static constexpr flocal kFLocalQuarterPi = flocal(kQuarterPi<fworld>);
