# pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/StrList.h"
# include "../../MyType/Path.h"

# include "./StructInfo.h"
# include "./Struct.h"

namespace $TJ::$ResPkgUtil::$DZUtil::$Packer {
	namespace $Pack {
		Void Pack(Byte * const & data, SzI & sz, $StructInfo::StructInfo const & structInfo, Path const & resDir);
	}
	namespace $UnPack {
		Void UnPack(Byte const * const & data, SzI const & sz, Path const & structInfoFile, Path const & resDir);
	}
}
