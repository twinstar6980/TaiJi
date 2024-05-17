# pragma once

# include "./Converter.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Converter {
	namespace $CnvToXFL {
		Void expCnv(Path const & jsonFile, Path const & xflDir, SzI const & imgSz);
		Void E_Cnv_OneStep(Path const & popAnimFile, Path const & xflDir, SzI const & imgSz);
	}
	namespace $CnvFromXFL {
		Void expCnv(Path const & xflDir, Path const & jsonFile, SzI const & imgSz);
		Void E_Cnv_OneStep(Path const & xflDir, Path const & popAnimFile, SzI const & imgSz, SzI const & memSz);
	}
	namespace $XFLScaleCnv {
		Void E_cnv(Path const & xflDir, SzI const & srcSz, SzI const & dstSz);
	}
	namespace $CnvToRipe {
		Void expCnv(Path const & jsonFile, Path const & ripeJSONFile);
		Void E_Cnv_OneStep(Path const & popAnimFile, Path const & ripeJSONFile);
	}
}
