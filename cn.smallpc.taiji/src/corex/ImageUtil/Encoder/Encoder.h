# pragma once

# include "../../Type.h"
# include "../../MyType/ListP.h"

# include "../FileUtil.h"
# include "../ImageInfoType.h"

# include "./ETCEncoder.h"

namespace $TJ::$ImageUtil::$Encoder {
	ImageSize & GetActualImageSize(ImageSize const & oriSz, TexFmt const & texFmt, ImageSize & actualSz);
	SzI GetActualImageByteSize(ImageSize const & sz, TexFmt const & texFmt);
	namespace $EnCode {
		Void EnCode(Bitmap const & src, Byte * const & dst, TexFmt const & texFmt);
		Void EnCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt);
	}
	namespace $DeCode {
		Void DeCode(Byte const * const & src, Bitmap & dst, TexFmt const & texFmt);
		Void DeCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt);
	}
}
