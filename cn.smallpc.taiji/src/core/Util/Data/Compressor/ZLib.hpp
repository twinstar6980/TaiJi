#pragma once

#include "../../../Type.hpp"
#include "../../../Type/ByteStream.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$Data::$Compressor::$ZLib {

	namespace $Compress {

		extern Sz calcMaxSz(Sz const & sz);

		extern Void compress(ByteSection const & src, ByteSection & dst);

		extern Void compressExport(Path const & src, Path const & dst);

	}

	namespace $UnCompress {

		extern Void uncompress(ByteSection const & src, ByteSection & dst);

		extern Void uncompressExport(Path const & src, Path const & dst, Sz const & dstCap);

	}

}
