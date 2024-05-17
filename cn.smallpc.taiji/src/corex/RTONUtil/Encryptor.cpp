# include "./Encryptor.h"

# include "../DataUtil/Hash/Hash.h"
# include "../DataUtil/Encryptor/PvZ2_CHS_IRE/Encryptor.h"

namespace $TJ::$RTONUtil::$Encryptor {

# if defined VER_ULTIMATE
	static Str const kDfltPlainKey("/* deleted */");
	static constexpr SzI kIVStrOfs(4);
	static constexpr SzI kIVStrLen(24);
# endif

	namespace $EnCrypt {
		Void enCrypt(Byte const * const & src, Byte * const & dst, SzI const & srcSz, Str const & key) {
		# if defined VER_ULTIMATE
			Str cipherKey;
			$DataUtil::$Hash::$MD5::make(key.isEmpty() ? kDfltPlainKey : key, cipherKey);
			memCpy(&dst[0], &kRTONMark[0], kRTONMarkSize);
			$DataUtil::$Encryptor::$PvZ2_CHS_IRE::$EnCrypt::EnCrypt(src, dst + kRTONMarkSize, srcSz, 6, 8, cipherKey, Str(cipherKey, kIVStrOfs, kIVStrLen));
			return;
		# endif
		}
	}
	namespace $DeCrypt {
		Void deCrypt(Byte const * const & src, Byte * const & dst, SzI const & srcSz, Str const & key) {
		# if defined VER_ULTIMATE
			if (memCmp(&src[0], &kRTONMark[0], kRTONMarkSize) != MemCmpResult::kEqual) wErr("ERROR-Mark");
			Str cipherKey;
			$DataUtil::$Hash::$MD5::make(key.isEmpty() ? kDfltPlainKey : key, cipherKey);
			$DataUtil::$Encryptor::$PvZ2_CHS_IRE::$DeCrypt::DeCrypt(src + kRTONMarkSize, dst, srcSz, 6, 8, cipherKey, Str(cipherKey, kIVStrOfs, kIVStrLen));
			return;
		# endif
		}
	}
}
