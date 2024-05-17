#include "./ZLib.hpp"

#include "../../../Type.hpp"
#include "../../File/File.hpp"

#include "../../../../dependency/zlib/zlib.h"

namespace $TJ::$Util::$Data::$Compressor::$ZLib {

	using ZLibULong = ::uLongf;

	inline auto compressBound(Sz const & sz) {
		return static_cast<Sz>(::compressBound(sz));
	}

	namespace $Compress {

		extern Sz calcMaxSz(Sz const & sz) {
			return compressBound(sz);
		}

		extern Void compress(ByteSection const & src, ByteSection & dst) {
			assert(dst.isEmpty());
			auto dstSzTmp = static_cast<ZLibULong>(dst.$cap);
			::compress2(dst.$data, &dstSzTmp, src.$data, src.$sz, int(9));
			dst.setSz(static_cast<Sz>(dstSzTmp));
			return;
		}

		extern Void compressExport(Path const & src, Path const & dst) {
			ByteSection srcData, dstData;
			$File::readFileForce(src, srcData);
			dstData.alloc(calcMaxSz(srcData.$sz));
			compress(srcData, dstData);
			$File::writeFile(dst, dstData);
			srcData.free();
			dstData.free();
			return;
		}

	}

	namespace $UnCompress {

		extern Void uncompress(ByteSection const & src, ByteSection & dst) {
			assert(dst.isEmpty());
			auto dstSzTmp = static_cast<ZLibULong>(dst.$cap);
			::uncompress(dst.$data, &dstSzTmp, src.$data, src.$sz);
			dst.setSz(static_cast<Sz>(dstSzTmp));
			return;
		}

		extern Void uncompressExport(Path const & src, Path const & dst, Sz const & dstCap) {
			ByteSection srcData, dstData;
			$File::readFileForce(src, srcData);
			dstData.alloc(dstCap);
			uncompress(srcData, dstData);
			$File::writeFile(dst, dstData);
			srcData.free();
			dstData.free();
			return;
		}

	}

}
