#pragma once

# include "./TmpCode.h"

# include "./Type.h"
# include "./SomethingElse.h"

# include "./MyType/Str.h"
# include "./MyType/StrList.h"

namespace $TJ {
	namespace $Deprecated {}
	namespace $NonStdFunc {
		Void getFileNameNoPathNoSuffixFromDir(Path const & srcDir, Path const & dstFile);
		Void getEventIDFromPAMDir(Path const & srcDir, Path const & dstFile);
		Void BatchCpyLevelFile(Path const & srcFile, Path const & dstDir, Str const & prefix, Str const & suffix, SzI const & sum);
		Void ReNameFileCase(Path const & oriListDataFile, Path const & tgtDir);
		Void MixBNKSubList(Path const & srcAFile, Path const & srcBFile, Path const & dstFile);
		Void CompressRawRSBSMFData(Byte const * const & src, SzI const & srcSz, Byte *& dst, SzI & dstSz);
		Void UnCompressRawRSBSMFData(Byte const * const & src, SzI const & srcSz, Byte *& dst, SzI & dstSz);
		Void CompressRawRSBSMF(Path const & srcFile, Path const & dstFile);
		Void UnCompressRawRSBSMF(Path const & srcFile, Path const & dstFile);
		Void CnvWEM(Path const & srcFile, Path const & dstFile);
		Void XCatRSBResInfoByPart(Path const & rawResInfoPartDir, Path const & partIDListFile, Path const & partInfoListFile, Path const & resInfoFile);
		Void XSplitRSBResInfoByPart(Path const & rawResInfoPartDir, Path const & partInfoListFile, Path const & resInfoFile, SzI const & memSz);
		Void MixRSBResInfoToHD(Path const & baseInfoFile, Path const & hdInfoFile, Path const & dstInfoFile, SzI const & lowResID, SzI const & highResID);
		Void MixRSBStructInfoToHD(Path const & baseInfoFile, Path const & hdInfoFile, Path const & dstInfoFile, SzI const & lowResID, SzI const & highResID);
	}
}
