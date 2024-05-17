# include "./Spliter_Export.h"

# include "../../UtilKit/JSONUtil.h"

namespace $TJ::$ImageUtil::$Spliter {
	namespace $Cat {
		Void expCat(ListB<SzI> const & atlasSz, Path const & imageInfoListFile, Path const & srcDir, Path const & dstFile) {
			return cat(ImageSize(atlasSz[kThFirst], atlasSz[kThSecond]), ListOfInfo<ImageInfo>().read($JSON::read(imageInfoListFile, $JSON::Arr().setNull())), srcDir, dstFile);
		}
		Void E_CatByAtlasInfo(Path const & atlasInfoFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return cat(AtlasInfo().read($JSON::read(atlasInfoFile, $JSON::Obj().setNull())), srcDir, dstDir, useSingleDir);
		}
		Void E_CatByAtlasInfoList(Path const & atlasInfoListFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return cat(ListOfInfo<AtlasInfo>().read($JSON::read(atlasInfoListFile, $JSON::Arr().setNull())), srcDir, dstDir, useSingleDir);
		}
	}
	namespace $Split {
		Void expSplit(Path const & imageInfoListFile, Path const & srcFile, Path const & dstDir) {
			return Split(ListOfInfo<ImageInfo>().read($JSON::read(imageInfoListFile, $JSON::Arr().setNull())), srcFile, dstDir);
		}
		Void E_SplitByAtlasInfo(Path const & atlasInfoFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return Split(AtlasInfo().read($JSON::read(atlasInfoFile, $JSON::Obj().setNull())), srcDir, dstDir, useSingleDir);
		}
		Void E_SplitByAtlasInfoList(Path const & atlasInfoListFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return Split(ListOfInfo<AtlasInfo>().read($JSON::read(atlasInfoListFile, $JSON::Arr().setNull())), srcDir, dstDir, useSingleDir);
		}
	}
}
