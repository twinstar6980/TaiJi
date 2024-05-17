# pragma once

# include "../../Type.h"
# include "../../Const.h"
# include "../../MyType/Interval.h"

namespace $TJ::$DataUtil::$Compressor::$ZLIB {
	constexpr Interval<int> kCompressLvInterval(0, 9, kFalse, kFalse);

	SzI GetCompressBound(SzI const & srcSz);

	Void Compress(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI & dstSz, SzI const & lv = kCompressLvInterval._max);
	Void UnCompress(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI const & dstSz);
	Void UnCompress_WithSize(Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI & dstSz);
}
