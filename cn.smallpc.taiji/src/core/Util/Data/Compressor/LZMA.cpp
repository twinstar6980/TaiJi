#include "./LZMA.hpp"

#include "../../../../dependency/lzma/LzmaLib.h"

namespace $TJ::$Util::$Data::$Compressor::$LZMA {

	using LZMASizeT = ::SizeT;

	namespace $Compress {

		extern Void compress(ByteSection & props, ByteSection const & src, ByteSection & dst) {
			assert(dst.isEmpty());
			LZMASizeT tmpSrcSz, tmpDstSz, tmpPropsSz;
			tmpSrcSz = src.$sz;
			tmpDstSz = dst.$cap;
			tmpPropsSz = kPropsSz;
		#if defined PLATFORM_WINDOWS
			::LzmaCompress(dst.$data, &tmpDstSz, src.$data, tmpSrcSz, props.$data, &tmpPropsSz, -1, 0, -1, -1, -1, -1, -1);
		#endif
			dst.setSz(tmpDstSz);
			props.setSz(tmpPropsSz);
			return;
		}

	}

	namespace $UnCompress {

		extern Void uncompress(ByteSection const & props, ByteSection const & src, ByteSection & dst) {
			assert(dst.isEmpty());
			LZMASizeT tmpSrcSz, tmpDstSz;
			tmpSrcSz = src.$sz;
			tmpDstSz = dst.$cap;
		#if defined PLATFORM_WINDOWS
			::LzmaUncompress(dst.$data, &tmpDstSz, src.$data, &tmpSrcSz, props.$data, kPropsSz);
		#endif
			dst.setSz(tmpDstSz);
			return;
		}

	}

}
