/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

/////////////////////////////////////////////////////////////////////
// Frame

typedef std::chrono::steady_clock TSteadyClock;
typedef std::chrono::steady_clock::time_point TTime;
typedef std::chrono::duration<double, std::ratio<60, 1>> TMinutes;
typedef std::chrono::duration<double, std::ratio<1, 1>> TSeconds;
typedef std::chrono::duration<double, std::ratio<1, 1000>> TMilliseconds;
typedef std::chrono::duration<uint64_t, std::ratio<1, 1000000>> TMicroseconds;

typedef uint64_t TFrameId;

struct SFrameContext
{
	TTime frameStartTime;
	TMilliseconds lastFrameDuration;
	TMinutes gameDuration;
	TFrameId frameId;
};
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Logging

#define VERBOSITY 1

#if VERBOSITY > 0
#define DiracLog(logVerbosity, ...) do { if constexpr (logVerbosity <= VERBOSITY) { printf(__VA_ARGS__); puts(""); } } while (0)
#define DiracLogSameLine(logVerbosity, ...) do { if constexpr (logVerbosity <= VERBOSITY) { printf(__VA_ARGS__); } } while (0)
#else
#define DiracLog(...)
#endif

#define DiracError(...) do { fprintf(stderr, __VA_ARGS__); puts(""); } while (0)
/////////////////////////////////////////////////////////////////////
	
enum ERunResult : int32_t
{
	eRR_Success = 0,
	eRR_Error = 1
};

#define BIT(x) (1 << x)

static const union
{
	uint32_t i;
	char c[4];
} _endianInt = { 0x01020304 };
static const bool kIsBigEndian = _endianInt.c[0] == 1;

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
#define AUTO_BITFIELD_MEMBER(member) member = BIT(static_cast<std::underlying_type<Enum>::type>(Enum::member)),
#define AUTO_BITFIELD_OPERATOR(T, OP) inline constexpr T operator OP(T lhs, T rhs)\
{ return (T) (static_cast<std::underlying_type<T>::type>(lhs) OP static_cast<std::underlying_type<T>::type>(rhs)); }
#define AUTO_BITFIELD_ENUM(name, list, flagType)\
SCOPED_AUTO_ENUM(name, list, flagType)\
namespace E##name\
{\
	enum class Flags : flagType\
	{\
		list(AUTO_BITFIELD_MEMBER)\
		FLAGS_NONE = 0,\
		FLAGS_ALL = flagType(BIT(E##name::count)) - 1\
	};\
	typedef flagType TUnderlyingType;\
}\
inline E##name::Flags operator~(E##name::Flags n) { return static_cast<E##name::Flags>(~static_cast<std::underlying_type<E##name::Flags>::type>(n)); }\
AUTO_BITFIELD_OPERATOR(E##name::Flags, |)\
AUTO_BITFIELD_OPERATOR(E##name::Flags, ^)\
AUTO_BITFIELD_OPERATOR(E##name::Flags, &)

