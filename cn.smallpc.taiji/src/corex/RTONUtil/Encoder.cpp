# include "./Encoder.h"

# include "../Type.h"

# include "../MyType/Str.h"
# include "../MyType/StrList.h"
# include "../MyType/DataStream.h"

# include "../UtilKit/PBVarIntUtil.h"
# include "../UtilKit/JSONUtil.h"

namespace $TJ::$RTONUtil::$Encoder {
	namespace $EnCode {
		static Void enc($JSON::Arr const & src, Byte * const & dst, SzI & pos, Bool const & useCacheStr, StrList * const & cacheStrList, Bool const & useSpecialTypeForRTID);
		static Void enc($JSON::Obj const & src, Byte * const & dst, SzI & pos, Bool const & useCacheStr, StrList * const & cacheStrList, Bool const & useSpecialTypeForRTID);

		static Void enc($JSON::Val const & src, Byte * const & dst, SzI & pos, Bool const & useCacheStr, StrList * const & cacheStrList, Bool const & useSpecialTypeForRTID) {
			switch (src._type) {
				default: wErr("UnKnown JSON Type"); break;
				case $JSON::ValType::kNull: wErr("RTON Can't Save Null"); break;
				case $JSON::ValType::kObj:
					enc(src.getObj(), dst, pos, useCacheStr, cacheStrList, useSpecialTypeForRTID);
					break;
				case $JSON::ValType::kArr:
					enc(src.getArr(), dst, pos, useCacheStr, cacheStrList, useSpecialTypeForRTID);
					break;
				case $JSON::ValType::kBool:
					dst[pos++] = ternary_oper(src.getBool(), $Struct::$TypeIdent::kBoolTrue, $Struct::$TypeIdent::kBoolFalse);
					break;
				case $JSON::ValType::kUInt:
					dst[pos++] = $Struct::$TypeIdent::kUVarInt64;
					pos += $PBVarIntUtil::enc64(src.getUInt(), dst + pos);
					break;
				case $JSON::ValType::kSInt:
					dst[pos++] = $Struct::$TypeIdent::kSVarInt64;
					pos += $PBVarIntUtil::enc64(src.getSInt(), dst + pos);
					break;
				case $JSON::ValType::kFloat:
					dst[pos++] = $Struct::$TypeIdent::kFloat64;
					memCpy(dst + pos, &src.getFloat(), szOf<F64>());
					pos += szOf<F64>();
					break;
				case $JSON::ValType::kStr: {
					auto & str = src.getStr();
					if (useSpecialTypeForRTID && $Struct::isRTIDStr(str)) {
						dst[pos++] = $Struct::$TypeIdent::kRTID;
						dst[pos] = $Struct::getRTIDType(str);
						switch (dst[pos++]) {
							default: break;
							case $Struct::$RTIDTypeIdent::kNullRef: break;
							case $Struct::$RTIDTypeIdent::kUIDRef: {
								StrRef content(str, 5, str._len - 6);
								auto splitPos = content.findFromEnd('@');
								if (splitPos == content._len) wErr("NOT A RTID STR");
								StrRef sheet(content, splitPos + kThSecond, content._len - splitPos - kThSecond);
								StrRef uid(content, kThFirst, splitPos);
								UI32 uidFirst, uidMiddle, uidLast;
								uid.scanFmt($Struct::kRTIDStr_Content_UID_Fmt._str, &uidFirst, &uidMiddle, &uidLast);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(sheet._len), dst + pos);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(sheet._len), dst + pos);
								memCpy(dst + pos, sheet._str, szOf<Ch>(sheet._len));
								pos += szOf<Ch>(sheet._len);
								pos += $PBVarIntUtil::enc32(uidMiddle, dst + pos);
								pos += $PBVarIntUtil::enc32(uidFirst, dst + pos);
								*reinterpret_cast<UI32 *>(dst + pos) = uidLast;
								pos += szOf<UI32>();
								break;
							}
							case $Struct::$RTIDTypeIdent::kAliasRef: {
								StrRef content(str, 5, str._len - 6);
								auto splitPos = content.findFromEnd('@');
								if (splitPos == content._len) wErr("NOT A RTID STR");
								StrRef sheet(content, splitPos + kThSecond, content._len - splitPos - kThSecond);
								StrRef alias(content, kThFirst, splitPos);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(sheet._len), dst + pos);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(sheet._len), dst + pos);
								memCpy(dst + pos, sheet._str, szOf<Ch>(sheet._len));
								pos += szOf<Ch>(sheet._len);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(alias._len), dst + pos);
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(alias._len), dst + pos);
								memCpy(dst + pos, alias._str, szOf<Ch>(alias._len));
								pos += szOf<Ch>(alias._len);
								break;
							}
						}
					} else {
						if (useCacheStr) {
							SzI strIdx;
							if ((*cacheStrList).find(str, strIdx)) {
								dst[pos++] = $Struct::$TypeIdent::kStrIdx;
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(strIdx), dst + pos);
							} else {
								(*cacheStrList).add(str);
								dst[pos++] = $Struct::$TypeIdent::kStrIdxIng;
								pos += $PBVarIntUtil::enc32(static_cast<UI32>(str._len), dst + pos);
								memCpy(dst + pos, str._str, szOf<Ch>(str._len));
								pos += szOf<Ch>(str._len);
							}
						} else {
							dst[pos++] = $Struct::$TypeIdent::kStr;
							pos += $PBVarIntUtil::enc32(static_cast<UI32>(str._len), dst + pos);
							memCpy(dst + pos, str._str, szOf<Ch>(str._len));
							pos += szOf<Ch>(str._len);
						}
					}
					break;
				}
			}
			return;
		}
		static Void enc($JSON::Arr const & src, Byte * const & dst, SzI & pos, Bool const & useCacheStr, StrList * const & cacheStrList, Bool const & useSpecialTypeForRTID) {
			dst[pos++] = $Struct::$TypeIdent::kArrBegin;
			dst[pos++] = $Struct::$TypeIdent::kArrSize;
			pos += $PBVarIntUtil::enc32(static_cast<UI32>(src._sz), dst + pos);
			for_criter(e, src)
				enc(e, dst, pos, useCacheStr, cacheStrList, useSpecialTypeForRTID);
			dst[pos++] = $Struct::$TypeIdent::kArrEnd;
			return;
		}
		static Void enc($JSON::Obj const & src, Byte * const & dst, SzI & pos, Bool const & useCacheStr, StrList * const & cacheStrList, Bool const & useSpecialTypeForRTID) {
			dst[pos++] = $Struct::$TypeIdent::kObjBegin;
			for_criter(e, src) {
				enc(e._key, dst, pos, useCacheStr, cacheStrList, useSpecialTypeForRTID);
				enc(e._val, dst, pos, useCacheStr, cacheStrList, useSpecialTypeForRTID);
			}
			dst[pos++] = $Struct::$TypeIdent::kObjEnd;
			return;
		}
		Void encFull($JSON::Obj const & src, Byte * const & dst, SzI & sz, Bool const & useCacheStr, Bool const & useSpecialTypeForRTID) {
			StrList * cacheStrList(kNullPtr);
			if (useCacheStr) {
				cacheStrList = new StrList($JSON::cntStrSum(src));
			}
			{
				sz = kSzZero;
				*reinterpret_cast<FourCC *>(dst + sz) = $Struct::$StructIdent::kBegin;
				sz += szOf<FourCC>();
				sz += szOf<UI32>();
				enc(src, dst, --sz, useCacheStr, cacheStrList, useSpecialTypeForRTID);
				*reinterpret_cast<UI32 *>(dst + szOf<FourCC>()) = $Struct::$StructIdent::kUnKnown_1;
				*reinterpret_cast<FourCC *>(dst + sz) = $Struct::$StructIdent::kEnd;
				sz += szOf<FourCC>();
			}
			{
				if (useCacheStr) delPtrSingle(cacheStrList);
			}
			return;
		}
	}
	namespace $DeCode {
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Arr & dst, StrList & cacheStrList, StrList & cacheWideStrList);
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Obj & dst, StrList & cacheStrList, StrList & cacheWideStrList);

		static Void dec(Byte const * const & src, SzI & pos, $JSON::Val & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			switch (src[pos++]) {
				default: wErr("UnKnown val Type 0x%X at 0x%X", src[--pos], pos); break;
				case $Struct::$TypeIdent::kBoolFalse: dst.set(kFalse); break;
				case $Struct::$TypeIdent::kBoolTrue: dst.set(kTrue); break;
				case $Struct::$TypeIdent::kSInt8:
					dst.set(*reinterpret_cast<SI8 const *>(src + pos));
					pos += szOf<SI8>();
					break;
				case $Struct::$TypeIdent::kSInt8_Zero: dst.set(static_cast<SI8>(0)); break;
				case $Struct::$TypeIdent::kUInt8:
					dst.set(*reinterpret_cast<UI8 const *>(src + pos));
					pos += szOf<UI8>();
					break;
				case $Struct::$TypeIdent::kUInt8_Zero: dst.set(static_cast<UI8>(0)); break;
				case $Struct::$TypeIdent::kSInt16:
					dst.set(*reinterpret_cast<SI16 const *>(src + pos));
					pos += szOf<SI16>();
					break;
				case $Struct::$TypeIdent::kSInt16_Zero: dst.set(static_cast<SI16>(0)); break;
				case $Struct::$TypeIdent::kUInt16:
					dst.set(*reinterpret_cast<UI16 const *>(src + pos));
					pos += szOf<UI16>();
					break;
				case $Struct::$TypeIdent::kUInt16_Zero: dst.set(static_cast<UI16>(0)); break;
				case $Struct::$TypeIdent::kSInt32:
					dst.set(*reinterpret_cast<SI32 const *>(src + pos));
					pos += szOf<SI32>();
					break;
				case $Struct::$TypeIdent::kSInt32_Zero: dst.set(static_cast<SI32>(0)); break;
				case $Struct::$TypeIdent::kFloat32:
				# if defined ABI_ARM
				{
					F32 fv; memcpy(&fv, src + pos, szOf<F32>());
					dst.set(fv);
				}
			# else
					dst.set(*reinterpret_cast<F32 const *>(src + pos));
				# endif
					pos += szOf<F32>();
					break;
				case $Struct::$TypeIdent::kFloat32_Zero: dst.set(static_cast<F32>(0.0f)); break;
				case $Struct::$TypeIdent::kUVarInt32:
					dst.set($PBVarIntUtil::dec32(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kSVarInt32:
					dst.set($PBVarIntUtil::toSI32($PBVarIntUtil::dec32(src + pos)));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kUInt32:
					dst.set(*reinterpret_cast<UI32 const *>(src + pos));
					pos += szOf<UI32>();
					break;
				case $Struct::$TypeIdent::kUInt32_Zero: dst.set(static_cast<UI32>(0)); break;
				case $Struct::$TypeIdent::kUVarInt32_S1:
					dst.set($PBVarIntUtil::dec32(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kSInt64:
					dst.set(*reinterpret_cast<SI64 const *>(src + pos));
					pos += szOf<SI64>();
					break;
				case $Struct::$TypeIdent::kSInt64_Zero: dst.set(static_cast<SI64>(0)); break;
				case $Struct::$TypeIdent::kFloat64:
				# if defined ABI_ARM
				{
					F64 fv; memcpy(&fv, src + pos, szOf<F64>());
					dst.set(fv);
				}
			# else
					dst.set(*reinterpret_cast<F64 const *>(src + pos));
				# endif
					pos += szOf<F64>();
					break;
				case $Struct::$TypeIdent::kFloat64_Zero: dst.set(static_cast<F64>(0.0)); break;
				case $Struct::$TypeIdent::kUVarInt64:
					dst.set($PBVarIntUtil::dec64(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kSVarInt64:
					dst.set($PBVarIntUtil::toSI64($PBVarIntUtil::dec64(src + pos)));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kUInt64:
					dst.set(*reinterpret_cast<UI64 const *>(src + pos));
					pos += szOf<UI64>();
					break;
				case $Struct::$TypeIdent::kUInt64_Zero: dst.set(static_cast<UI64>(0)); break;
				case $Struct::$TypeIdent::kUVarInt64_S1:
					dst.set($PBVarIntUtil::dec64(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kStr: {
					UI32 strLen;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kStrIdxIng: {
					UI32 strLen;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					cacheStrList.add(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kStrIdx: {
					UI32 strIdx;
					pos += $PBVarIntUtil::dec32(src + pos, strIdx);
					dst.set(cacheStrList[strIdx]);
					break;
				}
				case $Struct::$TypeIdent::kWideStr: {
					UI32 wideStrLen, strLen;
					pos += $PBVarIntUtil::dec32(src + pos, wideStrLen);
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kWideStrIdxIng: {
					UI32 wideStrLen, strLen;
					pos += $PBVarIntUtil::dec32(src + pos, wideStrLen);
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					cacheWideStrList.add(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kWideStrIdx: {
					UI32 strIdx;
					pos += $PBVarIntUtil::dec32(src + pos, strIdx);
					dst.set(cacheWideStrList[strIdx]);
					break;
				}
				case $Struct::$TypeIdent::kRTID: {
					Str rtidStr;
					switch (src[pos++]) {
						default: wErr("UnKnown RTID Type"); break;
						case $Struct::$RTIDTypeIdent::kNullRef: rtidStr.cpy($Struct::kRTIDStr_Full_Null); break;
						case $Struct::$RTIDTypeIdent::kUIDRef: {
							SzI sheetStrPos;
							UI32 sheetWideStrLen, sheetStrLen;
							UI32 uidFirst, uidMiddle, uidLast; // warning: unknown integer type , define it 'u32'
							{
								pos += $PBVarIntUtil::dec32(src + pos, sheetWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, sheetStrLen);
								sheetStrPos = pos;
								pos += sheetStrLen;
								pos += $PBVarIntUtil::dec32(src + pos, uidMiddle);
								pos += $PBVarIntUtil::dec32(src + pos, uidFirst);
								uidLast = *reinterpret_cast<UI32 const *>(src + pos);
								pos += szOf<UI32>();
							}
							rtidStr.alloc($Struct::kRTIDStrLen_Shell + kSzOne + sheetStrLen + $MaxStrLenDef::kInt32Str + $MaxStrLenDef::kInt32Str + $MaxStrLenDef::kHex32Str)
								.cat($Struct::kRTIDStr_Base)
								.cat($Char::kOpenParenthesis)
								.printFmtByAppend($Struct::kRTIDStr_Content_UID_Fmt._str, uidFirst, uidMiddle, uidLast)
								.cat($Char::kAt)
								.cat(reinterpret_cast<Ch const *>(src + sheetStrPos), sheetStrLen)
								.cat($Char::kCloseParenthesis);
							break;
						}
						case $Struct::$RTIDTypeIdent::kAliasRef: {
							SzI sheetStrPos, aliasStrPos;
							UI32 sheetWideStrLen, sheetStrLen, aliasWideStrLen, aliasStrLen;
							{
								pos += $PBVarIntUtil::dec32(src + pos, sheetWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, sheetStrLen);
								sheetStrPos = pos;
								pos += sheetStrLen;
								pos += $PBVarIntUtil::dec32(src + pos, aliasWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, aliasStrLen);
								aliasStrPos = pos;
								pos += aliasStrLen;
							}
							rtidStr.alloc($Struct::kRTIDStrLen_Shell + kSzOne + sheetStrLen + aliasStrLen)
								.cat($Struct::kRTIDStr_Base)
								.cat($Char::kOpenParenthesis)
								.cat(reinterpret_cast<Ch const *>(src + aliasStrPos), aliasStrLen)
								.cat($Char::kAt)
								.cat(reinterpret_cast<Ch const *>(src + sheetStrPos), sheetStrLen)
								.cat($Char::kCloseParenthesis);
							break;
						}
					}
					dst.set(rtidStr);
					rtidStr.tfree();
					break;
				}
				case $Struct::$TypeIdent::kRTID_NullRef: dst.set($Struct::kRTIDStr_Full_Null); break;
				case $Struct::$TypeIdent::kBinaryCast: {
					// TODO
					StrRef str;
					UI32 strLen;
					UI32 intVal;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					str.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					pos += $PBVarIntUtil::dec32(src + pos, intVal);
					dst.set(Str().printFmtByAppend($Struct::kBinaryCast_Fmt._str, str._str, intVal));
					str.clear();
					break;
				}
				case $Struct::$TypeIdent::kObjBegin: dec(src, pos, dst.setObj().getObj(), cacheStrList, cacheWideStrList); break;
				case $Struct::$TypeIdent::kArrBegin: dec(src, pos, dst.setArr().getArr(), cacheStrList, cacheWideStrList); break;
			}
			return;
		}
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Arr & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			if (src[pos++] != $Struct::$TypeIdent::kArrSize) wErr("Can Not get Array sz");
			UI32 arrSz;
			pos += $PBVarIntUtil::dec32(src + pos, arrSz);
			dst.alloc(arrSz);
			while_nonstop{
				if (src[pos] == $Struct::$TypeIdent::kArrEnd) {
					++pos;
					break;
				}
				dec(src, pos, dst.add().getLast(), cacheStrList, cacheWideStrList);
			}
			if (dst._sz != arrSz) wWarn("Actual ArrSize Is Diff Of ArrSize");
			return;
		}
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Obj & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			dst.setNull();
			while_nonstop{
				if (src[pos] == $Struct::$TypeIdent::kObjEnd) {
					pos++;
					break;
				}
				dst.add();
				dec(src, pos, dst.getLast()._key, cacheStrList, cacheWideStrList);
				dec(src, pos, dst.getLast()._val, cacheStrList, cacheWideStrList);
				dst.getLast()._key.review();
			}
			return;
		}
		Void decFull(Byte const * const & src, SzI const & sz, $JSON::Obj & dst) {
			StrList cacheStrList, cacheWideStrList;
			{
				auto strSumLimit = kSzZero, wideStrSumLimit = kSzZero;
				for_cnt(pos, kThFirst, sz)
					switch (src[pos]) {
						default: break;
						case $Struct::$TypeIdent::kStrIdxIng: ++strSumLimit; break;
						case $Struct::$TypeIdent::kWideStrIdxIng: ++wideStrSumLimit; break;
					}
				cacheStrList.alloc(strSumLimit); cacheWideStrList.alloc(wideStrSumLimit);
			}
			{
				auto pos = kThFirst;
				if (*reinterpret_cast<FourCC const *>(src + pos) != $Struct::$StructIdent::kBegin)
					wErr("RTON Begin Is Error");
				pos += szOf<FourCC>();
				if (*reinterpret_cast<UI32 const *>(src + pos) != $Struct::$StructIdent::kUnKnown_1)
					wErr("RTON Ver Is Error");
				pos += szOf<UI32>();
				dec(src, pos, dst, cacheStrList, cacheWideStrList);
				if (*reinterpret_cast<FourCC const *>(src + pos) != $Struct::$StructIdent::kEnd)
					wErr("RTON End Is Error");
				pos += szOf<FourCC>();
			}
			cacheStrList.tfree(); cacheWideStrList.tfree();
			return;
		}
	}/*
	namespace $DeCodex {

		static Void dec(DataStm & src, $JSON::Val & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			switch (src.get<Byte>()) {
				default: wErr("UnKnown val Type 0x%X at 0x%X", src.backPos(kSzOne).get<Byte>(), src._pos); break;
				case $Struct::$TypeIdent::kBoolFalse: dst.set(kFalse); break;
				case $Struct::$TypeIdent::kBoolTrue: dst.set(kTrue); break;
				case $Struct::$TypeIdent::kSInt8: dst.set(src.get<SI8>()); break;
				case $Struct::$TypeIdent::kSInt8_Zero: dst.set(static_cast<SI8>(0)); break;
				case $Struct::$TypeIdent::kUInt8: dst.set(src.get<UI8>()); break;
				case $Struct::$TypeIdent::kUInt8_Zero: dst.set(static_cast<UI8>(0)); break;
				case $Struct::$TypeIdent::kSInt16: dst.set(src.get<SI16>()); break;
				case $Struct::$TypeIdent::kSInt16_Zero: dst.set(static_cast<SI16>(0)); break;
				case $Struct::$TypeIdent::kUInt16: dst.set(src.get<UI16>()); break;
				case $Struct::$TypeIdent::kUInt16_Zero: dst.set(static_cast<UI16>(0)); break;
				case $Struct::$TypeIdent::kSInt32: dst.set(src.get<SI32>()); break;
				case $Struct::$TypeIdent::kSInt32_Zero: dst.set(static_cast<SI32>(0)); break;
				case $Struct::$TypeIdent::kUInt32: dst.set(src.get<UI32>()); break;
				case $Struct::$TypeIdent::kUInt32_Zero: dst.set(static_cast<UI32>(0)); break;
				case $Struct::$TypeIdent::kSInt64: dst.set(src.get<SI64>()); break;
				case $Struct::$TypeIdent::kSInt64_Zero: dst.set(static_cast<SI64>(0)); break;
				case $Struct::$TypeIdent::kUInt64: dst.set(src.get<UI64>()); break;
				case $Struct::$TypeIdent::kUInt64_Zero: dst.set(static_cast<UI64>(0)); break;
				case $Struct::$TypeIdent::kFloat32: dst.set(src.get<F32>()); break;
				case $Struct::$TypeIdent::kFloat32_Zero: dst.set(static_cast<F32>(0.0f)); break;
				case $Struct::$TypeIdent::kFloat64: dst.set(src.get<F64>()); break;
				case $Struct::$TypeIdent::kFloat64_Zero: dst.set(static_cast<F64>(0.0)); break;
				case $Struct::$TypeIdent::kUVarInt32:
					dst.set($PBVarIntUtil::dec32(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kSVarInt32:
					dst.set($PBVarIntUtil::toSI32($PBVarIntUtil::dec32(src + pos)));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kUVarInt32_S1:
					dst.set($PBVarIntUtil::dec32(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kUVarInt64:
					dst.set($PBVarIntUtil::dec64(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kSVarInt64:
					dst.set($PBVarIntUtil::toSI64($PBVarIntUtil::dec64(src + pos)));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kUVarInt64_S1:
					dst.set($PBVarIntUtil::dec64(src + pos));
					pos += $PBVarIntUtil::calcSz(src + pos);
					break;
				case $Struct::$TypeIdent::kStr: {
					UI32 strLen;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kStrIdxIng: {
					UI32 strLen;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					cacheStrList.add(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kStrIdx: {
					UI32 strIdx;
					pos += $PBVarIntUtil::dec32(src + pos, strIdx);
					dst.set(cacheStrList[strIdx]);
					break;
				}
				case $Struct::$TypeIdent::kWideStr: {
					UI32 wideStrLen, strLen;
					pos += $PBVarIntUtil::dec32(src + pos, wideStrLen);
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kWideStrIdxIng: {
					UI32 wideStrLen, strLen;
					pos += $PBVarIntUtil::dec32(src + pos, wideStrLen);
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					dst.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					cacheWideStrList.add(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					break;
				}
				case $Struct::$TypeIdent::kWideStrIdx: {
					UI32 strIdx;
					pos += $PBVarIntUtil::dec32(src + pos, strIdx);
					dst.set(cacheWideStrList[strIdx]);
					break;
				}
				case $Struct::$TypeIdent::kRTID: {
					Str rtidStr;
					switch (src[pos++]) {
						default: wErr("UnKnown RTID Type"); break;
						case $Struct::$RTIDTypeIdent::kNullRef: rtidStr.cpy($Struct::kRTIDStr_Full_Null); break;
						case $Struct::$RTIDTypeIdent::kUIDRef: {
							SzI sheetStrPos;
							UI32 sheetWideStrLen, sheetStrLen;
							UI32 uidFirst, uidMiddle, uidLast; // warning: unknown integer type , define it 'u32'
							{
								pos += $PBVarIntUtil::dec32(src + pos, sheetWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, sheetStrLen);
								sheetStrPos = pos;
								pos += sheetStrLen;
								pos += $PBVarIntUtil::dec32(src + pos, uidMiddle);
								pos += $PBVarIntUtil::dec32(src + pos, uidFirst);
								uidLast = *reinterpret_cast<UI32 const *>(src + pos);
								pos += szOf<UI32>();
							}
							rtidStr.alloc($Struct::kRTIDStrLen_Shell + kSzOne + sheetStrLen + $MaxStrLenDef::kInt32Str + $MaxStrLenDef::kInt32Str + $MaxStrLenDef::kHex32Str)
								.cat($Struct::kRTIDStr_Base)
								.cat($Char::kOpenParenthesis)
								.printFmtByAppend($Struct::kRTIDStr_Content_UID_Fmt._str, uidFirst, uidMiddle, uidLast)
								.cat($Char::kAt)
								.cat(reinterpret_cast<Ch const *>(src + sheetStrPos), sheetStrLen)
								.cat($Char::kCloseParenthesis);
							break;
						}
						case $Struct::$RTIDTypeIdent::kAliasRef: {
							SzI sheetStrPos, aliasStrPos;
							UI32 sheetWideStrLen, sheetStrLen, aliasWideStrLen, aliasStrLen;
							{
								pos += $PBVarIntUtil::dec32(src + pos, sheetWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, sheetStrLen);
								sheetStrPos = pos;
								pos += sheetStrLen;
								pos += $PBVarIntUtil::dec32(src + pos, aliasWideStrLen);
								pos += $PBVarIntUtil::dec32(src + pos, aliasStrLen);
								aliasStrPos = pos;
								pos += aliasStrLen;
							}
							rtidStr.alloc($Struct::kRTIDStrLen_Shell + kSzOne + sheetStrLen + aliasStrLen)
								.cat($Struct::kRTIDStr_Base)
								.cat($Char::kOpenParenthesis)
								.cat(reinterpret_cast<Ch const *>(src + aliasStrPos), aliasStrLen)
								.cat($Char::kAt)
								.cat(reinterpret_cast<Ch const *>(src + sheetStrPos), sheetStrLen)
								.cat($Char::kCloseParenthesis);
							break;
						}
					}
					dst.set(rtidStr);
					rtidStr.tfree();
					break;
				}
				case $Struct::$TypeIdent::kRTID_NullRef: dst.set($Struct::kRTIDStr_Full_Null); break;
				case $Struct::$TypeIdent::kBinaryCast: {
					// TODO
					StrRef str;
					UI32 strLen;
					UI32 intVal;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					str.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					pos += $PBVarIntUtil::dec32(src + pos, intVal);
					dst.set(Str().printFmtByAppend($Struct::kBinaryCast_Fmt._str, str._str, intVal));
					str.clear();
					break;
				}
				case $Struct::$TypeIdent::kObjBegin: dec(src, pos, dst.setObj().getObj(), cacheStrList, cacheWideStrList); break;
				case $Struct::$TypeIdent::kArrBegin: dec(src, pos, dst.setArr().getArr(), cacheStrList, cacheWideStrList); break;
			}
			return;
		}
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Arr & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			if (src[pos++] != $Struct::$TypeIdent::kArrSize) wErr("Can Not get Array sz");
			UI32 arrSz;
			pos += $PBVarIntUtil::dec32(src + pos, arrSz);
			dst.alloc(arrSz);
			while_nonstop{
				if (src[pos] == $Struct::$TypeIdent::kArrEnd) {
					++pos;
					break;
				}
				dec(src, pos, dst.add().getLast(), cacheStrList, cacheWideStrList);
			}
			if (dst._sz != arrSz) wWarn("Actual ArrSize Is Diff Of ArrSize");
			return;
		}
		static Void dec(Byte const * const & src, SzI & pos, $JSON::Obj & dst, StrList & cacheStrList, StrList & cacheWideStrList) {
			dst.setNull();
			while_nonstop{
				if (src[pos] == $Struct::$TypeIdent::kObjEnd) {
					pos++;
					break;
				}
				dst.add();
				dec(src, pos, dst.getLast()._key, cacheStrList, cacheWideStrList);
				dec(src, pos, dst.getLast()._val, cacheStrList, cacheWideStrList);
				dst.getLast()._key.review();
			}
			return;
		}
		Void decFull(Byte const * const & src, SzI const & sz, $JSON::Obj & dst) {
			StrList cacheStrList, cacheWideStrList;
			{
				auto strSumLimit = kSzZero, wideStrSumLimit = kSzZero;
				for_cnt(pos, kThFirst, sz)
					switch (src[pos]) {
						default: break;
						case $Struct::$TypeIdent::kStrIdxIng: ++strSumLimit; break;
						case $Struct::$TypeIdent::kWideStrIdxIng: ++wideStrSumLimit; break;
					}
				cacheStrList.alloc(strSumLimit); cacheWideStrList.alloc(wideStrSumLimit);
			}
			{
				auto pos = kThFirst;
				if (*reinterpret_cast<FourCC const *>(src + pos) != $Struct::$StructIdent::kBegin)
					wErr("RTON Begin Is Error");
				pos += szOf<FourCC>();
				if (*reinterpret_cast<UI32 const *>(src + pos) != $Struct::$StructIdent::kUnKnown_1)
					wErr("RTON Ver Is Error");
				pos += szOf<UI32>();
				dec(src, pos, dst, cacheStrList, cacheWideStrList);
				if (*reinterpret_cast<FourCC const *>(src + pos) != $Struct::$StructIdent::kEnd)
					wErr("RTON End Is Error");
				pos += szOf<FourCC>();
				if (dst._sz > kSzZero && dst[kThFirst]._key.get() == $Struct::kKeyStrOfAllStr) dst.erase(kThFirst);
			}
			cacheStrList.tfree(); cacheWideStrList.tfree();
			return;
		}
	}*/
}
