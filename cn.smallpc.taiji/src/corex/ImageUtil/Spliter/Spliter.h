# pragma once

# include "../../Type.h"
# include "../../MyType/ListP.h"

# include "../FileUtil.h"
# include "../ImageInfoType.h"

namespace $TJ::$ImageUtil::$Spliter {
	namespace $Cat {
		Void cat(ImageSize const & atlasSz, ListP<ImageInfo> const & imageInfoList, Path const & srcDir, Path const & dstFile);
		Void cat(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
		Void cat(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
	}
	namespace $Split {
		Void Split(ListP<ImageInfo> const & imageInfoList, Path const & srcFile, Path const & dstDir);
		Void Split(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
		Void Split(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
	}
}
