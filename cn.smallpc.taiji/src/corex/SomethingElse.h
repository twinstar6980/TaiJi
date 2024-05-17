# pragma once

# include "./TypeOfTime.h"
# include "./MyType/Path.h"

namespace $TJ {
	Void Delay(SzI const & sec);
	Void Pause(Ch const * const & msg = kNullPtr);
	Void Exit();
	Void ExitWithPause(Ch const * const & msg = kNullPtr);

	TimeStruct MakeTimeStruct(SzI const & year, SzI const & month, SzI const & day, SzI const & hour, SzI const & min, SzI const & sec);
	TimeInt MakeTimeInt(TimeStruct const & time);
	TimeInt MakeTimeInt(SzI const & year, SzI const & month, SzI const & day, SzI const & hour, SzI const & min, SzI const & sec);
	Str MakeTimeStr(TimeStruct const & time);
	SzSI GetTimeZone();
	TimeStruct GetCompileTimeStruct();
	TimeInt GetCompileTimeInt();
	Str GetCompileTimeStr();

	// easy pow , danger !
	tmpl_st(T) inline auto GetIntPow(T const & base, T const & exp) {
		auto val = static_cast<T>(1);
		for_cnt(idx, static_cast<T>(0), exp)
			val *= base;
		return val;
	}
	tmpl_st(T) inline T GetMaxIOfInt(T const & val, T const & base) {
		SzI idx;
		for (idx = 0u; base * (idx + 1) <= val; ++idx);
		return idx;
	}
	inline auto GetExp2Int(SzI const & val) {
		SzI exp;
		for (exp = 2; val > exp; exp *= 2);
		return exp;
	}
	inline auto isLE() {
		volatile constexpr UI16 val = 0x0001;
		return reinterpret_cast<volatile Byte const *>(&val)[0] == 0x01;
	}
	inline auto isBE() {
		volatile constexpr UI16 val = 0x0001;
		return reinterpret_cast<volatile Byte const *>(&val)[0] == 0x00;
	}

	namespace $ChSet {
	# if defined PLATFORM_WINDOWS
		Ch * UTF16ToUTF8(WCh const * const & src);
		WCh * UTF8ToUTF16(Ch const * const & src);
		Ch * UTF8ToANSI(Ch const * const & src);
		Ch * ANSIToUTF8(Ch const * const & src);

		Ch * UTF8ToANSI(Ch const * const & src);
		Ch * ANSIToUTF8(Ch const * const & src);
	# endif
	}

# if defined PLATFORM_WINDOWS

	Ch * getPathStrByWindows(Ch const * const & srcDir);
	Str & getPathStrByWindows(Str & dst, Path const & srcDir);

	Path & getFilePathByWindow(Path & dst, Path const & srcDir);
	Path & getFilePathByWindow(Path & dst);
	Path & getFilePathWithValidateByWindow(Path & dst, Path const & srcDir);
	Path & getFilePathWithValidateByWindow(Path & dst);

# else

	inline auto & getFilePathWithValidateByWindow(Path & dst, Path const & srcDir) {
		dst.read("<ERROR>:CanNot-GetPath-On-AndroidPlatform");
		return dst;
	}
	inline auto & getFilePathWithValidateByWindow(Path & dst) {
		dst.read("<ERROR>:CanNot-GetPath-On-AndroidPlatform");
		return dst;
	}

# endif
}
