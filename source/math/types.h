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
template <typename T>
static constexpr T kFMax = std::numeric_limits<T>::max();

template <typename T>
static constexpr T kFMin = std::numeric_limits<T>::lowest();

template <typename T>
static constexpr T kFEpsilon = std::numeric_limits<T>::epsilon();


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
enum class EEmpty : uint8_t { Constructor };


///////////////////////////////////////////////////////////////////////
// Constants
template <typename T>
static constexpr T kPi = T(3.14159265358979323846);

template <typename T>
static constexpr T kHalfPi = T(kPi<fworld> / fworld(2));

template <typename T>
static constexpr T kQuarterPi = T(kPi<fworld> / fworld(4));

template <typename T>
static constexpr T kTwoPi = T(kPi<fworld> * fworld(2));

static constexpr fworld kFWorldPi = kPi<fworld>;
static constexpr fworld kFWorldHalfPi = kHalfPi<fworld>;
static constexpr fworld kFWorldQuarterPi = kQuarterPi<fworld>;
static constexpr fworld kFWorldTwoPi = kTwoPi<fworld>;

static constexpr flocal kFLocalPi = kPi<flocal>;
static constexpr flocal kFLocalHalfPi = flocal(kHalfPi<fworld>);
static constexpr flocal kFLocalQuarterPi = flocal(kQuarterPi<fworld>);
static constexpr flocal kFLocalTwoPi = flocal(kTwoPi<fworld>);

///////////////////////////////////////////////////////////////////////
// Conversions
template <typename T>
static constexpr T DegreesToRadians(T d) { return d * kPi<T> / T(180); }

template <typename T>
static constexpr T RadiansToDegrees(T r) { return r * 180 / kPi<T>; }
