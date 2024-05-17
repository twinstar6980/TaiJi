# pragma once

# include "../Type.h"

# include "../MyType/Str.h"
# include "../MyType/StrList.h"
# include "../MyType/JSON.h"

# include "../UtilKit/PBVarIntUtil.h"
# include "./Struct.h"

namespace $TJ::$RTONUtil::$Encoder {
	namespace $EnCode {
		Void encFull($JSON::Obj const & src, Byte * const & dst, SzI & sz, Bool const & useCacheStr, Bool const & useSpecialTypeForRTID);
	}
	namespace $DeCode {
		Void decFull(Byte const * const & src, SzI const & sz, $JSON::Obj & dst);
	}
}
