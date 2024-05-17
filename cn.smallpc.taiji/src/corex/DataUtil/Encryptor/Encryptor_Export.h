# pragma once

# include "../../MyType/Path.h"

namespace $TJ::$DataUtil::$Encryptor {
	namespace $XOR {
		Void E_Crypt(Path const & srcFile, Path const & dstFile, Byte const & key);
		Void E_CryptByStrKey(Path const & srcFile, Path const & dstFile, Str const & key);
	}
	namespace $Base64 {
		Void expEnCrypt(Path const & srcFile, Path const & dstFile);
		Void expDeCrypt(Path const & srcFile, Path const & dstFile);
	}
	namespace $PvZ2_CHS_IRE {
		namespace $EnCrypt {
			Void expEnCrypt(Path const & srcFile, Path const & dstFile, SzI const & dataBlockSum, SzI const & bufBlockSum, Str const & key, Str const & iv);
		}
		namespace $DeCrypt {
			Void expDeCrypt(Path const & srcFile, Path const & dstFile, SzI const & dataBlockSum, SzI const & bufBlockSum, Str const & key, Str const & iv);
		}
	}
}
