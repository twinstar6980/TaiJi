#pragma once

#include "../Type.hpp"

namespace $TJ::$Util::$Image::$Encoder::$PVRTC {

	namespace $4BPP {

		inline constexpr Sz kBPP(4);

		Void encRGB(Bitmap const & src, ByteSection & dst);
		Void encRGBA(Bitmap const & src, ByteSection & dst);

		Void decRGB(ByteSection const & src, Bitmap & dst);
		Void decRGBA(ByteSection const & src, Bitmap & dst);

	}

}
