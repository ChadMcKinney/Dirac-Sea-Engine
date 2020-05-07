/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cstdio>

#include "vector.h"

int main()
{
    puts("Hello, world");
    Vec3w a(1, 2, 3);
    Vec3w b(4, 5, 6);
    Vec3w c = a + b;
    printf("c: { x: %f, y: %f, z: %f\n", c.x, c.y, c.z);
    return 0;
}
