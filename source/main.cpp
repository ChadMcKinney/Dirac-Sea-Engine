/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "platform/platform.h"
#include "tests/tests.h"

int main()
{
    RunTests();
    return platform::RunPlatform();
}
