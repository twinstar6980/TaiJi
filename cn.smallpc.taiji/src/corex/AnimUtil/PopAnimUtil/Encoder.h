# pragma once

# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../Type.h"

# include "../../MyType/ListOfOther.h"
# include "../../MyType/StrList.h"
# include "../../MyType/Path.h"

# include "./Struct.h"
# include "./StructInfo.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Encoder {
	namespace $EnCode {
		Void EnCode(Byte * const & data, SzI & sz, $StructInfo::Struct const & src);
	}
	namespace $DeCode {
		$StructInfo::Struct & DeCode(Byte const * const & data, SzI const & sz, $StructInfo::Struct & dst);
	}
}
