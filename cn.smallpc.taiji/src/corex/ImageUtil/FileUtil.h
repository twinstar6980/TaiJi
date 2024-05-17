# pragma once

# include "../Type.h"
# include "../MyType/PairVal.h"
# include "../MyType/JSON.h"
# include "../MyType/Path.h"

# include "./ImageType.h"

namespace $TJ::$ImageUtil::$FileUtil {
	namespace $PNGUtil {
		ImageSize & getSz(Path const & path, ImageSize & sz);
		Void read(Path const & path, Bitmap & image);
		Void write(Path const & path, Bitmap const & image);
		Void write(Path const & path, Bitmap const & image, ImagePos const & pos, ImageSize const & sz);
	}
}
