# pragma once

# include "./Spliter.h"

namespace $TJ::$ImageUtil::$Spliter {
	namespace $Cat {
		Void expCat(ListB<SzI> const & atlasSz, Path const & imageInfoListFile, Path const & srcDir, Path const & dstFile);
		Void E_CatByAtlasInfo(Path const & atlasInfoFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
		Void E_CatByAtlasInfoList(Path const & atlasInfoListFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
	}
	namespace $Split {
		Void expSplit(Path const & imageInfoListFile, Path const & srcFile, Path const & dstDir);
		Void E_SplitByAtlasInfo(Path const & atlasInfoFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
		Void E_SplitByAtlasInfoList(Path const & atlasInfoListFile, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir);
	}
}
