# pragma once

# include "./Encryptor.h"

# include "../MyType/Path.h"

namespace $TJ::$RTONUtil::$Encryptor {
	namespace $EnCrypt {
		Void expEnCrypt(Path const & srcFile, Path const & dstFile, Str const & key);
	}
	namespace $DeCrypt {
		Void expDeCrypt(Path const & srcFile, Path const & dstFile, Str const & key);
	}
}
