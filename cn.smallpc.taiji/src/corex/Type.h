# pragma once

# include <cstdint>
# include <type_traits>

# include "./Macro.h"

# define kNullPtr nullptr

#if defined PLATFORM_ANDROID
# include <stdlib.h>
#endif

namespace $TJ {

	using Void = void;

	using Bool = bool;

	using Byte = unsigned char;
	using Ch = char;

	using UCh = unsigned char;
	using SCh = signed char;

	using Ch8 = char;
	using Ch16 = char16_t;
	using Ch32 = char32_t;
	using WCh = wchar_t;

	using UI8 = uint8_t;
	using SI8 = int8_t;

	using UI16 = uint16_t;
	using SI16 = int16_t;

	using UI32 = uint32_t;
	using SI32 = int32_t;

	using UI64 = uint64_t;
	using SI64 = int64_t;

	using F32 = float;
	using F64 = double;

	using SzI = UI32;
	using SzSI = SI32;

	using FourCC = UI32;

	inline constexpr auto makeFourCC(Ch const & first, Ch const & second, Ch const & third, Ch const & fourth) {
		return static_cast<FourCC>((first << 0) | (second << 8) | (third << 16) | (fourth << 24));
	}

	tmpl_st(T) inline auto swpVal(T & v1, T & v2) {
		auto tmp = v1;
		v1 = v2;
		v2 = tmp;
		return;
	}
	tmpl_st(T) inline auto movVal(T & dst, T & src, T const & dflt) {
		dst = src;
		src = dflt;
		return;
	}
	tmpl_st(T) inline auto swpPtr(T *& p1, T *& p2) {
		auto tmp = p1;
		p1 = p2;
		p2 = tmp;
		tmp = kNullPtr;
		return;
	}
	tmpl_st(T) inline auto movPtr(T *& dst, T *& src) {
		dst = src;
		src = kNullPtr;
		return;
	}
	tmpl_st(T) auto calcNumDiff(T const & n1, T const & n2) {
		return static_cast<T>(ternary_oper(n1 > n2, n1 - n2, n2 - n1));
	}

	tmpl_dt(T1, T2) constexpr Bool cmpT() { return std::is_same<T1, T2>::value; }

	tmpl_st(T) constexpr auto isTBool() { return cmpT<Bool, T>(); }
	tmpl_st(T) constexpr auto isTUInt() { return cmpT<UI8, T>() || cmpT<UI16, T>() || cmpT<UI32, T>() || cmpT<UI64, T>(); }
	tmpl_st(T) constexpr auto isTSInt() { return cmpT<SI8, T>() || cmpT<SI16, T>() || cmpT<SI32, T>() || cmpT<SI64, T>(); }
	tmpl_st(T) constexpr auto isTInt() { return isTUInt<T>() || isTSInt<T>(); }
	tmpl_st(T) constexpr auto isTFolat() { return cmpT<F32, T>() || cmpT<F64, T>(); }

	tmpl_st(T) T cnvEndian(T const & src) {
		goifcexp(sizeof(T) == 1)
			return src;
		elifcexp(sizeof(T) == 2)
			return reinterpret_cast<Byte const *>(&src)[0] << 8 |
			reinterpret_cast<Byte const *>(&src)[1] << 0;
		elifcexp(sizeof(T) == 4)
			return reinterpret_cast<Byte const *>(&src)[0] << 24 |
			reinterpret_cast<Byte const *>(&src)[1] << 16 |
			reinterpret_cast<Byte const *>(&src)[2] << 8 |
			reinterpret_cast<Byte const *>(&src)[3] << 0;
		elifcexp(sizeof(T) == 8)
			return reinterpret_cast<Byte const *>(&src)[0] << 56 |
			reinterpret_cast<Byte const *>(&src)[1] << 48 |
			reinterpret_cast<Byte const *>(&src)[2] << 40 |
			reinterpret_cast<Byte const *>(&src)[3] << 32 |
			reinterpret_cast<Byte const *>(&src)[4] << 24 |
			reinterpret_cast<Byte const *>(&src)[5] << 16 |
			reinterpret_cast<Byte const *>(&src)[6] << 8 |
			reinterpret_cast<Byte const *>(&src)[7] << 0;
	}

}
