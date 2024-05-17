# include "./ZLIB.h"

# include "../../Type.h"
# include "../../../lib/zlib/zlib.h"

namespace $TJ::$DataUtil::$Compressor::$ZLIB {

	SzI GetCompressBound(SzI const & srcSz) { return static_cast<SzI>(compressBound(static_cast<uLong>(srcSz))); }

	Void Compress(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI & dstSz, SzI const & lv) {
		auto dstSzTmp = compressBound(srcSz);
		compress2(dst, &dstSzTmp, src, srcSz, lv);
		dstSz = static_cast<SzI>(dstSzTmp);
		return;
	}
	Void UnCompress(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI const & dstSz) {
		auto dstSzTmp = static_cast<uLongf>(dstSz);
		uncompress(dst, &dstSzTmp, src, srcSz);
		return;
	}
	Void UnCompress_WithSize(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI & dstSz) {
		auto dstSzTmp = static_cast<uLongf>(dstSz);
		uncompress(dst, &dstSzTmp, src, srcSz);
		dstSz = static_cast<SzI>(dstSzTmp);
		return;
	}
}
