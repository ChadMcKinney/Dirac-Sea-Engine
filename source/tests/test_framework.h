/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <cstdio>

#define TEST(name, ...) do { puts("Running test: " #name); assert(__VA_ARGS__); puts("Test successful"); } while (false)
