# include "./Encryptor_Export.h"

# include "./XOR/Encryptor.h"
# include "./Base64/Encryptor.h"
# include "./PvZ2_CHS_IRE/Encryptor.h"

# include "../../UtilKit/DevUtil.h"

namespace $TJ::$DataUtil::$Encryptor {
	namespace $XOR {
		Void E_Crypt(Path const & srcFile, Path const & dstFile, Byte const & key) {
			Byte * srcData(kNullPtr); SzI srcSz;
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			Crypt(srcData, key, srcSz);
			$DevUtil::WriteData(dstFile, srcData, srcSz);
			delPtrArr(srcData);
			return;
		}
		Void E_CryptByStrKey(Path const & srcFile, Path const & dstFile, Str const & key) {
			Byte * srcData(kNullPtr); SzI srcSz;
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			Crypt(srcData, srcSz, reinterpret_cast<Byte const *>(key._str), key._len, srcData);
			$DevUtil::WriteData(dstFile, srcData, srcSz);
			delPtrArr(srcData);
			return;
		}
	}
	namespace $Base64 {
		Void expEnCrypt(Path const & srcFile, Path const & dstFile) {
			Byte * srcData(kNullPtr); SzI srcSz;
			Byte * dstData(kNullPtr); SzI dstSz;
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			dstSz = CalcCipherSize(srcSz);
			newPtr(dstData, dstSz);
			EnCrypt(srcData, srcSz, dstData);
			$DevUtil::WriteData(dstFile, dstData, dstSz);
			delPtrArr(srcData); delPtrArr(dstData);
			return;
		}
		Void expDeCrypt(Path const & srcFile, Path const & dstFile) {
			Byte * srcData(kNullPtr); SzI srcSz;
			Byte * dstData(kNullPtr); SzI dstSz;
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			dstSz = CalcPlainSize(srcSz);
			newPtr(dstData, dstSz);
			DeCrypt(srcData, srcSz, dstData);
			$DevUtil::WriteData(dstFile, dstData, dstSz - CalcPlainSizeDiff(srcData, srcSz));
			delPtrArr(srcData); delPtrArr(dstData);
			return;
		}
	}
	namespace $PvZ2_CHS_IRE {
		namespace $EnCrypt {
			Void expEnCrypt(Path const & srcFile, Path const & dstFile, SzI const & dataBlockSum, SzI const & bufBlockSum, Str const & key, Str const & iv) {
				Byte * srcData(kNullPtr); SzI srcSz, fullSrcSz;
				Byte * dstData(kNullPtr);
				$DevUtil::ForceReadData(srcFile, srcData, srcSz);
				fullSrcSz = getEnoughSz(srcSz, dataBlockSum * $DataUtil::$Encryptor::$PvZ2_CHS_IRE::kBlockSize);
				newPtr(dstData, fullSrcSz);
				EnCrypt(srcData, dstData, srcSz, dataBlockSum, bufBlockSum, key, iv);
				$DevUtil::WriteData(dstFile, dstData, fullSrcSz);
				delPtrArr(srcData); delPtrArr(dstData);
				return;
			}
		}
		namespace $DeCrypt {
			Void expDeCrypt(Path const & srcFile, Path const & dstFile, SzI const & dataBlockSum, SzI const & bufBlockSum, Str const & key, Str const & iv) {
				Byte * srcData(kNullPtr); SzI srcSz;
				Byte * dstData(kNullPtr);
				$DevUtil::ForceReadData(srcFile, srcData, srcSz);
				newPtr(dstData, srcSz);
				DeCrypt(srcData, dstData, srcSz, dataBlockSum, bufBlockSum, key, iv);
				$DevUtil::WriteData(dstFile, dstData, srcSz);
				delPtrArr(srcData); delPtrArr(dstData);
				return;
			}
		}
	}
}
