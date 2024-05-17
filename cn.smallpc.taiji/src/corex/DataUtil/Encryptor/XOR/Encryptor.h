#pragma once

# include "../../../Type.h"

namespace $TJ::$DataUtil::$Encryptor::$XOR {
	tmpl_st(T) inline auto & Crypt(T & src, T const & key) {
		src ^= key;
		return src;
	}
	tmpl_st(T) inline auto & Crypt(T * const & src, T const & key, SzI const & sum) {
		for_cnt(idx, kThFirst, sum)
			Crypt(src[idx], key);
		return src;
	}
	tmpl_st(T) inline auto & Crypt(T * const & src, T const * const & key, SzI const & sum) {
		for_cnt(idx, kThFirst, sum)
			Crypt(src[idx], key[idx]);
		return src;
	}
	tmpl_st(T) inline auto & Crypt(T const & src, T const & key, T & dst) {
		dst = src ^ key;
		return dst;
	}
	tmpl_st(T) inline auto & Crypt(T const * const & src, T const & key, SzI const & sum, T * const & dst) {
		for_cnt(idx, kThFirst, sum)
			Crypt(src[idx], key, dst[idx]);
		return dst;
	}
	tmpl_st(T) inline auto & Crypt(T const * const & src, T const * const & key, SzI const & sum, T * const & dst) {
		for_cnt(idx, kThFirst, sum)
			Crypt(src[idx], key[idx], dst[idx]);
		return dst;
	}
	inline auto & Crypt(Byte const * const & src, SzI const & srcSz, Byte const * const & key, SzI const & keySz, Byte * const & dst) {
		for_cnt(idx, kThFirst, srcSz)
			dst[idx] = src[idx] ^ key[idx % keySz];
		return dst;
	}
}
