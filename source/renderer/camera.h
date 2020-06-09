/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include "matrix44.h"

struct SCamera
{
    Matrix44l transform = { EIdentity::Constructor };
};
