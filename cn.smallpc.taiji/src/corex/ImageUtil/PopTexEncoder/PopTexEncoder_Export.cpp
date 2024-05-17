# include "./PopTexEncoder_Export.h"

# include "../../UtilKit/JSONUtil.h"

namespace $TJ::$ImageUtil::$PopTexEncoder {
	namespace $EnCode {
		Void E_EnCode(Path const & srcFile, Path const & dstFile, ListB<SzI> const & sz, Str const & texFmt) {
			return EnCode(srcFile, dstFile, ImageSize(sz[kThFirst], sz[kThSecond]), GetTexFmt(texFmt));
		}
		Void E_EnCodeAtlasList(Path const & atlasListFile, Path const & srcDir, Path const & dstDir) {
			return EnCodeAtlasList(ListOfInfo<AtlasInfo>().read($JSON::read(atlasListFile, $JSON::Arr().setNull())), srcDir, dstDir);
		}
	}
	namespace $DeCode {
		Void E_DeCode(Path const & srcFile, Path const & dstFile, ListB<SzI> const & sz, Str const & texFmt) {
			return DeCode(srcFile, dstFile, ImageSize(sz[kThFirst], sz[kThSecond]), GetTexFmt(texFmt));
		}
		Void E_DeCodeAtlasList(Path const & atlasListFile, Path const & srcDir, Path const & dstDir) {
			return DeCodeAtlasList(ListOfInfo<AtlasInfo>().read($JSON::read(atlasListFile, $JSON::Arr().setNull())), srcDir, dstDir);
		}
	}
}
