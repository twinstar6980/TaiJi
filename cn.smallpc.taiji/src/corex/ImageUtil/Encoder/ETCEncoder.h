# pragma once

# include "../ImageType.h"

namespace $TJ::$ImageUtil::$Encoder::$ETCEncoder {
	namespace $ETC1 {
		constexpr SzI kBitPerPixel(4);
		constexpr SzI kETC1_Block_4Pixel(4);
		constexpr SzI kSizeOfAPart(4);
		constexpr SzI kETC1_4x4_BlockSize_Compressed(8);

		Void EnCode(Bitmap const & src, Byte * const & dst);
		Void DeCode(Byte const * const & src, Bitmap & dst);
	}
}
