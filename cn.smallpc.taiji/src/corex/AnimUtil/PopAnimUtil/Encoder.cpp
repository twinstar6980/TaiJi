# include "./Encoder.h"
# include "../../UtilKit/JSONUtil.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Encoder {
	namespace $EnCode {
		Void EnCode(Byte * const & data, SzI & sz, $StructInfo::Struct const & src) {
			$Struct::Struct pamStruct;
			src.parseTo(pamStruct);
			pamStruct.write(data, sz);
			pamStruct.tfree();
			return;
		}
	}
	namespace $DeCode {
		$StructInfo::Struct & DeCode(Byte const * const & data, SzI const & sz, $StructInfo::Struct & dst) {
			return dst.parse($Struct::Struct().read(data));
		}
	}
}
