#include "./Encryptor.hpp"

#include "../../Type.hpp"
#include "../../Type/ByteStream.hpp"
#include "../../Type/Path.hpp"
#include "../Data/Encryptor/Rijndael.hpp"
#include "../Data/Hasher/MD5.hpp"
#include "../File/File.hpp"

namespace $TJ::$Util::$RTON::$Encryptor {

#if defined ENABLE_RTON_CRYPTOR

	static constexpr Byte kHeader[]{ 0x10, 0x00 };
	static constexpr Sz kHeaderSz(2);

	static constexpr Sz kBlockSz(6);
	static constexpr Sz kKeySz(8);

	static Str const kDfltKey("/* deleted */"_r);

	namespace $EnCrypt {

		extern Void enc(ByteSection const & plain, ByteSection & cipher, Str const & key) {
			assert(cipher.isEmpty());
			Str rKey, rIV;
			$Data::$Hasher::$MD5::make(key.isNull() ? kDfltKey : key, rKey);
			rIV.ref(rKey, kNo5, rKey.$sz - kSz8);
			Section<IU32> rPlain, rCipher;
			if (plain.$sz % (kBlockSz * kTSz<IU32>) == kSzN) {
				rPlain.ref(reinterpret_cast<IU32 const * const>(plain.$data), plain.$sz / kTSz<IU32>);
			} else {
				auto enoughSz = calcEnoughSz(plain.$sz, kBlockSz * kTSz<IU32>);
				rPlain.allocThenUse(enoughSz / kTSz<IU32>);
				Range(reinterpret_cast<Byte * const>(rPlain.$data), plain.$sz).cpy(plain.$data);
			}
			cipher.setSz(kHeaderSz);
			cipher[kNo1] = kHeader[kNo1];
			cipher[kNo2] = kHeader[kNo2];
			rCipher.ref(reinterpret_cast<IU32 * const>(cipher.$data + kHeaderSz), kSzN, (cipher.$cap - kHeaderSz) / kTSz<IU32>);
			$Data::$Encryptor::$Rijndael::$EnCrypt::enc(rPlain, rCipher, kBlockSz, kKeySz, rKey, rIV);
			cipher.expandSz(rCipher.$sz * kTSz<IU32>);
			rPlain.free();
			rCipher.unref();
			return;
		}

		extern Void encExport(Path const & plainFile, Path const & cipherFile, Str const & key) {
			ByteSection plain, cipher;
			$File::readFileForce(plainFile, plain);
			cipher.alloc(kHeaderSz + calcEnoughSz(plain.$sz, kBlockSz * kTSz<IU32>));
			enc(plain, cipher, key);
			$File::writeFile(cipherFile, cipher);
			plain.free();
			cipher.free();
			return;
		}

	}

	namespace $DeCrypt {

		extern Void dec(ByteSection const & cipher, ByteSection & plain, Str const & key) {
			assert(plain.isEmpty());
			if (cipher.$sz < kHeaderSz || (cipher.$sz - kHeaderSz) % (kBlockSz * kTSz<IU32>) != kSzN)
				throw VAMGException("RTONDecryptor", "cipher-size error");
			if (cipher[kNo1] != kHeader[kNo1] || cipher[kNo2] != kHeader[kNo2])
				throw VAMGException("RTONDecryptor", "header error");
			auto rCipherSz = (cipher.$sz - kHeaderSz) / kTSz<IU32>;
			Str rKey, rIV;
			$Data::$Hasher::$MD5::make(key.isNull() ? kDfltKey : key, rKey);
			rIV.ref(rKey, kNo5, rKey.$sz - kSz8);
			Section<IU32> rPlain;
			rPlain.ref(reinterpret_cast<IU32 * const>(plain.$data), kSzN, plain.$cap / kTSz<IU32>);
			$Data::$Encryptor::$Rijndael::$DeCrypt::dec(Section<IU32>().ref(reinterpret_cast<IU32 * const>(cipher.$data + kHeaderSz), rCipherSz), rPlain, kBlockSz, kKeySz, rKey, rIV);
			plain.setSz(rPlain.$sz * kTSz<IU32>);
			rPlain.unref();
			return;
		}

		extern Void decExport(Path const & cipherFile, Path const & plainFile, Str const & key) {
			ByteSection cipher, plain;
			$File::readFileForce(cipherFile, cipher);
			if (cipher.$sz < kHeaderSz)
				throw VAMGException("RTONDecryptor", "cipher-size error");
			plain.alloc(cipher.$sz - kHeaderSz);
			dec(cipher, plain, key);
			$File::writeFile(plainFile, plain);
			cipher.free();
			plain.free();
			return;
		}

	}

#endif

}
