#pragma once

# include "../Type.h"

# include "../MyType/Str.h"
# include "../MyType/StrList.h"
# include "../MyType/JSON.h"

# include "../UtilKit/PBVarIntUtil.h"

namespace $TJ::$RTONUtil::$Struct {
	namespace $StructIdent {
		constexpr FourCC kBegin(makeFourCC('R', 'T', 'O', 'N'));
		constexpr FourCC kEnd(makeFourCC('D', 'O', 'N', 'E'));
		constexpr UI32 kUnKnown_1(0x00000001); // unknown block after 'RTON', maybe 'version' ?
	}
	namespace $TypeIdent {
		constexpr Byte kBoolFalse(0x00);
		constexpr Byte kBoolTrue(0x01);

		constexpr Byte kSInt8(0x08);
		constexpr Byte kSInt8_Zero(0x09);

		constexpr Byte kUInt8(0x0A);
		constexpr Byte kUInt8_Zero(0x0B);

		constexpr Byte kSInt16(0x10);
		constexpr Byte kSInt16_Zero(0x11);

		constexpr Byte kUInt16(0x12);
		constexpr Byte kUInt16_Zero(0x13);

		constexpr Byte kSInt32(0x20);
		constexpr Byte kSInt32_Zero(0x21);

		constexpr Byte kFloat32(0x22);
		constexpr Byte kFloat32_Zero(0x23);

		constexpr Byte kUVarInt32(0x24);
		constexpr Byte kSVarInt32(0x25);

		constexpr Byte kUInt32(0x26);
		constexpr Byte kUInt32_Zero(0x27);

		constexpr Byte kUVarInt32_S1(0x28);

		constexpr Byte kSInt64(0x40);
		constexpr Byte kSInt64_Zero(0x41);

		constexpr Byte kFloat64(0x42);
		constexpr Byte kFloat64_Zero(0x43);

		constexpr Byte kUVarInt64(0x44);
		constexpr Byte kSVarInt64(0x45);

		constexpr Byte kUInt64(0x46);
		constexpr Byte kUInt64_Zero(0x47);

		constexpr Byte kUVarInt64_S1(0x48);

		constexpr Byte kStr(0x81);
		constexpr Byte kStrIdxIng(0x90);
		constexpr Byte kStrIdx(0x91);

		constexpr Byte kWideStr(0x82);
		constexpr Byte kWideStrIdxIng(0x92);
		constexpr Byte kWideStrIdx(0x93);

		constexpr Byte kRTID(0x83);
		constexpr Byte kRTID_NullRef(0x84);

		constexpr Byte kObjBegin(0x85);
		constexpr Byte kObjEnd(0xFF);

		constexpr Byte kArrBegin(0x86);
		constexpr Byte kArrSize(0xFD);
		constexpr Byte kArrEnd(0xFE);

		constexpr Byte kBinaryCast(0x87);
	}
	namespace $RTIDTypeIdent {
		constexpr Byte kNullRef(0x0); // 0
		constexpr Byte kUIDRef(0x2); // u32.u32.h32@ls
		constexpr Byte kAliasRef(0x3); // ls@ls
	}

	constexpr SzI kRTIDStrLen_Shell(6); // RTID()
	constexpr SzI kRTIDStrLen_Fmt_u32u32h4ls_Max($MaxStrLenDef::kInt32Str + kSzOne + $MaxStrLenDef::kInt32Str + kSzOne + $MaxStrLenDef::kHex32Str); // RTID()
	Str const kRTIDStr_Base("RTID");
	Str const kRTIDStr_Full_Null("RTID(0)");
	Str const kRTIDStr_Content_Null("0");
	Str const kRTIDStr_Content_UID_Fmt("%u.%u.%08x");
	Str const kBinaryCast_Fmt("$BINARY(\"%s\", %d)");

	Str const kErrToastStr("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);

	inline auto isRTIDStr(Str const & s) {
		return s._len >= kRTIDStrLen_Shell && StrRef(s, kThFirst, 5).cmp("RTID(") && StrRef(s, s._len - 1, 1).cmp(")");
	}
	inline auto getRTIDType(Str const & s) {
		return ternary_oper(s == kRTIDStr_Full_Null, $RTIDTypeIdent::kNullRef,
							ternary_oper(s.cnt('.', 5, s.findFromEnd('@') - 5) == 2, $RTIDTypeIdent::kUIDRef, $RTIDTypeIdent::kAliasRef));
	}
}
