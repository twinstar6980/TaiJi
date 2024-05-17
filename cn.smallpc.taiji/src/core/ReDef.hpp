#include "./Platform.hpp"

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace {

	inline int frd_strcmp(char const * const & s1, char const * const & s2) {
		return std::strcmp(s1, s2);
	}
	inline int frd_stricmp(char const * const & s1, char const * const & s2) {
	#if defined PLATFORM_WINDOWS
		return ::stricmp(s1, s2);
	#elif defined PLATFORM_ANDROID
		return ::strcasecmp(s1, s2);
	#endif
	}

	inline auto frd_vsprintf(char * _Buf, std::size_t _BufCnt, char const * _Fmt, std::va_list _ArgList) {
		return std::vsnprintf(_Buf, _BufCnt, _Fmt, _ArgList);
	}
	inline auto frd_vsscanf(char const * _Buf, char const * _Fmt, std::va_list _ArgList) {
		return std::vsscanf(_Buf, _Fmt, _ArgList);
	}

}
