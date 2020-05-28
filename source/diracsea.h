/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma warning(disable: 6255) // disable alloca overflow warnings, we are aware alloca could cause overflow if mishandled
#pragma warning(disable: 6011) // disable null pointer dereference, we are usually asserting pointer validity prior to access
#pragma warning(disable: 26812) // Vulkan and our code uses unscoped enums in places, this is unnecessary to warn

#include <cstdint>
#include <cstdio>
#include <cstring>
	
enum ERunResult : int32_t
{
	eRR_Success = 0,
	eRR_Error = 1
};
