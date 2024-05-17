# include "./Encryptor.h"

namespace $TJ::$DataUtil::$Encryptor::$Base64 {
	static constexpr Byte kKeyTable[]{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
	};
	static constexpr Byte kDeKeyTable[] = {
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		62, // '+'
		255, 255, 255,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		255, 255, 255, 255, 255, 255, 255,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		255, 255, 255, 255, 255, 255,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
		255, 255, 255, 255, 255
	};

	Void EnCrypt(Byte const * const & plain, SzI const & plainSz, Byte * const & cipher) {
		auto cipherSz = CalcCipherSize(plainSz);
		SzI i, j;
		for (i = 0, j = 0; i < cipherSz - 2; j += 3, i += 4) {
			cipher[i + 0] = kKeyTable[plain[j + 0] >> 2];
			cipher[i + 1] = kKeyTable[(plain[j + 0] & 0x3) << 4 | (plain[j + 1] >> 4)];
			cipher[i + 2] = kKeyTable[(plain[j + 1] & 0xf) << 2 | (plain[j + 2] >> 6)];
			cipher[i + 3] = kKeyTable[plain[j + 2] & 0x3f];
		}
		switch (plainSz % 3) {
			case 1: cipher[i - 2] = '=';
			case 2: cipher[i - 1] = '=';
			case 0: break;
		}
		return;
	}
	Void DeCrypt(Byte const * const & cipher, SzI const & cipherSz, Byte * const & plain) {
		for (SzI i = 0, j = 0; i < cipherSz - 2; j += 3, i += 4) {
			plain[j + 0] = ((kDeKeyTable[cipher[i + 0]]) << 2) | ((kDeKeyTable[cipher[i + 1]]) >> 4);
			plain[j + 1] = ((kDeKeyTable[cipher[i + 1]]) << 4) | ((kDeKeyTable[cipher[i + 2]]) >> 2);
			plain[j + 2] = ((kDeKeyTable[cipher[i + 2]]) << 6) | ((kDeKeyTable[cipher[i + 3]]) >> 0);
		}
		return;
	}
}
