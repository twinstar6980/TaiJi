#include "./MD5.hpp"

#include "../../../../dependency/md5/md5.h"

namespace $TJ::$Util::$Data::$Hasher::$MD5 {

	Str & make(Str const & src, Str & dst) {
		auto r = MD5(std::string(src.$data, src.$sz)).toStr();
		dst.cpy(r.c_str(), r.length());
		return dst;
	}

}
