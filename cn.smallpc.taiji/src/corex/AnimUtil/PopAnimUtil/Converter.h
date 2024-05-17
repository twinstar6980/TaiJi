# pragma once

# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../Type.h"

# include "../../MyType/ListOfOther.h"
# include "../../MyType/StrList.h"
# include "../../MyType/Path.h"

# include "./Struct.h"
# include "./StructInfo.h"
# include "./RipePAMStructInfo.h"
#include "../../UtilKit/JSONUtil.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Converter {
	namespace $CnvToXFL {
		Void Cnv($StructInfo::Struct const & pam, Path const & xflDir, SzI const & imgSz);
	}
	namespace $CnvFromXFL {
		$StructInfo::Struct & Cnv(Path const & xflDir, $StructInfo::Struct & pam, SzI const & imgSz);
	}
	namespace $XFLScaleCnv {
		Void cnv(Path const & xflDir, SzI const & srcSz, SzI const & dstSz);
	}
	namespace $CnvToRipe {
		$RipePAMStructInfo::StructInfo & Cnv($StructInfo::Struct const & src, $RipePAMStructInfo::StructInfo & dst);
	}
}
