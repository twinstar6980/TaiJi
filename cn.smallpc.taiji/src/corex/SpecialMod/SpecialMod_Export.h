# pragma once

# include "../MyType/Str.h"
# include "../UtilKit/JSONUtil.h"
# include "../MyType/ListP.h"

namespace $TJ::$SpecialMod {
	namespace $PvZ_2 {
		namespace $RSBPatch {
			Void patchResInfo(Path const & patchFile, Path const & srcFile, Path const & dstFile);
			Void patchStructInfo(Path const & patchFile, Path const & srcFile, Path const & dstFile);
			Void patchRes(Path const & patchFile, Path const & structInfoFile, Path const & srcResDir, Path const & dstGroupDir);
		}
	}
}
