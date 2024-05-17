#pragma once

#include "../../../Type.hpp"
#include "../../../Type/ByteStream.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$Data::$Compressor::$LZMA {

	inline constexpr Sz kPropsSz(5);

	using Props = Byte[kPropsSz];

	namespace $Compress {

		extern Void compress(ByteSection & props, ByteSection const & src, ByteSection & dst);

	}

	namespace $UnCompress {

		extern Void uncompress(ByteSection const & props, ByteSection const & src, ByteSection & dst);

	}

}
