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
static constexpr fworld kFWorldMin = std::numeric_limits<fworld>::min();
static constexpr fworld kFWorldEpsilon = DBL_EPSILON;

///////////////////////////////////////////////////////////////////////
// FLocal
typedef float flocal;
static constexpr flocal kFLocalMax = std::numeric_limits<flocal>::max();
static constexpr flocal kFLocalMin = std::numeric_limits<flocal>::min();
static constexpr flocal kFLocalEpsilon = FLT_EPSILON;
