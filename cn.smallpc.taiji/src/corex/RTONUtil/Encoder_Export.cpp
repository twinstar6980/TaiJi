# pragma once

# include "./Encoder_Export.h"

# include "../UtilKit/DevUtil.h"
# include "../UtilKit/JSONUtil.h"

namespace $TJ::$RTONUtil::$Encoder {
	namespace $EnCode {
		Void expEnCodeRTON(Path const & srcFile, Path const & dstFile, Bool const & useCacheStr, Bool const & useSpecialTypeForRTID, SzI const & memSz) {
			Byte * dstData(kNullPtr); SzI dstSz;
			$JSON::Val srcData;
			$JSON::read(srcFile, srcData);
			newPtr(dstData, memSz);
			if (!srcData.isTypeObj()) wErr("InPut JSON Not A Object");
			else {
				encFull(srcData.getObj(), dstData, dstSz, useCacheStr, useSpecialTypeForRTID);
				$DevUtil::WriteData(dstFile, dstData, dstSz);
			}
			delPtrArr(dstData);
			return;
		}
	}
	namespace $DeCode {
		Void expDeCodeRTON(Path const & srcFile, Path const & dstFile, SzI const & srcDataOfs) {
			Byte * srcData(kNullPtr); SzI srcSz;
			$JSON::Obj dstData;
			$DevUtil::ForceReadData(srcFile, srcData, srcSz);
			decFull(srcData + srcDataOfs, srcSz - srcDataOfs, dstData);
			$JSON::write(dstFile, dstData);
			delPtrArr(srcData);
			return;
		}
	}
}
