#ifndef PUZZLE_UTIL
#define PUZZLE_UTIL

#include <cassert>

#ifndef NDEBUG
#define PUZZLE_UNREACHABLE assert(false)
#elif defined(__GNUC__) || defined(__clang__)
#define PUZZLE_UNREACHABLE __builtin_unreachable()
#else
#error "Need definition for unreachable macro"
#endif

template<typename T, typename U>
T* cast(U* u)
{
	assert(T::classof(u));
	return static_cast<T*>(u);
}

template<typename T, typename U>
const T* cast(const U* u)
{
	assert(T::classof(u));
	return static_cast<const T*>(u);
}

#endif
