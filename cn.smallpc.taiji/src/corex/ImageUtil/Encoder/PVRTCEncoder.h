# pragma once

# include "../ImageType.h"

namespace $TJ::$ImageUtil::$Encoder::$PVRTCEncoder {
	namespace $PVRTC_4BPP {
		constexpr SzI kBitPerPixel(4);

		Void EnCodeRGB(Bitmap const & src, Byte * const & dst);
		Void EnCodeRGBA(Bitmap const & src, Byte * const & dst);
		Void DeCodeRGB(Byte const * const & src, Bitmap & dst);
		Void DeCodeRGBA(Byte const * const & src, Bitmap & dst);
	}
}
