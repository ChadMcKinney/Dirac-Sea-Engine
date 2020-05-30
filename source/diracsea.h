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

#define BIT(x) (1 << x)

/////////////////////////////////////////////////////////////////////
// Enum macros for reflection and range handling

// Helpers
#define AUTO_ENUM_MEMBER(member) member,
#define AUTO_ENUM_MEMBER_NAME(member) #member,
#define AUTO_ENUM_HELPERS(name, list, type)\
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

//////////////////////
// Auto Scoped Enum
#define SCOPED_AUTO_ENUM(name, list, type)\
namespace E##name\
{\
	enum class Enum : type\
	{\
		list(AUTO_ENUM_MEMBER)\
		ENUM_COUNT,\
		ENUM_UNDEFINED\
	};\
	AUTO_ENUM_HELPERS(name, list, type)\
}

//////////////////////
// Auto Enum
#define AUTO_ENUM(name, list, type)\
namespace E##name\
{\
	enum Enum : type\
	{\
		list(AUTO_ENUM_MEMBER)\
		ENUM_COUNT,\
		ENUM_UNDEFINED\
	};\
	AUTO_ENUM_HELPERS(name, list, type)\
}

//////////////////////
// Auto Bitfield
#define AUTO_BITFIELD_MEMBER(name, member) BIT(E##name::member),
#define AUTO_BITFIELD_OPERATOR(T, OP) inline T operator OP(T lhs, T rhs) { return T((std::underlying_type<T>)(lhs) op (std::underlying_type<T>)(rhs)); }
#define AUTO_BITFIELD_ENUM(name, list, type)\
SCOPED_AUTO_ENUM(name, list, type)\
namespace E##name\
{\
	enum class Flags : type\
	{\
		list(AUTO_BITFIELD_MEMBER)\
		FLAGS_NONE = 0,\
		FLAGS_ALL = type(BIT(E##name::ENUM_COUNT)) - 1\
	};\
}\
inline E##name::Flags operator~(E##name::Flags n) { return ~(std::underlying_type<type>(n)); }\
AUTO_BITFIELD_OPERATOR(E##name::Flags, |)\
AUTO_BITFIELD_OPERATOR(E##name::Flags, ^)\
AUTO_BITFIELD_OPERATOR(E##name::Flags, &)

