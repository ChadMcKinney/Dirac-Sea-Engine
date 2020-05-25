/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cstdio>

#define TEST(name, ...) do { assert(__VA_ARGS__ && "Failed for test: " name); } while (false)
