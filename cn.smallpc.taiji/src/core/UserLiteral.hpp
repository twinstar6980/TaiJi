#pragma once

#include "./Type.hpp"
#include "./Const.hpp"
#include "./TypeUtil.hpp"

namespace $TJ {

	inline constexpr auto operator""_bin8(char const * const s, std::size_t const len) {
		return cnvCStrToBin<IU8>(s, static_cast<Sz>(len));
	}
	inline constexpr auto operator""_bin16(char const * const s, std::size_t const len) {
		return cnvCStrToBin<IU16>(s, static_cast<Sz>(len));
	}
	inline constexpr auto operator""_bin32(char const * const s, std::size_t const len) {
		return cnvCStrToBin<IU32>(s, static_cast<Sz>(len));
	}
	inline constexpr auto operator""_bin64(char const * const s, std::size_t const len) {
		return cnvCStrToBin<IU64>(s, static_cast<Sz>(len));
	}

	inline constexpr auto operator""_len(char const * const s, std::size_t const len) {
		return getCStrLen(s); // return len(size_t)
	}

	inline constexpr auto operator""_sh32(char const * const s, std::size_t const len) {
		// fnv-1 32bits hash
		constexpr IU32 offset(2166136261);
		constexpr IU32 prime(16777619);
		auto r = offset;
		for (auto i = kNo1; i < len; ++i) {
			r *= prime;
			r ^= static_cast<IU8>(s[i]);
		}
		return r;
	}
	inline constexpr auto operator""_sh64(char const * const s, std::size_t const len) {
		// fnv-1 64bits hash
		constexpr IU64 offset(14695981039346656037);
		constexpr IU64 prime(1099511628211);
		auto r = offset;
		for (auto i = kNo1; i < len; ++i) {
			r *= prime;
			r ^= static_cast<IU8>(s[i]);
		}
		return r;
	}

	inline constexpr auto operator""_sh(char const * const s, std::size_t const len) {
		return operator""_sh64(s, len);
	}

}
