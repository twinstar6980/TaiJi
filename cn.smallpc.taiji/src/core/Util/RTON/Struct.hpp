#pragma once

#include "../../Type.hpp"
#include "../../UserLiteral.hpp"
#include "../../Type/Str.hpp"

namespace $TJ::$Util::$RTON::$Struct {

	namespace $StructIdent {

		inline constexpr FourCC kBeg(makeFourCC('R', 'T', 'O', 'N'));
		inline constexpr FourCC kEnd(makeFourCC('D', 'O', 'N', 'E'));
		inline constexpr IU32 kUnKnown1(0x00000001); // unknown block after 'RTON', maybe 'version' ?

	}

	namespace $TypeIdent {

		inline constexpr Byte kBoolFalse(0x00);
		inline constexpr Byte kBoolTrue(0x01);

		inline constexpr Byte kIntS8(0x08);
		inline constexpr Byte kIntS8Zero(0x09);

		inline constexpr Byte kIntU8(0x0A);
		inline constexpr Byte kIntU8Zero(0x0B);

		inline constexpr Byte kIntS16(0x10);
		inline constexpr Byte kIntS16Zero(0x11);

		inline constexpr Byte kIntU16(0x12);
		inline constexpr Byte kIntU16Zero(0x13);

		inline constexpr Byte kIntS32(0x20);
		inline constexpr Byte kIntS32Zero(0x21);

		inline constexpr Byte kFloat32(0x22);
		inline constexpr Byte kFloat32Zero(0x23);

		inline constexpr Byte kVarIntU32(0x24);
		inline constexpr Byte kVarIntS32(0x25);

		inline constexpr Byte kIntU32(0x26);
		inline constexpr Byte kIntU32Zero(0x27);

		inline constexpr Byte kVarIntU32Equal(0x28);

		inline constexpr Byte kIntS64(0x40);
		inline constexpr Byte kIntS64Zero(0x41);

		inline constexpr Byte kFloat64(0x42);
		inline constexpr Byte kFloat64Zero(0x43);

		inline constexpr Byte kVarIntU64(0x44);
		inline constexpr Byte kVarIntS64(0x45);

		inline constexpr Byte kIntU64(0x46);
		inline constexpr Byte kIntU64Zero(0x47);

		inline constexpr Byte kVarIntU64Equal(0x48);

		inline constexpr Byte kStr(0x81);
		inline constexpr Byte kStrIdxIng(0x90);
		inline constexpr Byte kStrIdx(0x91);

		inline constexpr Byte kUTF8Str(0x82);
		inline constexpr Byte kUTF8StrIdxIng(0x92);
		inline constexpr Byte kUTF8StrIdx(0x93);

		inline constexpr Byte kRTID(0x83);
		inline constexpr Byte kRTIDOfNullRef(0x84);

		inline constexpr Byte kArrBeg(0x86);
		inline constexpr Byte kArrSz(0xFD);
		inline constexpr Byte kArrEnd(0xFE);

		inline constexpr Byte kObjBeg(0x85);
		inline constexpr Byte kObjEnd(0xFF);

		inline constexpr Byte kBinaryCast(0x87);

	}

	inline Str const kBinaryCastFmt("$BINARY(\"%s\", %d)"_r);

	namespace $RTID {

		namespace $TypeIdent {

			inline constexpr Byte kNullRef(0x0); // 0
			inline constexpr Byte kUIDRef(0x2); // dec32u.dec32u.hex32u@ls
			inline constexpr Byte kAliasRef(0x3); // ls@ls

		}

		inline Str const kNullRTID("RTID(0)"_r);
		inline Str const kUIDFmt("%u.%u.%08x"_r);

		inline auto isRTIDStr(Str const & s) {
			return s.$sz > "RTID()"_len
				&& Str().ref(s, kNo1, "RTID("_len).cmp("RTID("_r)
				&& Str().ref(s, s.$sz - ")"_len, ")"_len).cmp(")"_r)
				&& Str().ref(s, "RTID("_len, s.$sz - "RTID()"_len).range().cnt('@') == kSz1;
		}

		inline auto getRTIDType(Str const & s) {
			return s == kNullRTID ? $TypeIdent::kNullRef
				: (s.range("RTID("_len, s.range().find('@') - "RTID("_len).cnt('.') == kSz2
				   ? $TypeIdent::kUIDRef : $TypeIdent::kAliasRef);
		}

	}

}

namespace $TJ::$Util::$RTON {

	using namespace $Struct;

}
