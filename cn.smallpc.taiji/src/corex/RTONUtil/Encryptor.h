# pragma once

# include "../Type.h"

# include "../MyType/Str.h"

namespace $TJ::$RTONUtil::$Encryptor {
	constexpr SzI kRTONMarkSize(2);
	constexpr Byte kRTONMark[kRTONMarkSize]{ 0x10, 0x00 };

	namespace $EnCrypt {
		Void enCrypt(Byte const * const & src, Byte * const & dst, SzI const & srcSz, Str const & key);
	}
	namespace $DeCrypt {
		Void deCrypt(Byte const * const & src, Byte * const & dst, SzI const & srcSz, Str const & key);
	}
}
