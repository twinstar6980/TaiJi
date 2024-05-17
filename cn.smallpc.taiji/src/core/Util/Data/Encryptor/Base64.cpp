#include "./Base64.hpp"

#include "../../../UserLiteral.hpp"
#include "../../../Type/List.hpp"
#include "../../File/File.hpp"

namespace $TJ::$Util::$Data::$Encryptor::$Base64 {

	static constexpr Byte kKeyTable[]{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'+', '/'
	};

	static constexpr Byte kDeKeyTable[]{
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		62, // '+'
		255, 255, 255,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		255, 255, 255, 255, 255, 255, 255,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		255, 255, 255, 255, 255, 255,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
		255, 255, 255, 255, 255
	};

	inline auto calcPlainSzDiff(ByteSection const & cipher) {
		auto r = kSzN;
		if (cipher[cipher.lastIdx() - kNo1] == '=')
			++r;
		if (cipher[cipher.lastIdx() - kNo2] == '=')
			++r;
		return r;
	}
	inline auto calcCipherSz(Sz const & sz) {
		return static_cast<Sz>((sz % kSz3 == kSz0) ? (sz / kSz3 * kSz4) : ((sz / kSz3 + kSz1) * kSz4));
	}
	inline auto calcPlainSzRaw(Sz const & sz) {
		return static_cast<Sz>(sz / kSz4 * kSz3);
	}
	inline auto calcPlainSz(ByteSection const & cipher) {
		return calcPlainSzRaw(cipher.$sz) - calcPlainSzDiff(cipher);
	}

	namespace $EnCrypt {

		extern Void enc(ByteSection const & plain, ByteSection & cipher) {
			assert(cipher.isEmpty());
			Byte tmp[kSz3];
			cipher.setSz(calcCipherSz(plain.$sz));
			Sz i, j;
			for (i = kNo1, j = kNo1; i < cipher.$sz - kSz2; i += kSz4, j += kSz3) {
				tmp[kNo1] = plain[j + kNo1];
				tmp[kNo2] = j + kNo2 < plain.$sz ? plain[j + kNo2] : kByteNull;
				tmp[kNo3] = j + kNo3 < plain.$sz ? plain[j + kNo3] : kByteNull;
				cipher[i + kNo1] = kKeyTable[(tmp[kNo1] & "11111111"_bin8) >> 2];
				cipher[i + kNo2] = kKeyTable[(tmp[kNo1] & "00000011"_bin8) << 4 | tmp[kNo2] >> 4];
				cipher[i + kNo3] = kKeyTable[(tmp[kNo2] & "00001111"_bin8) << 2 | tmp[kNo3] >> 6];
				cipher[i + kNo4] = kKeyTable[(tmp[kNo3] & "00111111"_bin8) << 0];
			}
			switch (plain.$sz % kSz3) {
				case 1:
					cipher[i - kNo3] = '=';
				case 2:
					cipher[i - kNo2] = '=';
				case 0:
					break;
			}
			return;
		}

		extern Void encExport(Path const & plainFile, Path const & cipherFile) {
			ByteSection srcData, dstData;
			$File::readFileForce(plainFile, srcData);
			dstData.alloc(calcCipherSz(srcData.$sz));
			enc(srcData, dstData);
			$File::writeFile(cipherFile, dstData);
			srcData.free();
			dstData.free();
			return;
		}

	}

	namespace $DeCrypt {

		extern Void dec(ByteSection const & cipher, ByteSection & plain) {
			assert(!plain.isNull());
			plain.setSz(calcPlainSz(cipher));
			for (auto i = kNo1, j = kNo1; i < cipher.$sz - kSz2; i += kSz4, j += kSz3) {
				plain[j + kNo1] = kDeKeyTable[cipher[i + kNo1]] << 2 | kDeKeyTable[cipher[i + kNo2]] >> 4;
				if (j + kNo2 >= plain.$sz)
					break;
				plain[j + kNo2] = kDeKeyTable[cipher[i + kNo2]] << 4 | kDeKeyTable[cipher[i + kNo3]] >> 2;
				if (j + kNo3 >= plain.$sz)
					break;
				plain[j + kNo3] = kDeKeyTable[cipher[i + kNo3]] << 6 | kDeKeyTable[cipher[i + kNo4]] >> 0;
			}
			return;
		}

		extern Void decExport(Path const & cipherFile, Path const & plainFile) {
			ByteSection srcData, dstData;
			$File::readFileForce(cipherFile, srcData);
			dstData.alloc(calcPlainSz(srcData));
			dec(srcData, dstData);
			$File::writeFile(plainFile, dstData);
			srcData.free();
			dstData.free();
			return;
		}

	}

}
