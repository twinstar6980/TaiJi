#include "./PBVarInt.hpp"

#include "../../Const.hpp"

namespace $TJ::$Util::$PBVarInt {

	constexpr Byte kB("10000000"_bin8);
	constexpr Byte kX("01111111"_bin8);

	Void encU32(IU32 src, ByteStream & dst) {
		while (src >= kB) {
			dst << static_cast<Byte>((src & kX) | kB);
			src >>= 7;
		}
		dst << static_cast<Byte>(src);
		return;
	}
	Void encU64(IU64 src, ByteStream & dst) {
		while (src >= kB) {
			dst << static_cast<Byte>((src & kX) | kB);
			src >>= 7;
		}
		dst << static_cast<Byte>(src);
		return;
	}

	IU32 decU32(ByteStream & src) {
		auto dst = IU32(0);
		Byte cur;
		for (auto shift = 0u; shift <= 31u; shift += 7u) {
			src >> cur;
			if (cur & kB) {
				dst |= IU32(cur & kX) << shift;
			} else {
				dst |= IU32(cur) << shift;
				break;
			}
		}
		return dst;
	}
	IU64 decU64(ByteStream & src) {
		auto dst = IU64(0);
		Byte cur;
		for (auto shift = 0u; shift <= 63u; shift += 7u) {
			src >> cur;
			if (cur & kB) {
				dst |= IU64(cur & kX) << shift;
			} else {
				dst |= IU64(cur) << shift;
				break;
			}
		}
		return dst;
	}

}
