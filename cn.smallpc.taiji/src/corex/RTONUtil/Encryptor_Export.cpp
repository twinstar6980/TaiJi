# include "./Encryptor_Export.h"

# include "../DataUtil/Encryptor/PvZ2_CHS_IRE/Encryptor.h"
# include "../UtilKit/DevUtil.h"
# include "../UtilKit/JSONUtil.h"

namespace $TJ::$RTONUtil::$Encryptor {
	namespace $EnCrypt {
		Void expEnCrypt(Path const & srcFile, Path const & dstFile, Str const & key) {
			Byte * srcData(kNullPtr); SzI srcSz, fullSrcSz;
			Byte * dstData(kNullPtr);
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			fullSrcSz = getEnoughSz(srcSz, 6 * $DataUtil::$Encryptor::$PvZ2_CHS_IRE::kBlockSize);
			newPtr(dstData, fullSrcSz + kRTONMarkSize);
			enCrypt(srcData, dstData, srcSz, key);
			$DevUtil::WriteData(dstFile, dstData, fullSrcSz + kRTONMarkSize);
			delPtrArr(srcData);
			delPtrArr(dstData);
			return;
		}
	}
	namespace $DeCrypt {
		Void expDeCrypt(Path const & srcFile, Path const & dstFile, Str const & key) {
			Byte * srcData(kNullPtr); SzI srcSz;
			Byte * dstData(kNullPtr);
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			newPtr(dstData, srcSz - kRTONMarkSize);
			deCrypt(srcData, dstData, srcSz, key);
			$DevUtil::WriteData(dstFile, dstData, srcSz - kRTONMarkSize);
			delPtrArr(srcData);
			delPtrArr(dstData);
			return;
		}
	}
}
