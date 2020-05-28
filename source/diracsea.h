/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
	
enum ERunResult : int32_t
{
	eRR_Success = 0,
	eRR_Error = 1
};

#define AUTO_ENUM_MEMBER(member) member,
#define AUTO_ENUM_MEMBER_NAME(member) #member,

#define SCOPED_AUTO_ENUM(name, list, type)\
namespace E##name\
{\
	enum class Enum : type\
	{\
		list(AUTO_ENUM_MEMBER)\
		ENUM_COUNT,\
		ENUM_UNDEFINED\
	};\
	const char* names[] =\
	{\
		list(AUTO_ENUM_MEMBER_NAME)\
		"ENUM_COUNT",\
		"ENUM_UNDEFINED"\
	};\
	static inline constexpr const char* ToString(Enum e)\
	{\
		if ((size_t) e < (size_t) Enum::ENUM_COUNT)\
			return names[(size_t)e];\
		else\
			return "ENUM_UNDEFINED";\
	}\
	static constexpr size_t count = (size_t) Enum::ENUM_COUNT;\
}

#define AUTO_ENUM(name, list, type)\
namespace E##name\
{\
	enum Enum : type\
	{\
		list(AUTO_ENUM_MEMBER)\
		ENUM_COUNT,\
		ENUM_UNDEFINED\
	};\
	const char* names[] =\
	{\
		list(AUTO_ENUM_MEMBER_NAME)\
		"ENUM_COUNT",\
		"ENUM_UNDEFINED"\
	};\
	static inline constexpr const char* ToString(Enum e)\
	{\
		if ((size_t) e < (size_t) Enum::ENUM_COUNT)\
			return names[(size_t)e];\
		else\
			return "ENUM_UNDEFINED";\
	}\
	static constexpr size_t count = (size_t) Enum::ENUM_COUNT;\
}
