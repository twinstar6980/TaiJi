#pragma once

#include "../Type.hpp"

namespace $TJ::$Util::$Image::$Encoder::$ETC {

	namespace $ETC1 {

		inline constexpr Sz kBPP(4);
		inline constexpr Sz kETC1_Block_4Pixel(4);
		inline constexpr Sz kSizeOfAPart(4);
		inline constexpr Sz kETC1_4x4_BlockSize_Compressed(8);

		Void enc(Bitmap const & src, ByteSection & dst);
		Void dec(ByteSection const & src, Bitmap & dst);

	}

}
