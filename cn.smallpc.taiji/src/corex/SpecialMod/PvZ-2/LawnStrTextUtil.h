# pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/Path.h"
# include "../../MyType/JSON.h"

namespace $TJ::$SpecialMod::$PvZ_2 {
	namespace $LawnStrTextUtil {
		Str & CnvJSONToText($JSON::Obj const & src, Str & dst);
		Byte * const & CnvJSONToText_RipeUTF16($JSON::Obj const & src, Byte * const & dst, SzI & dstSz);
		Void CnvJSONToText_ByFile(Path const & srcFile, Path const & dstFile);

		$JSON::Obj & CnvTextToJSON(Ch const * const & src, SzI const & srcSz, $JSON::Obj & dst);
		$JSON::Obj & CnvTextToJSON_RipeUTF16(Ch16 const * const & src, SzI const & srcSz, $JSON::Obj & dst);
		Void CnvTextToJSON_ByFile(Path const & srcFile, Path const & dstFile);
	}
}
