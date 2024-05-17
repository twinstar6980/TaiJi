#pragma once

#include "../../Type.hpp"
#include "../../UserLiteral.hpp"
#include "../../Type/ByteStream.hpp"

namespace $TJ::$Util::$PBVarInt {

	namespace $ZigZag {

		inline IU32 enc32(IS32 const & t) { return t << 1 ^ t >> 31; }
		inline IU64 enc64(IS64 const & t) { return t << 1 ^ t >> 63; }
		inline IS32 dec32(IU32 const & t) { return t >> 1 ^ -(t & 1); }
		inline IS64 dec64(IU64 const & t) { return t >> 1 ^ -(t & 1); }

	}

	inline auto toU32(IS32 const & t) { return $ZigZag::enc32(t); }
	inline auto toU64(IS64 const & t) { return $ZigZag::enc64(t); }
	inline auto toS32(IU32 const & t) { return $ZigZag::dec32(t); }
	inline auto toS64(IU64 const & t) { return $ZigZag::dec64(t); }

	Void encU32(IU32 src, ByteStream & dst);
	Void encU64(IU64 src, ByteStream & dst);

	IU32 decU32(ByteStream & src);
	IU64 decU64(ByteStream & src);

	inline auto encS32(IS32 src, ByteStream & dst) { return encU32(toU32(src), dst); }
	inline auto encS64(IS64 src, ByteStream & dst) { return encU64(toU64(src), dst); }

	inline auto decS32(ByteStream & src) { return toS32(decU32(src)); }
	inline auto decS64(ByteStream & src) { return toS64(decU64(src)); }

}
