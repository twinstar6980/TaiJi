# include "./Hash.h"

# include "../../../lib/md5/md5.h"

# include <string>

namespace $TJ::$DataUtil::$Hash {
	namespace $MD5 {
		Str & make(Str const & src, Str & dst) {
			std::string s; s.assign(src._str, src._len);
			dst.cpy(MD5(s).toStr().c_str());
			return dst;
		}
	}
	namespace $FNV {
		static constexpr UI32 kPrime32(16777619);
		static constexpr UI32 kOffset32(2166136261);
		static constexpr UI64 kPrime64(1099511628211);
		static constexpr UI64 kOffset64(14695981039346656037);

		UI32 make32(Byte const * const & data, SzI const & sz) {
			auto val = kOffset32;
			for_cnt(i, kThFirst, sz) {
				val *= kPrime32;
				val ^= static_cast<UI8>(data[i]);
			}
			return val;
		}
		UI64 make64(Byte const * const & data, SzI const & sz) {
			auto val = kOffset64;
			for_cnt(i, kThFirst, sz) {
				val *= kPrime64;
				val ^= static_cast<UI8>(data[i]);
			}
			return val;
		}
	}
}
