#pragma once

# include "../../../Type.h"
# include "../../../Const.h"

namespace $TJ::$DataUtil::$Encryptor::$Base64 {
	inline auto CalcCipherSize(SzI const & plainSz) {
		return static_cast<SzI>((plainSz % 3 == 0) ? (plainSz / 3 * 4) : ((plainSz / 3 + 1) * 4));
	}
	inline auto CalcPlainSize(SzI const & cipherSz) {
		return static_cast<SzI>(cipherSz / 4 * 3);
	}
	inline auto CalcPlainSizeDiff(Byte const * const & cipher, SzI const & cipherSz) {
		auto diffSz = kSzZero;
		if (cipher[cipherSz - 1] == '=') ++diffSz;
		if (cipher[cipherSz - 2] == '=') ++diffSz;
		return diffSz;
	}
	Void EnCrypt(Byte const * const & plain, SzI const & plainSz, Byte * const & cipher);
	Void DeCrypt(Byte const * const & cipher, SzI const & cipherSz, Byte * const & plain);
}
