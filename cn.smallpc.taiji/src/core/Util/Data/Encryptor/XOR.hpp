#pragma once

#include "../../../Type.hpp"
#include "../../../Type/List.hpp"
#include "../../../Type/Path.hpp"

#include "../../File/File.hpp"

namespace $TJ::$Util::$Data::$Encryptor::$XOR {

	template <typename T>
	inline auto & crypt(T & t, T const & k) {
		t ^= k;
		return t;
	}
	template <typename T>
	inline auto & crypt(List<T> & t, T const & k) {
		for (auto i = kNo1; i < t.$sz; ++i)
			crypt(t[i], k);
		return t;
	}
	template <typename T>
	inline auto & crypt(List<T> & t, List<T> const & k) {
		assert(t.$sz == k.$sz);
		for (auto i = kNo1; i < t.$sz; ++i)
			crypt(t[i], k[i]);
		return t;
	}

	template <typename T>
	inline auto & crypt(T const & src, T & dst, T const & k) {
		dst = src ^ k;
		return dst;
	}
	template <typename T>
	inline auto & crypt(List<T> const & src, List<T> & dst, T const & k) {
		for (auto i = kNo1; i < src.$sz; ++i)
			crypt(src[i], dst[i], k);
		return dst;
	}
	template <typename T>
	inline auto & crypt(List<T> const & src, List<T> & dst, List<T> const & k) {
		assert(src.$sz == k.$sz);
		for (auto i = kNo1; i < src.$sz; ++i)
			crypt(src[i], dst[i], k[i]);
		return dst;
	}

	inline Void cryptExport(Path const & plainFile, Path const & cipherFile, Byte const & key) {
		ByteSection data;
		$File::readFileForce(plainFile, data);
		crypt(data, key);
		$File::writeFile(cipherFile, data);
		data.free();
		return;
	}

}
