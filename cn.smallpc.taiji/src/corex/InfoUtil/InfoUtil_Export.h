# pragma once

# include "./InfoUtil.h"

namespace $TJ::$InfoUtil {
	namespace $Cnv {
		Void expCnv(Str const & srcInfoType, Str const & dstInfoType, Path const & srcInfoFile, Path const & dstInfoFile, Path const & appendInfoFile);
	}
	namespace $Cpy {
		Void expCpy(Str const & infoType, Path const & srcFile, Path const & dstFile);
	}
	namespace $Spliter {
		Void expSplit(Str const & infoType, Path const & srcFile, Path const & dstDir);
		Void expCat(Str const & infoType, Path const & dstFile, Path const & srcDir);
	}
	namespace $Lite {
		Void expLite(Str const & infoType, Path const & srcFile, Path const & dstFile, ListB<SzI> const & resIDList);
	}
}
