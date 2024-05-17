# pragma once

# include "../../Type.h"

namespace $TJ::$DataUtil::$Compressor::$LZMA {
	constexpr SzI kPropsSz(5);
	using Props = Byte[kPropsSz];

	constexpr SzI kSrcSz_IgnoreActualSz(0xFFFFFFFF); //danger

	// TODO
	Void Compress();
	Void UnCompress(Props const props, Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI const & dstSz);
}
