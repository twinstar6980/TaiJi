# pragma once

# include "../../Type.h"
# include "../../MyType/ListP.h"

# include "../FileUtil.h"
# include "../ImageInfoType.h"

namespace $TJ::$ImageUtil::$PopTexEncoder {
	namespace $EnCode {
		Void EnCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt);
		Void EnCodeAtlas(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir);
		Void EnCodeAtlasList(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir);
	}
	namespace $DeCode {
		Void DeCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt);
		Void DeCodeAtlas(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir);
		Void DeCodeAtlasList(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir);
	}
}
