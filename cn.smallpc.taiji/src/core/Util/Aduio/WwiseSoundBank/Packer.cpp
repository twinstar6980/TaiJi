#include "./Packer.hpp"

#include "../../../Type/BlockFunc.hpp"
#include "../../File/File.hpp"

#include "../../../Type/ByteStream.hpp"
#include "../../../Type/Map.hpp"
#include "../../../Type/Path.hpp"
#include "../../../Type/JSON.hpp"

#include <filesystem>

namespace $TJ::$Util::$Audio::$WwiseSoundBank {

	namespace $NameMapperMod {

		Void load(List<Str> const & t) {
			gNameMapper.alloc();
			for (auto const & s : t)
				gNameMapper(calcID32(s)) = s;
			return;
		}
		Void load($JSON::Arr const & t) {
			gNameMapper.alloc();
			for (auto const & s : t)
				gNameMapper(calcID32(s.getStr())) = s.getStr();
			return;
		}
		Void clear() {
			gNameMapper.alloc();
			return;
		}

	}

}

namespace $TJ::$Util::$Audio::$WwiseSoundBank::$Packer {

	namespace $Pack {

		static constexpr Sz kWEMPaddingSzUnit(0x10);

		Void pack(ByteSection & data, Path const & mainDir) {
			assert(data.isEmpty());
			ByteStream stm;
			stm.ref(data.$data, data.$cap);
			Path parentDir; $File::getCurDirThenChgDir(parentDir, mainDir);
			List<Str> chunkID;
			$JSON::getListFromArr($JSON::read("chunk.json"_path, crv($JSON::Val())), chunkID);
			if (chunkID.range().exist($Struct::$ChunkIDStr::kBKHD)) {
				$Info::BKHD bkhdInfo;
				bkhdInfo.read($JSON::read("BKHD.json"_path, crv($JSON::Val())));
				auto signPos = stm.$pos;
				stm.posGo($Struct::ChunkSign::kSz);
				$StaticBlock::write($Struct::BKHD_Begin({ bkhdInfo.$ver, bkhdInfo.$id }), stm);
				if ($File::existFile("BKHD.dat"_path)) {
					auto extraDataSz = $File::getFileSize("BKHD.dat"_path);
					$File::readFile("BKHD.dat"_path, ByteSection().ref(stm, stm.$pos, kSzN, extraDataSz));
					stm.posGo(extraDataSz);
				}
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kBKHD, stm.$pos - (signPos + $Struct::ChunkSign::kSz) }), ByteSection().ref(stm, signPos, $Struct::ChunkSign::kSz));
			}
			if (chunkID.range().exist($Struct::$ChunkIDStr::kDIDX) &&
				chunkID.range().exist($Struct::$ChunkIDStr::kDATA)) {
				InfoList<$Info::DIDX> didxInfo;
				didxInfo.read($JSON::read("DIDX.json"_path, crv($JSON::Val())));
				ByteStream didxStm;
				auto didxSignPos = stm.$pos;
				stm.posGo($Struct::ChunkSign::kSz);
				didxStm.ref(stm, stm.$pos, $Struct::DIDX_Sub::kSz * didxInfo.$sz).setPosBeg();
				stm.posGo(didxStm.$sz);
				auto dataSignPos = stm.$pos;
				stm.posGo($Struct::ChunkSign::kSz);
				auto dataPos = stm.$pos;
				for (auto const & e : didxInfo) {
					auto wemFile(("DATA"_path + $Str::printFmt("%u", e.$id)).setExtension("wem"_r));
					auto wemSz = $File::getFileSize(wemFile);
					stm.setPosBeg(calcEnoughSz(stm.$pos, kWEMPaddingSzUnit));
					$StaticBlock::write($Struct::DIDX_Sub({ e.$id, stm.$pos - dataPos, wemSz }), didxStm);
					$File::readFile(wemFile, ByteSection().ref(stm, stm.$pos, kSzN, wemSz));
					stm.posGo(wemSz);
				}
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kDIDX, didxStm.$sz }), ByteSection().ref(stm, didxSignPos, $Struct::ChunkSign::kSz));
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kDATA, stm.$pos - dataPos }), ByteSection().ref(stm, dataSignPos, $Struct::ChunkSign::kSz));
				didxStm.unref();
			}
			if (chunkID.range().exist($Struct::$ChunkIDStr::kSTMG)) {
				auto stmgSz = $File::getFileSize("STMG.dat"_path);
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kSTMG, stmgSz }), stm);
				$File::readFile("STMG.dat"_path, ByteSection().ref(stm, stm.$pos, kSzN, stmgSz));
				stm.posGo(stmgSz);
			}
			if (chunkID.range().exist($Struct::$ChunkIDStr::kHIRC)) {
				InfoList<$Info::HIRC> hircInfo;
				hircInfo.read($JSON::read("HIRC.json"_path, crv($JSON::Val())));
				auto signPos = stm.$pos;
				stm.posGo($Struct::ChunkSign::kSz);
				$StaticBlock::write($Struct::HIRC_Begin({ hircInfo.$sz }), stm);
				for (auto const & e : hircInfo) {
					$StaticBlock::write($Struct::HIRC_Sub_Begin({ e.$type, e.$data.$sz + kTSz<decltype(std::declval<$Struct::HIRC_Sub_Begin>().$id)> , e.$id }), stm);
					stm << e.$data;
				}
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kHIRC, stm.$pos - (signPos + $Struct::ChunkSign::kSz) }), ByteSection().ref(stm, signPos, $Struct::ChunkSign::kSz));
			}
			if (chunkID.range().exist($Struct::$ChunkIDStr::kSTID)) {
				InfoList<$Info::STID> stidInfoList;
				stidInfoList.read($JSON::read("STID.json"_path, crv($JSON::Val())));
				auto signPos = stm.$pos;
				stm.posGo($Struct::ChunkSign::kSz);
				$StaticBlock::write($Struct::STID_Begin({ 1, stidInfoList.$sz }), stm);
				for (auto const & e : stidInfoList) {
					$Struct::STID_Sub({ calcID32(e.$name), { Str(e.$name) } }).write(stm);
				}
				$StaticBlock::write($Struct::ChunkSign({ $Struct::$ChunkID::kSTID, stm.$pos - (signPos + $Struct::ChunkSign::kSz) }), ByteSection().ref(stm, signPos, $Struct::ChunkSign::kSz));
			}
			for (auto const & e : chunkID) {
				if (e != $Struct::$ChunkIDStr::kBKHD &&
					e != $Struct::$ChunkIDStr::kDIDX &&
					e != $Struct::$ChunkIDStr::kDATA &&
					e != $Struct::$ChunkIDStr::kSTID &&
					e != $Struct::$ChunkIDStr::kSTMG &&
					e != $Struct::$ChunkIDStr::kHIRC) {
					Path chunkFile(e);
					auto chunkSz = $File::getFileSize(chunkFile);
					$StaticBlock::write($Struct::ChunkSign({ *reinterpret_cast<FourCC const * const &>(e.$data), chunkSz }), stm);
					$File::readFile(chunkFile, ByteSection().ref(stm, stm.$pos, kSzN, chunkSz));
					stm.posGo(chunkSz);
				}
			}
			$File::chgDirThenFree(parentDir);
			data.setSz(stm.$pos);
			stm.unref();
			return;
		}

		extern Void packExport(Path const & dataFile, Path const & mainDir, Sz const & memSz) {
			ByteSection data;
			data.alloc(memSz);
			pack(data, mainDir);
			$File::writeFile(dataFile, data);
			data.free();
			return;
		}

	}

	namespace $UnPack {

		/*static auto ParseSoundStruct(DataStream & stm, $JSON::Obj & dst) {
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
			stm.setRef(src.$data._item, src.$data._cap);
			switch (src.$type) {
				default: break;
				case 0x1: { // Settings // ok
					UI8 settingSum; stm.out(settingSum);
					auto & setting = src.$plain.addArr("Setting").getLast()._val.getArr().allocThenNew(settingSum);
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
					src.$plain.add("UnKnown1", stm.out<UI32>());
					UI8 soundLoad; stm.out(soundLoad); // byte shound is
					switch (soundLoad) {
						default: break;
						case 0x0: src.$plain.add("Load", StrRef("inline")); break;
						case 0x1: src.$plain.add("Load", StrRef("extern")); break;
						case 0x2: src.$plain.add("Load", StrRef("extern-loaded")); break;
					}
					//src.$plain.add("SoundID", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("SoundID").getLast()._val);
					src.$plain.add("SrcID", stm.out<UI32>());
					if (soundLoad == 0x0) { // if inline
						src.$plain.add("SrcOfs", stm.out<UI32>());
						src.$plain.add("SrcSz", stm.out<UI32>());
					}
					UI8 soundType; stm.out(soundType);
					switch (soundType) {
						default: break;
						case 0x0: src.$plain.add("SoundType", StrRef("SFX")); break;
						case 0x1: src.$plain.add("SoundType", StrRef("Voice")); break;
					}
					// then is sound-struct
					src.$plain.addObj("SoundStruct");
					ParseSoundStruct(stm, src.$plain.getLast()._val.getObj());
					break;
				}
				case 0x3: { // Event Action
					src.$plain.add("Scope", stm.out<UI8>());
					UI8 actionType; stm.out(actionType);
					src.$plain.add("Type", actionType);
					//src.$plain.add("ObjectID", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("Object").getLast()._val);
					stm.pos += 1; // pass
					src.$plain.addArr("Parm").getLast()._val.getArr().allocThenNew(stm.out<UI8>());
					for_riter(e, src.$plain.getLast()._val.getArr()) {
						e.setObj();
					}
					for_riter(e, src.$plain.getLast()._val.getArr()) {
						e.getObj().add("Type", stm.out<UI8>());
					}
					for_riter(e, src.$plain.getLast()._val.getArr()) {
						switch (e["Type"].getUInt()) {
							default: log.err("Parm Type error"); break;
							case 0x0E:
							case 0x0F: e.getObj().add("Val", stm.out<UI32>()); break;
							case 0x10: e.getObj().add("Val", stm.out<F32>()); break;
						}
					}
					stm.pos += 1; // pass
					if (actionType == 0x12) {
						//src.$plain.add("StateGroupID", stm.out<UI32>());
						//src.$plain.add("StateID", stm.out<UI32>());
						setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("StateGroupID").getLast()._val);
						setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("StateID").getLast()._val);
					} elif(actionType == 0x19) {
						//src.$plain.add("SwitchGroupID", stm.out<UI32>());
						//src.$plain.add("SwitchID", stm.out<UI32>());
						setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("SwitchGroupID").getLast()._val);
						setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("SwitchID").getLast()._val);
					}
					// then is unknown
					break;
				}
				case 0x4: { // Event // ok
					UI32 actionSum; stm.out(actionSum);
					src.$plain.addArr("Action");
					for_cnt(idx, kThFirst, actionSum) {
						setIDMaybeHasName(stm.out<UI32>(), src.$plain.getLast()._val.getArr().add().getLast());
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
					//src.$plain.add("ParentObject", stm.out<UI32>());
					setIDMaybeHasName(stm.out<UI32>(), src.$plain.add("ParentObject").getLast()._val);
					UI8 parmSum; stm.out(parmSum);
					auto & parm = src.$plain.addArr("Parm").getLast()._val.getArr().allocThenNew(parmSum);
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].setObj();
					}
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].getObj().add("Type", stm.out<UI8>());
					}
					for_cnt(idx, kThFirst, parmSum) {
						parm[idx].getObj().add("Val", stm.out<F32>());
					}
					src.$plain.add("What to do when priority is equal", stm.out<UI8>());
					src.$plain.add("What to do when limit is reached", stm.out<UI8>());
					src.$plain.add("Limit sound instances to ...", stm.out<UI16>());
					src.$plain.add("whether to override parent settings for Playback Limit", stm.out<Bool>());
					src.$plain.add("unknown, always 3F 00 00 00 (= 63)", stm.out<UI32>());
					src.$plain.add("Auto-ducking: Recover time (ms)", stm.out<UI32>());
					src.$plain.add("Auto-ducking: Maximum ducking volume", stm.out<F32>());
					src.$plain.add("Auto-ducking: Number of ducked busses", stm.out<UI32>());
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
					src.$plain.addArr("Child");
					for_cnt(idx, kThFirst, childSum) {
						src.$plain.getLast()._val.getArr().add(stm.out<UI32>());
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
					src.$plain.addArr("Child");
					for_cnt(idx, kThFirst, childSum) {
						src.$plain.getLast()._val.getArr().add(stm.out<UI32>());
					}
					stm.pos += 4; // pass
					src.$plain.add("UnKnownF32", stm.out<F32>());
					stm.pos += 8; // pass
					src.$plain.add("tempo (usually 120)", stm.out<F32>());
					src.$plain.add("time signature, part 1", stm.out<UI8>());
					src.$plain.add("time signature, part 2 (usually 4/4)", stm.out<UI8>());
					src.$plain.add("always 01", stm.out<UI8>());
					stm.pos += 4; // pass
					UI32 transitionSum; stm.out(transitionSum);
					auto & transition = src.$plain.addArr("Transition").getLast()._val.getArr().allocThenNew(transitionSum);
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
		*/

		Void unpack(ByteSection const & data, Path const & mainDir) {
			Path parentDir; $File::getCurDirThenChgDirForce(parentDir, mainDir);
			ByteSection bkhdData, didxData, dataData, stidData, stmgData, hircData;
			{
				List<Str> chunkID;
				chunkID.alloc();
				ByteStream stm;
				stm.ref(data).setPosBeg();
				while (!stm.eos()) {
					$Struct::ChunkSign sign;
					$StaticBlock::read(sign, stm);
					Str curID;
					curID.ref(reinterpret_cast<Ch const *>(&sign.$id), kTSz<FourCC>);
					ByteSection curChunk;
					curChunk.ref(stm, stm.$pos, sign.$sz);
					chunkID.append(curID);
					switch (sign.$id) {
						default:
							$File::writeFile(Path(curID), curChunk);
							break;
						case $Struct::$ChunkID::kBKHD: bkhdData.ref(curChunk); break;
						case $Struct::$ChunkID::kDIDX: didxData.ref(curChunk); break;
						case $Struct::$ChunkID::kDATA: dataData.ref(curChunk); break;
						case $Struct::$ChunkID::kSTMG: stmgData.ref(curChunk); break;
						case $Struct::$ChunkID::kHIRC: hircData.ref(curChunk); break;
						case $Struct::$ChunkID::kSTID: stidData.ref(curChunk); break;
					}
					curChunk.unref();
					stm.posGo(sign.$sz);
				}
				stm.unref();
				$JSON::write("chunk.json"_path, $JSON::setListToArr(chunkID, crv($JSON::Val())));
				chunkID.free();
			}
			if (!bkhdData.isNull()) {
				$Struct::BKHD_Begin bkhdBegBlock;
				$StaticBlock::read(bkhdBegBlock, bkhdData);
				$JSON::write("BKHD.json"_path, $Info::BKHD().parse(bkhdBegBlock).write(crv($JSON::Val())));
				if (bkhdData.$sz > bkhdBegBlock.kSz)
					$File::writeFile("BKHD.dat"_path, ByteSection().ref(bkhdData, bkhdBegBlock.kSz));
				bkhdData.unref();
			}
			if (!didxData.isNull() && !dataData.isNull()) {
				List<$Struct::DIDX_Sub> didxSubBlock;
				InfoList<$Info::DIDX> didxInfo;
				didxSubBlock.allocThenUse(didxData.$sz / $Struct::DIDX_Sub::kSz);
				$StaticBlock::read(didxSubBlock, didxData);
				didxInfo.parse(didxSubBlock);
				$JSON::write("DIDX.json"_path, didxInfo.write(crv($JSON::Val())));
				for (auto const & e : didxSubBlock)
					$File::writeFile(("DATA"_path + $Str::printFmt("%u", e.$id)).setExtension("wem"_r), ByteSection().ref(dataData, e.$ofs, e.$sz));
				didxSubBlock.free();
				didxInfo.free();
				didxData.unref();
				dataData.unref();
			}
			if (!stmgData.isNull()) {
				$File::writeFile("STMG.dat"_path, stmgData);
				//$JSON::write("STMG.json"_path, parseSTMGData(stmgData, crv($JSON::Obj())));
			}
			if (!hircData.isNull()) {
				InfoList<$Info::HIRC> hircInfoList;
				$Struct::HIRC_Begin hircBeginBlock;
				$Struct::HIRC_Sub_Begin hircSubBlockBegin;
				ByteStream stm;
				stm.ref(hircData).setPosBeg();
				$StaticBlock::read(hircBeginBlock, stm);
				hircInfoList.alloc(hircBeginBlock.$sz);
				for (auto i = kNo1; i < hircBeginBlock.$sz; ++i) {
					$StaticBlock::read(hircSubBlockBegin, stm);
					hircInfoList.appendX().last().parse(hircSubBlockBegin, ByteSection().ref(stm, stm.$pos, hircSubBlockBegin.$sz - kTSz<decltype(hircSubBlockBegin.$id)>));
					//ParseHIRCData(hircInfoList.last());
					stm.posGo(hircSubBlockBegin.$sz - kTSz<decltype(hircSubBlockBegin.$id)>);
				}
				$JSON::write("HIRC.json"_path, hircInfoList.write(crv($JSON::Val())));
				hircInfoList.free();
				hircData.unref();
			}
			if (!stidData.isNull()) {
				$Struct::STID_Begin stidBegBlock;
				List<$Struct::STID_Sub> stidSubBlock;
				InfoList<$Info::STID> stidInfo;
				$StaticBlock::read(stidBegBlock, stidData);
				stidSubBlock.allocThenUse(stidBegBlock.$itemSum);
				$DynamicBlock::read(stidSubBlock, ByteSection().ref(stidData, stidBegBlock.kSz));
				stidInfo.parse(stidSubBlock);
				$JSON::write("STID.json"_path, stidInfo.write(crv($JSON::Val())));
				stidSubBlock.free();
				stidInfo.free();
				stidData.unref();
			}
			$File::chgDirThenFree(parentDir);
			return;
		}

		extern Void unpackExport(Path const & dataFile, Path const & mainDir) {
			ByteSection data;
			$File::readFileForce(dataFile, data);
			unpack(data, mainDir);
			data.free();
			return;
		}

	}

}
