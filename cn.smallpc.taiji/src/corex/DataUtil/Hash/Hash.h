#pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"

namespace $TJ::$DataUtil::$Hash {
	namespace $MD5 {
		Str & make(Str const & src, Str & dst);
	}
	namespace $FNV {
		UI32 make32(Byte const * const & data, SzI const & sz);
		UI64 make64(Byte const * const & data, SzI const & sz);

		inline auto make32(Str const & data) { return make32(reinterpret_cast<Byte const *>(data._str), data._len); }
		inline auto make64(Str const & data) { return make64(reinterpret_cast<Byte const *>(data._str), data._len); }
	}
}
