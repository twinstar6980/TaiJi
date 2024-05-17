# include "./Compressor_Export.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$DataUtil::$Compressor {
	namespace $ZLIB {
		Void E_Compress(Path const & srcFile, Path const & dstFile, SzI const & ofs, SzI const & lv) {
			Byte * srcData(kNullPtr); SzI srcSize;
			Byte * dstData(kNullPtr); SzI dstSize;
			$DevUtil::ForceReadData(srcFile, srcData, srcSize);
			newPtr(dstData, GetCompressBound(srcSize - ofs));
			Compress(srcData + ofs, srcSize - ofs, dstData, dstSize, lv);
			$DevUtil::WriteData(dstFile, dstData, dstSize);
			delPtrArr(dstData); delPtrArr(srcData);
			return;
		}
		Void E_UnCompress(Path const & srcFile, Path const & dstFile, SzI const & ofs, SzI const & bufRate) {
			Byte * srcData(kNullPtr); SzI srcSize;
			Byte * dstData(kNullPtr); SzI dstSize;
			$DevUtil::ForceReadData(srcFile, srcData, srcSize);
			dstSize = srcSize * bufRate;
			newPtr(dstData, dstSize);
			UnCompress_WithSize(srcData + ofs, srcSize - ofs, dstData, dstSize);
			$DevUtil::WriteData(dstFile, dstData, dstSize);
			delPtrArr(dstData); delPtrArr(srcData);
			return;
		}
	}
}
