# pragma once

# include "./Encoder.h"

# include "../MyType/Path.h"

namespace $TJ::$RTONUtil::$Encoder {
	namespace $EnCode {
		Void expEnCodeRTON(Path const & srcFile, Path const & dstFile, Bool const & useCacheStr, Bool const & useSpecialTypeForRTID, SzI const & memSz);
	}
	namespace $DeCode {
		Void expDeCodeRTON(Path const & srcFile, Path const & dstFile, SzI const & srcDataOfs);
	}
}
