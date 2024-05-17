# include "./Packer.h"

# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../MyType/DataStream.h"
# include "../../MyType/Map.h"
# include "../../MyType/Path.h"
# include "../../UtilKit/JSONUtil.h"

namespace $TJ::$AudioUtil::$SoundBankUtil {
	Map<UI32, Str> gNameMapper;

	namespace $NameMapperMod {
		Void load(StrList const & src) {
			gNameMapper.setNull();
			for_criter(s, src)
				gNameMapper.addByKV(calcID32(s), s);
			return;
		}
		Void load($JSON::Arr const & src) {
			gNameMapper.setNull();
			for_criter(s, src)
				gNameMapper.addByKV(calcID32(s.getStr()), s.getStr());
			return;
		}
		Void clear() {
			gNameMapper.setNull();
			return;
		}
	}
}
namespace $TJ::$AudioUtil::$SoundBankUtil::$Packer {
	namespace $Pack {
		Void Pack(Byte * const & data, SzI & sz, Path const & mainDir) {
			Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, mainDir);
			StrList blockIDList;
			$JSON::getArrStr($JSON::read(Path("BlockList.json"), $JSON::Arr().setNull()), blockIDList);
			$Struct::$BlockSign::Member blockSign;
			sz = kSzZero;
			if (blockIDList.exist($Struct::$BlockIDStr::kBKHD)) {
				$StructInfo::BKHD bkhdInfo;
				bkhdInfo.read($JSON::read(Path("BKHD.json"), $JSON::Obj().setNull()));
				auto bkhdData = data + (sz + blockSign.getSz());
				auto bkhdSz = kSzZero;
				{
					$Struct::$BKHD_Begin::Member bkhdBeginBlock;
					bkhdBeginBlock._id = bkhdInfo._id;
					bkhdBeginBlock._ver = bkhdInfo._ver;
					bkhdSz += $DataBlockUtil::write(bkhdData + kThFirst, bkhdBeginBlock).getSz();
				}
				if ($DevUtil::existFile(Path("BKHD_Append"))) {
					auto moreBlockSz = $DevUtil::getFileSize(Path("BKHD_Append"));
					$DevUtil::ReadData(Path("BKHD_Append"), bkhdData + bkhdSz, moreBlockSz);
					bkhdSz += moreBlockSz;
				}
				{
					blockSign._id = $Struct::$BlockID::kBKHD; blockSign._sz = bkhdSz;
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + +blockSign._sz;
				}
				bkhdData = kNullPtr;
			}
			if (blockIDList.exist($Struct::$BlockIDStr::kDIDX) && blockIDList.exist($Struct::$BlockIDStr::kDATA)) {
				ListOfInfo<$StructInfo::DIDX> didxInfoList;
				didxInfoList.read($JSON::read(Path("DIDX.json"), $JSON::Arr().setNull()));
				auto didxData = data + (sz + blockSign.getSz());
				auto didxSz = kSzZero;
				auto dataData = didxData + (($Struct::$DIDX_Sub::kBlockSize * didxInfoList._sz) + blockSign.getSz());
				auto dataSz = kSzZero;
				for_criter(didxInfo, didxInfoList) {
					$Struct::$DIDX_Sub::Member didxSubBlock;
					didxSubBlock._id = didxInfo._id;
					didxSubBlock._ofs = dataSz;
					$DevUtil::ReadData(Path("Data").add(Str().printFmtByAppend("%u", didxSubBlock._id)).setSuffix("wem"), dataData + didxSubBlock._ofs, didxSubBlock._sz);
					dataSz += didxSubBlock._sz;
					didxSz += $DataBlockUtil::write(didxData + didxSz, didxSubBlock).getSz();
				}
				{
					blockSign._id = $Struct::$BlockID::kDIDX; blockSign._sz = didxSz;
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + blockSign._sz;
				}
				{
					blockSign._id = $Struct::$BlockID::kDATA; blockSign._sz = dataSz;
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + blockSign._sz;
				}
				didxData = kNullPtr; dataData = kNullPtr;
			}
			if (blockIDList.exist($Struct::$BlockIDStr::kSTID)) {
				ListOfInfo<$StructInfo::STID> stidInfoList;
				stidInfoList.read($JSON::read(Path("STID.json"), $JSON::Arr().setNull()));
				auto stidData = data + (sz + blockSign.getSz());
				auto stidSz = kSzZero;
				{
					$Struct::$STID_Begin::Member stidBeginBlock;
					stidBeginBlock._unKnown_1 = $Struct::$STID_Begin::$MemberConst::kUnKnown_1;
					stidBeginBlock._itemSum = stidInfoList._sz;
					stidSz += $DataBlockUtil::write(stidData + stidSz, stidBeginBlock).getSz();
				}
				for_criter(stidInfo, stidInfoList) {
					$Struct::STID_Sub stidSubBlock;
					stidSubBlock._id = stidInfo._id;
					stidSubBlock._name.set(stidInfo._name);
					stidSz += stidSubBlock.write(stidData + stidSz).getSz();
					stidSubBlock._name.tfree();
				}
				{
					blockSign._id = $Struct::$BlockID::kSTID; blockSign._sz = stidSz;
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + blockSign._sz;
				}
				stidData = kNullPtr;
			}
			if (blockIDList.exist($Struct::$BlockIDStr::kHIRC)) {
				ListOfInfo<$StructInfo::HIRC> hircInfoList;
				hircInfoList.read($JSON::read(Path("HIRC.json"), $JSON::Arr().setNull()));
				auto hircData = data + (sz + blockSign.getSz());
				auto hircSz = kSzZero;
				{
					$Struct::$HIRC_Begin::Member hircBeginBlock;
					hircBeginBlock._sz = hircInfoList._sz;
					hircSz += $DataBlockUtil::write(hircData + hircSz, hircBeginBlock).getSz();
				}
				for_criter(hircInfo, hircInfoList) {
					$Struct::$HIRC_Sub_Begin::Member hircSubBlockBegin;
					hircSubBlockBegin._id = hircInfo._id;
					hircSubBlockBegin._type = hircInfo._type;
					memCpy(hircData + hircSz + hircSubBlockBegin.getSz(), hircInfo._data._item, hircInfo._data._sz);
					hircSubBlockBegin._sz = hircInfo._data._sz + $Struct::$HIRC_Sub_Begin::$MemberOSD::kID._sz;
					hircSz += $DataBlockUtil::write(hircData + hircSz, hircSubBlockBegin).getSz();
					hircSz += hircSubBlockBegin._sz - $Struct::$HIRC_Sub_Begin::$MemberOSD::kID._sz;
				}
				{
					blockSign._id = $Struct::$BlockID::kHIRC; blockSign._sz = hircSz;
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + blockSign._sz;
				}
				hircData = kNullPtr;
			}
			for_criter(blockID, blockIDList) {
				if (blockID != $Struct::$BlockIDStr::kBKHD &&
					blockID != $Struct::$BlockIDStr::kDIDX &&
					blockID != $Struct::$BlockIDStr::kDATA &&
					blockID != $Struct::$BlockIDStr::kSTID &&
					blockID != $Struct::$BlockIDStr::kHIRC) {
					memCpy(&blockSign._id, blockID._str, $Struct::$BlockSign::$MemberOSD::kID._sz);
					$DevUtil::ReadData(Path(blockID), data + sz + blockSign.getSz(), blockSign._sz);
					sz += $DataBlockUtil::write(data + sz, blockSign).getSz() + blockSign._sz;
				}
			}
			$DevUtil::chgDirThenFree(parentDir);
			return;
		}
	}
	namespace $UnPack {
		static auto ParseSoundStruct(DataStream & stm, $JSON::Obj & dst) {
			dst.add("OverridParent", stm.out<Bool>());
			UI8 effectSum; stm.out(effectSum);
			dst.add("EffectSum", effectSum);
			if (effectSum > kSzZero) {
				dst.add("bitMask", stm.out<UI8>());
				dst.addObj("Effect");
				for_cnt(idx, kThFirst, effectSum) {
					dst.getLast()._val.getObj().add("Index", stm.out<UI8>()).add("ObjectID", stm.out<UI32>());
					stm.pos += 2; // pass, always zero?
				}
			}
			dst.add("OutputBusID", stm.out<UI32>());
			dst.add("ParentID", stm.out<UI32>());
			dst.add("OverrideParentPlaybackPriority", stm.out<Bool>());
			dst.add("ActivatedDistance", stm.out<Bool>());
			UI8 parmSum; stm.out(parmSum);
			dst.add("ParmSum", parmSum);
			dst.addArr("ParmType");
			for_cnt(idx, kThFirst, parmSum) {
				dst.getLast()._val.getArr().add(stm.out<UI8>());
			}
			dst.addArr("ParmValue");
			for_cnt(idx, kThFirst, parmSum) {
				dst.getLast()._val.getArr().add(stm.out<UI32>());
			}
			return;
		}
		static auto ParseHIRCData($StructInfo::HIRC & src) {
			DataStream stm;
			stm.setRef(src._data._item, src._data._cap);
			switch (src._type) {
				default: break;
				case 0x1: { // Settings // ok
					UI8 settingSum; stm.out(settingSum);
					auto & setting = src._plain.addArr("Setting").getLast()._val.getArr().allocThenNew(settingSum);
					for_cnt(idx, kThFirst, settingSum) {
						setting[idx].setObj();
					}
					for_cnt(idx, kThFirst, settingSum) {
						setting[idx].getObj().add("Type", stm.out<UI8>());
					}
					for_cnt(idx, kThFirst, settingSum) {
						setting[idx].getObj().add("Val", stm.out<F32>());
					}
					break;
				}
				case 0x2: { // Sound SFX/Sound Voice
					src._plain.add("UnKnown1", stm.out<UI32>());
					UI8 soundLoad; stm.out(soundLoad); // byte shound is
					switch (soundLoad) {
						default: break;
						case 0x0: src._plain.add("Load", StrRef("inline")); break;
						case 0x1: src._plain.add("Load", StrRef("extern")); break;
						case 0x2: src._plain.add("Load", StrRef("extern-loaded")); break;
					}
					//src._plain.add("SoundID", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src._plain.add("SoundID").getLast()._val);
					src._plain.add("SrcID", stm.out<UI32>());
					if (soundLoad == 0x0) { // if inline
						src._plain.add("SrcOfs", stm.out<UI32>());
						src._plain.add("SrcSz", stm.out<UI32>());
					}
					UI8 soundType; stm.out(soundType);
					switch (soundType) {
						default: break;
						case 0x0: src._plain.add("SoundType", StrRef("SFX")); break;
						case 0x1: src._plain.add("SoundType", StrRef("Voice")); break;
					}
					// then is sound-struct
					src._plain.addObj("SoundStruct");
					ParseSoundStruct(stm, src._plain.getLast()._val.getObj());
					break;
				}
				case 0x3: { // Event Action
					src._plain.add("Scope", stm.out<UI8>());
					UI8 actionType; stm.out(actionType);
					src._plain.add("Type", actionType);
					//src._plain.add("ObjectID", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src._plain.add("Object").getLast()._val);
					stm.pos += 1; // pass
					src._plain.addArr("Parm").getLast()._val.getArr().allocThenNew(stm.out<UI8>());
					for_riter(e, src._plain.getLast()._val.getArr()) {
						e.setObj();
					}
					for_riter(e, src._plain.getLast()._val.getArr()) {
						e.getObj().add("Type", stm.out<UI8>());
					}
					for_riter(e, src._plain.getLast()._val.getArr()) {
						switch (e["Type"].getUInt()) {
							default: wErr("Parm Type error"); break;
							case 0x0E:
							case 0x0F: e.getObj().add("Val", stm.out<UI32>()); break;
							case 0x10: e.getObj().add("Val", stm.out<F32>()); break;
						}
					}
					stm.pos += 1; // pass
					if (actionType == 0x12) {
						//src._plain.add("StateGroupID", stm.out<UI32>());
						//src._plain.add("StateID", stm.out<UI32>());
						setIDMaybeHasName(stm.out<UI32>(), src._plain.add("StateGroupID").getLast()._val);
						setIDMaybeHasName(stm.out<UI32>(), src._plain.add("StateID").getLast()._val);
					} elif(actionType == 0x19) {
						//src._plain.add("SwitchGroupID", stm.out<UI32>());
						//src._plain.add("SwitchID", stm.out<UI32>());
						setIDMaybeHasName(stm.out<UI32>(), src._plain.add("SwitchGroupID").getLast()._val);
						setIDMaybeHasName(stm.out<UI32>(), src._plain.add("SwitchID").getLast()._val);
					}
					// then is unknown
					break;
				}
				case 0x4: { // Event // ok
					UI32 actionSum; stm.out(actionSum);
					src._plain.addArr("Action");
					for_cnt(idx, kThFirst, actionSum) {
						setIDMaybeHasName(stm.out<UI32>(), src._plain.getLast()._val.getArr().add().getLast());
					}
					break;
				}
				case 0x5: { // unknown
					break;
				}
				case 0x6: { // unknown
					break;
				}
				case 0x7: { // unknown
					break;
				}
				case 0x8: {
					break;
					//src._plain.add("ParentObject", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src._plain.add("ParentObject").getLast()._val);
					UI8 parmSum; stm.out(parmSum);
					auto & parm = src._plain.addArr("Parm").getLast()._val.getArr().allocThenNew(parmSum);
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].setObj();
					}
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].getObj().add("Type", stm.out<UI8>());
					}
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].getObj().add("Val", stm.out<F32>());
					}
					src._plain.add("What to do when priority is equal", stm.out<UI8>());
					src._plain.add("What to do when limit is reached", stm.out<UI8>());
					src._plain.add("Limit sound instances to ...", stm.out<UI16>());
					src._plain.add("whether to override parent settings for Playback Limit", stm.out<Bool>());
					src._plain.add("unknown, always 3F 00 00 00 (= 63)", stm.out<UI32>());
					src._plain.add("Auto-ducking: Recover time (ms)", stm.out<UI32>());
					src._plain.add("Auto-ducking: Maximum ducking volume", stm.out<F32>());
					src._plain.add("Auto-ducking: Number of ducked busses", stm.out<UI32>());
					break;
				}
				case 0x9: { // unknown
					break;
				}
				case 0xA: {
					break;
					// first is sound-struct

					// own property
					UI32 childSum; stm.out(childSum);
					src._plain.addArr("Child");
					for_cnt(idx, kThFirst, childSum) {
						src._plain.getLast()._val.getArr().add(stm.out<UI32>());
					}
					// 剩余的未知字节
					break;
				}
				case 0xB: { // ??
					break;
				}
				case 0xC: { // Music Switch Container
					break;
					// first is sound-struct

					// own property
					UI32 childSum; stm.out(childSum);
					src._plain.addArr("Child");
					for_cnt(idx, kThFirst, childSum) {
						src._plain.getLast()._val.getArr().add(stm.out<UI32>());
					}
					stm.pos += 4; // pass
					src._plain.add("UnKnownF32", stm.out<F32>());
					stm.pos += 8; // pass
					src._plain.add("tempo (usually 120)", stm.out<F32>());
					src._plain.add("time signature, part 1", stm.out<UI8>());
					src._plain.add("time signature, part 2 (usually 4/4)", stm.out<UI8>());
					src._plain.add("always 01", stm.out<UI8>());
					stm.pos += 4; // pass
					UI32 transitionSum; stm.out(transitionSum);
					auto & transition = src._plain.addArr("Transition").getLast()._val.getArr().allocThenNew(transitionSum);
					for_cnt(idx, kThFirst, childSum) {
						auto & dstTrans = transition[idx].setObj().getObj();
						dstTrans.add("SrcID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("Source Fade-out Time (ms)", stm.out<SI32>());
						dstTrans.add("shape of Source Fade-out curve", stm.out<UI32>());
						dstTrans.add("Source Fade-out Offset (ms)", stm.out<SI32>());
						dstTrans.add("Exit source at", stm.out<UI32>());
						// WARN : maybe u32 at here, pass it
						dstTrans.add("00 = Nothing, FF = Play post-exit section of Source object", stm.out<UI8>());
						dstTrans.add("Destination Fade-in Time (ms)", stm.out<SI32>());
						dstTrans.add("shape of Destination Fade-in curve", stm.out<UI32>());
						dstTrans.add("Destination Fade-in Offset (ms)", stm.out<SI32>());
						dstTrans.add("id of Match for Custom Cue Filter", stm.out<UI32>());
						dstTrans.add("the id of the Playlist item to jump to", stm.out<UI32>());
						dstTrans.add("Sync to", stm.out<UI16>());
						dstTrans.add("00 = Nothing, FF = Play pre-entry section of Destination object", stm.out<UI8>());
						// WARN : maybe u32 at here, pass it
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
						dstTrans.add("DstID", stm.out<UI32>());
					}
					break;
				}
				case 0xD: {
					break;
				}
				case 0xE: { // unknown
					break;
				}
				case 0xF: { // unknown
					break;
				}
				case 0x10: { // unknown
					break;
				}
				case 0x11: { // unknown
					break;
				}
				case 0x12: { // unknown
					break;
				}
				case 0x13: { // unknown
					break;
				}
				case 0x14: { // unknown
					break;
				}
			}
			return;
		}
		static auto & ParseSTMGData(DataStream & src, $JSON::Obj & dst) {
			dst.add("Volume Threshold", src.out<F32>());
			dst.add("Max Voice Instances", src.out<UI16>());
			dst.addArr("StateGroup").getLast()._val.getArr().allocThenNew(src.out<UI32>());
			for_riter(e, dst.getLast()._val.getArr()) {
				e.setObj();
				setIDMaybeHasName(src.out<UI32>(), e.getObj().add("ID").getLast()._val);
				e.getObj().add("default transition time", src.out<UI32>());
				UI32 customTransitionTimeSum = src.out<UI32>();
				e.getObj().addArr("CustomTransitionTime").getLast()._val.getArr().allocThenNew(customTransitionTimeSum);
				for_riter(ee, e.getObj().getLast()._val.getArr()) {
					ee.setObj();
					setIDMaybeHasName(src.out<UI32>(), ee.getObj().add("From").getLast()._val);
					setIDMaybeHasName(src.out<UI32>(), ee.getObj().add("To").getLast()._val);
					ee.getObj().add("Time", src.out<UI32>());
				}
			}
			dst.addArr("SwitchGroup").getLast()._val.getArr().allocThenNew(src.out<UI32>());
			for_riter(e, dst.getLast()._val.getArr()) {
				e.setObj();
				setIDMaybeHasName(src.out<UI32>(), e.getObj().add("ID").getLast()._val);
				setIDMaybeHasName(src.out<UI32>(), e.getObj().add("GameParm").getLast()._val);
				src.pos += 1;
				e.getObj().addArr("Point").getLast()._val.getArr().allocThenNew(src.out<UI32>());
				for_riter(ee, e.getObj().getLast()._val.getArr()) {
					ee.setObj();
					ee.getObj().add("GameParmVal", src.out<F32>());
					setIDMaybeHasName(src.out<UI32>(), ee.getObj().add("Switch").getLast()._val);
					ee.getObj().add("CurveShape", src.out<UI32>());
				}
			}
			dst.addArr("GameParm").getLast()._val.getArr().allocThenNew(src.out<UI32>());
			for_riter(e, dst.getLast()._val.getArr()) {
				e.setObj();
				continue; // todo
				setIDMaybeHasName(src.out<UI32>(), e.getObj().add("ID").getLast()._val);
				e.getObj().add("Val", src.out<F32>());
			}
			return dst;
		}
		Void UnPack(Byte const * const & data, SzI const & sz, Path const & mainDir) {
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, mainDir);

			Byte const * bkhdData(kNullPtr); SzI bkhdSz;
			Byte const * didxData(kNullPtr); SzI didxSz;
			Byte const * dataData(kNullPtr); SzI dataSz;
			Byte const * stidData(kNullPtr); SzI stidSz;
			Byte const * stmgData(kNullPtr); SzI stmgSz;
			Byte const * hircData(kNullPtr); SzI hircSz;
			{
				StrList blockIDList;
				$Struct::$BlockSign::Member blockSign;
				for (auto pos = kThFirst; pos < sz;) {
					pos += $DataBlockUtil::read(data + pos, blockSign).getSz();
					blockIDList.add(StrRef(reinterpret_cast<Ch const *>(&blockSign._id), $Struct::$BlockSign::$MemberOSD::kID._sz));
					switch (blockSign._id) {
						default: $DevUtil::WriteData(Path(StrRef(reinterpret_cast<Ch const *>(&blockSign._id), $Struct::$BlockSign::$MemberOSD::kID._sz)), data + pos, blockSign._sz); break;
						case $Struct::$BlockID::kBKHD: bkhdData = data + pos; bkhdSz = blockSign._sz; break;
						case $Struct::$BlockID::kDIDX: didxData = data + pos; didxSz = blockSign._sz; break;
						case $Struct::$BlockID::kDATA: dataData = data + pos; dataSz = blockSign._sz; break;
						case $Struct::$BlockID::kSTID: stidData = data + pos; stidSz = blockSign._sz; break;
						case $Struct::$BlockID::kSTMG: stmgData = data + pos; stmgSz = blockSign._sz; break;
						case $Struct::$BlockID::kHIRC: hircData = data + pos; hircSz = blockSign._sz; break;
					}
					pos += blockSign._sz;
				}
				$JSON::write(Path("BlockList.json"), $JSON::setArrStr(blockIDList, $JSON::Arr().setNull()));
				blockIDList.tfree();
			}
			if (bkhdData) {
				$Struct::$BKHD_Begin::Member bkhdBeginBlock;
				$DataBlockUtil::read(bkhdData + kThFirst, bkhdBeginBlock);
				$JSON::write(Path("BKHD.json"), $StructInfo::BKHD().parse(bkhdBeginBlock).write($JSON::Obj().setNull()));
				if (bkhdBeginBlock.getSz() < bkhdSz)
					$DevUtil::WriteData(Path("BKHD_Append"), bkhdData + bkhdBeginBlock.getSz(), bkhdSz - bkhdBeginBlock.getSz());
				bkhdData = kNullPtr;
			}
			if (didxData && dataData) {
				ListOfBlock<$Struct::$DIDX_Sub::Member> didxSubBlockList;
				ListOfInfo<$StructInfo::DIDX> didxInfoList;
				$DataBlockUtil::readArr(didxData + kThFirst, didxSubBlockList.allocThenNew(didxSz / $Struct::$DIDX_Sub::kBlockSize));
				didxInfoList.parse(didxSubBlockList);
				$JSON::write(Path("DIDX.json"), didxInfoList.writeJustVal($JSON::Arr().setNull()));
				for_criter(didxSubBlock, didxSubBlockList)
					$DevUtil::WriteData(Path("Data").add(Str().printFmtByAppend("%u", didxSubBlock._id)).setSuffix("wem"), dataData + didxSubBlock._ofs, didxSubBlock._sz);
				didxSubBlockList.tfree(); didxInfoList.tfree();
				didxData = kNullPtr; dataData = kNullPtr;
			}
			if (stidData) {
				$Struct::$STID_Begin::Member stidBeginBlock;
				ListOfBlock<$Struct::STID_Sub> stidSubBlockList;
				ListOfInfo<$StructInfo::STID> stidInfoList;
				$DataBlockUtil::read(stidData + kThFirst, stidBeginBlock);
				stidSubBlockList.read(stidData + stidBeginBlock.getSz(), stidBeginBlock._itemSum);
				stidInfoList.parse(stidSubBlockList);
				$JSON::write(Path("STID.json"), stidInfoList.writeJustVal($JSON::Arr().setNull()));
				stidSubBlockList.tfree(); stidInfoList.tfree();
				stidData = kNullPtr;
			}
			if (stmgData) {
				$JSON::write(Path("STMG.json"), ParseSTMGData(DataStream().setRef(stmgData, stmgSz), $JSON::Obj().setNull()));
			}
			if (hircData) {
				ListOfInfo<$StructInfo::HIRC> hircInfoList;
				$Struct::$HIRC_Begin::Member hircBeginBlock;
				$Struct::$HIRC_Sub_Begin::Member hircSubBlockBegin;
				auto hircPos = kThFirst;
				hircPos += $DataBlockUtil::read(hircData + hircPos, hircBeginBlock).getSz();
				hircInfoList.alloc(hircBeginBlock._sz);
				for_cnt(hircSubBlockIdx, kThFirst, hircBeginBlock._sz) {
					hircPos += $DataBlockUtil::read(hircData + hircPos, hircSubBlockBegin).getSz();
					hircInfoList.add().getLast().parse(hircSubBlockBegin, hircData + hircPos, hircSubBlockBegin._sz - $Struct::$HIRC_Sub_Begin::$MemberOSD::kID._sz);
					ParseHIRCData(hircInfoList.getLast());
					hircPos += hircSubBlockBegin._sz - $Struct::$HIRC_Sub_Begin::$MemberOSD::kID._sz;
				}
				$JSON::write(Path("HIRC.json"), hircInfoList.write($JSON::Arr().setNull()));
				hircInfoList.tfree();
				hircData = kNullPtr;
			}
			$DevUtil::chgDirThenFree(parentDir);
			return;
		}
	}
}
