# pragma once

# include "../Type.h"
# include "../Const.h"

#include <memory.h>

namespace $TJ::$MemUtil {
	inline auto getEnoughSz(SzI const & sz, SzI const & unit = 0x1000u) {
		return static_cast<SzI>(ternary_oper((sz % unit) == kSzZero, sz, ((sz / unit) + kSzOne) * unit));
	}
	tmpl_st(T) inline Byte getByteN(T const * const & src, SzI const & N) {
		return reinterpret_cast<Byte const * const>(src)[N];
	}

	inline auto & memSet(Void * const & dst, SzI const & sz, Byte const & val = kNullByte) {
		memset(dst, val, sz);
		return dst;
	}
	tmpl_st(T) inline auto & memSet(T * const & dst, SzI const & sz, Byte const & val = kNullByte) {
		memset(dst, val, szOf<T>(sz));
		return dst;
	}
	inline auto & memSet(Bool * const & dst, SzI const & sz, Bool const & val = kFalse) {
		memset(dst, static_cast<Byte>(val), szOf<Bool>(sz));
		return dst;
	}

	inline auto & memCpy(Void * const & dst, Void const * const & src, SzI const & sz) {
		memcpy(dst, src, sz);
		return dst;
	}
	tmpl_st(T) inline auto & memCpy(T * const & dst, T const * const & src, SzI const & sz) {
		memcpy(dst, src, szOf<T>(sz));
		return dst;
	}
	tmpl_dt(T1, T2) inline auto & memCpy(T1 * const & dst, T2 const * const & src, SzI const & sz) {
		memcpy(dst, src, sz);
		return dst;
	}

	enum class MemCmpResult : UI8 { kLittleThan, kEqual, kGreatThan };

	constexpr decltype(memcmp(nullptr, nullptr, 0)) kMemCmpIsNonDiff(0);

	inline auto memCmp(Void const * const & p1, Void const * const & p2, SzI const & sz) {
		auto result = memcmp(p1, p2, sz);
		return ternary_oper(result == kMemCmpIsNonDiff, MemCmpResult::kEqual, ternary_oper(result > kMemCmpIsNonDiff, MemCmpResult::kGreatThan, MemCmpResult::kLittleThan));
	}
	tmpl_st(T) inline auto memCmp(T const * const & p1, T const * const & p2, SzI const & sz) {
		auto result = memcmp(p1, p2, szOf<T>(sz));
		return ternary_oper(result == kMemCmpIsNonDiff, MemCmpResult::kEqual, ternary_oper(result > kMemCmpIsNonDiff, MemCmpResult::kGreatThan, MemCmpResult::kLittleThan));
	}
	tmpl_dt(T1, T2) inline auto memCmp(T1 const * const & p1, T2 const * const & p2, SzI const & sz) {
		auto result = memcmp(p1, p2, sz);
		return ternary_oper(result == kMemCmpIsNonDiff, MemCmpResult::kEqual, ternary_oper(result > kMemCmpIsNonDiff, MemCmpResult::kGreatThan, MemCmpResult::kLittleThan));
	}
}
namespace $TJ {
	using namespace $MemUtil;
}
