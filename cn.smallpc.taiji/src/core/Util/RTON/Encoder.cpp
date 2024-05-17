#include "./Encoder.hpp"

#include "./Struct.hpp"

#include "../../Type.hpp"
#include "../../Type/Str.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/ByteStream.hpp"

#include "../PBVarInt/PBVarInt.hpp"
#include "../File/File.hpp"

namespace $TJ::$Util::$RTON::$Encoder {

	namespace $EnCode {

		extern Void encUnit($JSON::Val const & src, ByteStream & dst, List<Str> & idxStr, Bool const & enableRTIDType) {
			switch (src.$$ty()) {
				default:
					throw VAMGException("RTONEncoder", "RTON can't store unknown JSON-Value");
					break;
				case $JSON::ValType::kUndef:
					throw VAMGException("RTONEncoder", "RTON can't store undefined JSON-Value");
					break;
				case $JSON::ValType::kNull:
					throw VAMGException("RTONEncoder", "RTON can't store null JSON-Value");
					break;
				case $JSON::ValType::kBool:
					dst << (src.getBool() ? $TypeIdent::kBoolTrue : $TypeIdent::kBoolFalse);
					break;
				case $JSON::ValType::kIntU:
					dst << $TypeIdent::kVarIntU64;
					$PBVarInt::encU64(src.getIntU(), dst);
					break;
				case $JSON::ValType::kIntS:
					dst << $TypeIdent::kVarIntS64;
					$PBVarInt::encS64(src.getIntS(), dst);
					break;
				case $JSON::ValType::kFloat:
					dst << $TypeIdent::kFloat64 << src.getFloat();
					break;
				case $JSON::ValType::kStr: {
					auto & s = src.getStr();
					if (enableRTIDType && $RTID::isRTIDStr(s)) {
						auto rtidType = $RTID::getRTIDType(s);
						dst << $TypeIdent::kRTID << rtidType;
						switch (rtidType) {
							default: break;
							case $RTID::$TypeIdent::kNullRef:
								break;
							case $RTID::$TypeIdent::kUIDRef: {
								Str content, sheet, uid;
								content.ref(s, "RTID("_len, s.$sz - "RTID()"_len);
								auto splitPos = content.range().find('@');
								sheet.ref(content, splitPos + "@"_len, content.$sz - splitPos - "@"_len);
								uid.ref(content, kNo1, splitPos);
								IU32 uidFirst, uidMiddle, uidLast;
								$Str::scanFmt(uid, $RTID::kUIDFmt.$data, &uidFirst, &uidMiddle, &uidLast);
								$PBVarInt::encU32(static_cast<IU32>(sheet.$sz), dst);
								$PBVarInt::encU32(static_cast<IU32>(sheet.$sz), dst);
								dst << sheet;
								$PBVarInt::encU32(uidMiddle, dst);
								$PBVarInt::encU32(uidFirst, dst);
								dst << uidLast;
								break;
							}
							case $RTID::$TypeIdent::kAliasRef: {
								Str content, sheet, alias;
								content.ref(s, "RTID("_len, s.$sz - "RTID()"_len);
								auto splitPos = content.range().find('@');
								sheet.ref(content, splitPos + "@"_len, content.$sz - splitPos - "@"_len);
								alias.ref(content, kNo1, splitPos);
								$PBVarInt::encU32(static_cast<IU32>(sheet.$sz), dst);
								$PBVarInt::encU32(static_cast<IU32>(sheet.$sz), dst);
								dst << sheet;
								$PBVarInt::encU32(static_cast<IU32>(alias.$sz), dst);
								$PBVarInt::encU32(static_cast<IU32>(alias.$sz), dst);
								dst << alias;
								break;
							}
						}
					} else {
						if (!idxStr.isNull()) {
							if (Sz i; idxStr.range().find(s, i)) {
								dst << $TypeIdent::kStrIdx;
								$PBVarInt::encU32(static_cast<IU32>(i), dst);
							} else {
								idxStr.append(s);
								dst << $TypeIdent::kStrIdxIng;
								$PBVarInt::encU32(static_cast<IU32>(s.$sz), dst);
								dst << s;
							}
						} else {
							dst << $TypeIdent::kStr;
							$PBVarInt::encU32(static_cast<IU32>(s.$sz), dst);
							dst << s;
						}
					}
					break;
				}
				case $JSON::ValType::kArr:
					dst << $TypeIdent::kArrBeg;
					dst << $TypeIdent::kArrSz;
					$PBVarInt::encU32(static_cast<IU32>(src.getArr().$sz), dst);
					for (auto & e : src.getArr()) {
						encUnit(e, dst, idxStr, enableRTIDType);
					}
					dst << $TypeIdent::kArrEnd;
					break;
				case $JSON::ValType::kObj:
					dst << $TypeIdent::kObjBeg;
					for (auto const & e : src.getObj()) {
						encUnit(e.$k, dst, idxStr, enableRTIDType);
						encUnit(e.$v, dst, idxStr, enableRTIDType);
					}
					dst << $TypeIdent::kObjEnd;
					break;
			}
			return;
		}

		extern Void encWhole($JSON::Obj const & src, ByteStream & dst, Bool const & enableStrIdx, Bool const & enableRTIDType) {
			List<Str> idxStr;
			if (enableStrIdx)
				idxStr.alloc($JSON::cntStrSum(src));
			else
				idxStr.setNull();
			$JSON::Val srcShell;
			srcShell.bind(src);
			{
				dst << $StructIdent::kBeg;
				auto posAfterBeg = dst.$pos;
				dst.posGo<IU32>().posBack<Byte>();
				encUnit(srcShell, dst, idxStr, enableRTIDType);
				dst << $StructIdent::kEnd;
				auto posOfEnd = dst.$pos;
				dst.setPosBeg(posAfterBeg) << $StructIdent::kUnKnown1;
				dst.setPosBeg(posOfEnd);
			}
			srcShell.unbind();
			if (enableStrIdx)
				idxStr.free();
			return;
		}

		extern Void encExport(Path const & jsonFile, Path const & rtonFile, Bool const & enableStrIdx, Bool const & enableRTIDType, Bool const & isWhole, Sz const & memSz) {
			$JSON::Val json;
			ByteStream rton;
			$JSON::read(jsonFile, json);
			if (isWhole && !json.isObj())
				throw VAMGException("RTONEncoder", "source JSON must be object when parameter 'isWhole' == true");
			rton.allocThenUse(memSz).setPosBeg();
			if (isWhole)
				encWhole(json.getObj(), rton, enableStrIdx, enableRTIDType);
			else
				encUnit(json, rton, enableStrIdx ? List<Str>().alloc() : List<Str>().setNull(), enableRTIDType);
			rton.contractSzUntilPos();
			$File::writeFile(rtonFile, rton);
			rton.free();
			return;
		}

	}

	namespace $DeCode {

		extern Void decUnit(ByteStream & src, $JSON::Val & dst, List<Str> & idxStr, List<Str> & idxUTF8Str) {
			switch (src.rd<Byte>()) {
				default: {
					src.posBack<Byte>();
					auto ident = src.rdNoPos<Byte>();
					throw VAMGException("RTONDecoder", "[0x%X] unknown RTON-TypeIdent : 0x%02X", src.$pos, ident);
					break;
				}
				case $TypeIdent::kBoolFalse:
					dst.set(kBoF);
					break;
				case $TypeIdent::kBoolTrue:
					dst.set(kBoT);
					break;
				case $TypeIdent::kIntS8:
					dst.set(src.rd<IS8>());
					break;
				case $TypeIdent::kIntS8Zero:
					dst.set(static_cast<IS8>(0));
					break;
				case $TypeIdent::kIntU8:
					dst.set(src.rd<IU8>());
					break;
				case $TypeIdent::kIntU8Zero:
					dst.set(static_cast<IU8>(0));
					break;
				case $TypeIdent::kIntS16:
					dst.set(src.rd<IS16>());
					break;
				case $TypeIdent::kIntS16Zero:
					dst.set(static_cast<IS16>(0));
					break;
				case $TypeIdent::kIntU16:
					dst.set(src.rd<IU16>());
					break;
				case $TypeIdent::kIntU16Zero:
					dst.set(static_cast<IU16>(0));
					break;
				case $TypeIdent::kIntS32:
					dst.set(src.rd<IS32>());
					break;
				case $TypeIdent::kIntS32Zero:
					dst.set(static_cast<IS32>(0));
					break;
				case $TypeIdent::kIntU32:
					dst.set(src.rd<IU32>());
					break;
				case $TypeIdent::kIntU32Zero:
					dst.set(static_cast<IU32>(0));
					break;
				case $TypeIdent::kIntS64:
					dst.set(src.rd<IS64>());
					break;
				case $TypeIdent::kIntS64Zero:
					dst.set(static_cast<IS64>(0));
					break;
				case $TypeIdent::kIntU64:
					dst.set(src.rd<IU64>());
					break;
				case $TypeIdent::kIntU64Zero:
					dst.set(static_cast<IU64>(0));
					break;
				case $TypeIdent::kFloat32: {
					F32 floatVal;
					src >> floatVal;
					dst.set(floatVal);
					break;
				}
				case $TypeIdent::kFloat32Zero:
					dst.set(static_cast<F32>(0.0));
					break;
				case $TypeIdent::kFloat64:
					dst.setFloat();
					src >> dst.getFloat();
					break;
				case $TypeIdent::kFloat64Zero:
					dst.set(static_cast<F64>(0.0));
					break;
				case $TypeIdent::kVarIntU32:
					dst.set($PBVarInt::decU32(src));
					break;
				case $TypeIdent::kVarIntS32:
					dst.set($PBVarInt::decS32(src));
					break;
				case $TypeIdent::kVarIntU32Equal:
					dst.set($PBVarInt::decS32(src));
					break;
				case $TypeIdent::kVarIntU64:
					dst.set($PBVarInt::decU64(src));
					break;
				case $TypeIdent::kVarIntS64:
					dst.set($PBVarInt::decS64(src));
					break;
				case $TypeIdent::kVarIntU64Equal:
					dst.set($PBVarInt::decS64(src));
					break;
				case $TypeIdent::kStr: {
					auto len = $PBVarInt::decU32(src);
					dst.set(Str().ref(reinterpret_cast<Ch const *>(src.curData()), len));
					src.posGo<Ch>(len);
					break;
				}
				case $TypeIdent::kStrIdxIng: {
					auto len = $PBVarInt::decU32(src);
					dst.set(Str().ref(reinterpret_cast<Ch const *>(src.curData()), len));
					src.posGo<Ch>(len);
					idxStr.append(dst.getStr());
					break;
				}
				case $TypeIdent::kStrIdx:
					dst.set(idxStr[$PBVarInt::decU32(src)]);
					break;
				case $TypeIdent::kUTF8Str: {
					auto len = $PBVarInt::decU32(src);
					auto sz = $PBVarInt::decU32(src);
					dst.set(Str().ref(reinterpret_cast<Ch const *>(src.curData()), sz));
					src.posGo<Ch>(sz);
					break;
				}
				case $TypeIdent::kUTF8StrIdxIng: {
					auto len = $PBVarInt::decU32(src);
					auto sz = $PBVarInt::decU32(src);
					dst.set(Str().ref(reinterpret_cast<Ch const *>(src.curData()), sz));
					src.posGo<Ch>(sz);
					idxUTF8Str.append(dst.getStr());
					break;
				}
				case $TypeIdent::kUTF8StrIdx:
					dst.set(idxUTF8Str[$PBVarInt::decU32(src)]);
					break;
				case $TypeIdent::kRTID: {
					auto & rtidStr = dst.setStr().getStr();
					switch (src.rd<Byte>()) {
						default: {
							src.posBack<Byte>();
							auto ident = src.rdNoPos<Byte>();
							throw VAMGException("RTONDecoder", "[0x%X] unknown RTID-Type : 0x%02X at 0x%X", src.$pos, ident);
							break;
						}
						case $RTID::$TypeIdent::kNullRef:
							rtidStr = $RTID::kNullRTID;
							break;
						case $RTID::$TypeIdent::kUIDRef: {
							auto sheetLen = $PBVarInt::decU32(src);
							auto sheetSz = $PBVarInt::decU32(src);
							auto sheetPos = src.$pos;
							src.posGo<Ch>(sheetSz);
							auto uidMiddle = $PBVarInt::decU32(src);
							auto uidFirst = $PBVarInt::decU32(src);
							IU32 uidLast; src >> uidLast;
							auto uid = $Str::printFmt($RTID::kUIDFmt.$data, uidFirst, uidMiddle, uidLast);
							// unknown type of uid's three value , define it 'u32'
							rtidStr.alloc("RTID(..@)"_len + sheetSz + uid.$sz)
								.append("RTID"_r)
								.append('(')
								.append(uid)
								.append('@')
								.append(Str().ref(reinterpret_cast<Ch const *>(src.$data + sheetPos), sheetSz))
								.append(')');
							;
							break;
						}
						case $RTID::$TypeIdent::kAliasRef: {
							auto sheetLen = $PBVarInt::decU32(src);
							auto sheetSz = $PBVarInt::decU32(src);
							auto sheetPos = src.$pos;
							src.posGo<Ch>(sheetSz);
							auto aliasLen = $PBVarInt::decU32(src);
							auto aliasSz = $PBVarInt::decU32(src);
							auto aliasPos = src.$pos;
							src.posGo<Ch>(aliasSz);
							rtidStr.alloc("RTID(@)"_len + sheetSz + aliasSz)
								.append("RTID"_r)
								.append('(')
								.append(Str().ref(reinterpret_cast<Ch const *>(src.$data + aliasPos), aliasSz))
								.append('@')
								.append(Str().ref(reinterpret_cast<Ch const *>(src.$data + sheetPos), sheetSz))
								.append(')');
							break;
						}
					}
					break;
				}
				case $TypeIdent::kRTIDOfNullRef:
					dst.set($RTID::kNullRTID);
					break;
				case $TypeIdent::kBinaryCast: {
					// TODO
					/*StrRef str;
					UI32 strLen;
					UI32 intVal;
					pos += $PBVarIntUtil::dec32(src + pos, strLen);
					str.set(StrRef(reinterpret_cast<Ch const *>(src + pos), strLen));
					pos += szOf<Ch>(strLen);
					pos += $PBVarIntUtil::dec32(src + pos, intVal);
					dst.set(Str().printFmtByAppend(kBinaryCast_Fmt._str, str._str, intVal));
					str.clear();*/
					break;
				}
				case $TypeIdent::kArrBeg: {
					auto & dstArr = dst.setArr().getArr();
					if (src.rdNoPos<Byte>() != $TypeIdent::kArrSz)
						throw VAMGException("RTONDecoder", "[0x%X] can't get array-size", src.$pos);
					src.posGo<Byte>();
					auto sz = $PBVarInt::decU32(src);
					dstArr.alloc(sz);
					while (src.rdNoPos<Byte>() != $TypeIdent::kArrEnd) {
						decUnit(src, dstArr.addUndef().last(), idxStr, idxUTF8Str);
					}
					src.posGo<Byte>();
					if (dstArr.$sz != sz)
						throw VAMGException("RTONDecoder", "[0x%X] array's actual-size is different form declared-size : %u != %u", src.$pos, dstArr.$sz, sz);
					break;
				}
				case $TypeIdent::kObjBeg: {
					auto & dstObj = dst.setObj().getObj();
					List<$JSON::Member *> tmpList;
					tmpList.alloc();
					while (src.rdNoPos<Byte>() != $TypeIdent::kObjEnd) {
						tmpList.append(new $JSON::Member());
						decUnit(src, (*tmpList.last()).$k, idxStr, idxUTF8Str);
						decUnit(src, (*tmpList.last()).$v, idxStr, idxUTF8Str);
					}
					src.posGo<Byte>();
					dstObj.alloc(tmpList.$sz);
					for (auto & e : tmpList) {
						auto & t = dstObj.appendX().last();
						t.$k.moveFrom((*e).$k);
						t.$v.moveFrom((*e).$v);
						pFreeS(e);
					}
					break;
				}
			}
			return;
		}

		extern Void decWhole(ByteStream & src, $JSON::Obj & dst) {
			List<Str> idxStr, idxUTF8Str;
			{
				auto strLimit = kSzN, u8strLimit = kSzN;
				for (auto i = kNo1; i < src.$sz; ++i) {
					switch (src[i]) {
						default:
							break;
						case $TypeIdent::kStrIdxIng:
							++strLimit;
							break;
						case $TypeIdent::kUTF8StrIdxIng:
							++u8strLimit;
							break;
					}
				}
				idxStr.alloc(strLimit);
				idxUTF8Str.alloc(u8strLimit);
			}
			{
				if (FourCC ident; (ident = src.rdNoPos<FourCC>()) != $StructIdent::kBeg)
					throw VAMGException("RTONDecoder", "[0x%X] whole-RTON's begin-ident is error : 0x%08X", src.$pos, ident);
				src.posGo<FourCC>();
				if (FourCC ident; (ident = src.rdNoPos<FourCC>()) != $StructIdent::kUnKnown1)
					throw VAMGException("RTONDecoder", "[0x%X] whole-RTON's unknown-ident is error : 0x%08X", src.$pos, ident);
				src.posGo<FourCC>();
				{
					dst.alloc();
					while (src.rdNoPos<Byte>() != $TypeIdent::kObjEnd) {
						dst.appendX();
						decUnit(src, dst.last().$k, idxStr, idxUTF8Str);
						decUnit(src, dst.last().$v, idxStr, idxUTF8Str);
					}
					src.posGo<Byte>();
				}
				if (FourCC ident; (ident = src.rdNoPos<FourCC>()) != $StructIdent::kEnd)
					throw VAMGException("RTONDecoder", "[0x%X] whole-RTON's end-ident is error : 0x%08X", src.$pos, ident);
				src.posGo<FourCC>();
			}
			idxStr.free();
			idxUTF8Str.free();
			return;
		}

		extern Void decExport(Path const & rtonFile, Path const & jsonFile, Bool const & isWhole) {
			ByteStream rton;
			$JSON::Val json;
			$File::readFileForce(rtonFile, rton);
			rton.setPosBeg();
			if (isWhole)
				decWhole(rton, json.setObj().getObj());
			else
				decUnit(rton, json, List<Str>().alloc(), List<Str>().alloc());
			$JSON::write(jsonFile, json);
			rton.free();
			return;
		}

	}

}
