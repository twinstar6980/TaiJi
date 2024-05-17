# pragma once

# include "../../MyType/Path.h"
# include "./ZLIB.h"
# include "./LZMA.h"

namespace $TJ::$DataUtil::$Compressor {
	namespace $ZLIB {
		Void E_Compress(Path const & srcFile, Path const & dstFile, SzI const & ofs = kThFirst, SzI const & lv = kCompressLvInterval._max);
		Void E_UnCompress(Path const & srcFile, Path const & dstFile, SzI const & ofs = kThFirst, SzI const & bufRate = kSzNone);
	}
	namespace $LZMA {
	}
}
