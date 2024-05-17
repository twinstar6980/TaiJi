# include "./LZMA.h"

# include "../../Const.h"

# include "../../../lib/lzma/LzmaLib.h"

namespace $TJ::$DataUtil::$Compressor::$LZMA {
	Void UnCompress(Props const props, Byte const * const & src, SzI const & srcSz, Byte * const & dst, SzI const & dstSz) {
		size_t tmpSrcSz, tmpDstSz;
		tmpDstSz = dstSz;
		tmpSrcSz = srcSz; // warning!
	# if defined PLATFORM_WINDOWS
		LzmaUncompress(dst, &tmpDstSz, src, &tmpSrcSz, props, kPropsSz);
	# endif
		return;
	}
}
