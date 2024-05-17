# pragma once

# include "./Encoder.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Encoder {
	namespace $EnCode {
		Void E_EnCode_Dflt(Path const & jsonFile, Path const & popAnimFile, SzI const & memSz);
	}
	namespace $DeCode {
		Void E_DeCode_Dflt(Path const & popAnimFile, Path const & jsonFile);
	}
}
