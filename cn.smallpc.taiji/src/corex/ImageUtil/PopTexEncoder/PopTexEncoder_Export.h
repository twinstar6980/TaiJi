# pragma once

# include "./PopTexEncoder.h"

namespace $TJ::$ImageUtil::$PopTexEncoder {
	namespace $EnCode {
		Void E_EnCode(Path const & srcFile, Path const & dstFile, ListB<SzI> const & sz, Str const & texFmt);
		Void E_EnCodeAtlasList(Path const & atlasListFile, Path const & srcDir, Path const & dstDir);
	}
	namespace $DeCode {
		Void E_DeCode(Path const & srcFile, Path const & dstFile, ListB<SzI> const & sz, Str const & texFmt);
		Void E_DeCodeAtlasList(Path const & atlasListFile, Path const & srcDir, Path const & dstDir);
	}
}
