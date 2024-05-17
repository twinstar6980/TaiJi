# include "./Encoder_Export.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../MyType/Path.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Encoder {
	namespace $EnCode {
		Void E_EnCode_Dflt(Path const & jsonFile, Path const & popAnimFile, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			EnCode(data, sz, $StructInfo::Struct().read($JSON::read(jsonFile, $JSON::Obj().setNull())));
			$DevUtil::WriteData(popAnimFile, data, sz);
			delPtrArr(data);
			return;
		}
	}
	namespace $DeCode {
		Void E_DeCode_Dflt(Path const & popAnimFile, Path const & jsonFile) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(popAnimFile, data, sz);
			$JSON::write(jsonFile, DeCode(data, sz, $StructInfo::Struct().getThis()).write($JSON::Obj().setNull()));
			delPtrArr(data);
			return;
		}
	}
}
