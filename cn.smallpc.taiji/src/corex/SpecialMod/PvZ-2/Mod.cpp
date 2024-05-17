# include "./Mod.h"

# include "../../MyType/Map.h"
# include "../../UtilKit/DevUtil.h"
# include "../../InfoUtil/InfoUtil.h"
# include "../../RTONUtil/Encoder_Export.h"
# include "../../ImageUtil/PopTexEncoder/PopTexEncoder_Export.h"

# include "../../InfoUtil/ResPkgInfo.h"
# include "../../ResPkgUtil/RSBUtil/StructInfo.h"
# include "../../ResPkgUtil/RSBUtil/Packer_Export.h"
# include "../../ResPkgUtil/RSGPUtil/Packer_Export.h"
# include "../../ImageUtil/ImageInfoType.h"

namespace $TJ::$SpecialMod::$PvZ_2::$Mod {

	static auto intro(EnvCfg const & env) {
		for_criter(e, env._rsbBuild) {
			$ResPkgUtil::$RSBUtil::$Packer::$UnPack::expUnPackDflt(e._rsbFile, e._rsbDir, kFalse, kFalse, kFalse, kTrue, kFalse, kTrue);
			Path resInfoPacketDir; resInfoPacketDir.cpy(e._rsbDir).add("Group").add(e._resInfoGroup).add(e._resInfoSubGroup);
			{
				Path resInfoPacketFile; resInfoPacketFile.cpy(resInfoPacketDir).add("Packet.rsgp");
				$ResPkgUtil::$RSGPUtil::$Packer::$UnPack::expUnPackDflt(resInfoPacketFile, resInfoPacketDir);
				$DevUtil::ReMovFile(resInfoPacketFile);
			}
			$JSON::Obj resInfoJSON;
			$InfoUtil::$ResPkgInfo::ResPkgInfo resInfo;
			{
				Byte * rtonData; SzI rtonSz;
				$DevUtil::ForceReadData(Path(resInfoPacketDir).add("Res").add(e._resInfoPath), rtonData, rtonSz);
				$RTONUtil::$Encoder::$DeCode::decFull(rtonData, rtonSz, resInfoJSON);
				delPtrArr(rtonData);
			}
			$InfoUtil::$Cnv::$OriToResPkgInfo::ParseInfo($InfoUtil::$ResPkgInfo_Ori::ResPkgInfo_Ori().read(resInfoJSON), resInfo);
			$JSON::write(Path(e._rsbDir).add("ResInfo.json"), resInfo.write($JSON::Obj().setNull()));
		}
		return;
	}
	static auto outro(EnvCfg const & env) {
		for_criter(e, env._rsbBuild) {
			$InfoUtil::$ResPkgInfo_Ori::ResPkgInfo_Ori resInfoOri;
			$InfoUtil::$Cnv::$ResPkgInfoToOri::ParseInfo($InfoUtil::$ResPkgInfo::ResPkgInfo().read($JSON::read(Path(e._rsbDir).add("ResInfo.json"), $JSON::Obj().setNull())), resInfoOri);
			{
				Path resInfoFile; resInfoFile.cpy(e._rsbDir).add("Group").add(e._resInfoGroup).add(e._resInfoSubGroup).add("Res").add(e._resInfoPath);
				Byte * rtonData; SzI rtonSz;
				newPtr(rtonData, env._memSzWhenRTONEnCode * kSzBinMega);
				$RTONUtil::$Encoder::$EnCode::encFull(resInfoOri.write($JSON::Obj().setNull()), rtonData, rtonSz, env._useCacheStrWhenRTONEnCode, kTrue);
				$DevUtil::WriteData(resInfoFile, rtonData, rtonSz);
				delPtrArr(rtonData);
			}
			$ResPkgUtil::$RSBUtil::$Packer::$Pack::expPackDflt(e._rsbFile, e._rsbDir, kTrue, kTrue, kFalse, kTrue, kTrue, env._memSzWhenRSBPack * kSzBinMega);

		}
		return;
	}

	static auto findFileAtRSBStruct($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & rsbStruct, Path const & path, Str & group, Str & subGroup) {
		group.setNull(); subGroup.setNull();
		for_criter(e, rsbStruct._group) {
			if (!group.isNull())
				break;
			for_criter(ee, e._val._subGroup) {
				if (!group.isNull())
					break;
				for_criter(eee, ee._val._res) {
					if (pathIsEqual(eee._path, path)) {
						group = e._key;
						subGroup = ee._key;
						break;
					}
				}
			}
		}
		return;
	}

	static auto apply(PatchInfo const & patchInfo, EnvCfg const & env, Path const & modDir) {
		if (!patchInfo._enable)
			return;
		wCmt("Start Patch : %s", patchInfo._name._str);
		Path tgtDir; tgtDir = env._path.getValByKey(patchInfo._target[kThFirst].getStr());
		if (patchInfo._type == "RSBPatch") {
			Path patchDir, patchFile;
			patchDir.cpy(modDir).add(patchInfo._name).setSuffix("rsbpatch");
			patchFile.cpy(patchDir).add("Patch.json");
			$RSBPatch::patchRes(patchFile, Path(tgtDir).add("StructInfo.json"), Path(patchDir).add("Res"), Path(tgtDir).add("Group"));
			$RSBPatch::patchResInfo(patchFile, Path(tgtDir).add("ResInfo.json"), Path(tgtDir).add("ResInfo.json"));
			$RSBPatch::patchStructInfo(patchFile, Path(tgtDir).add("StructInfo.json"), Path(tgtDir).add("StructInfo.json"));
		} elif(patchInfo._type == "JSONPatch") {
			ListOfInfo<$JSON::$Patch::Patch> jsonPatch;
			jsonPatch.read($JSON::read(Path(modDir).add(patchInfo._name).setSuffix("jsonpatch"), $JSON::Arr().setNull()));

			Path jsonPath, rawJSONPath; rawJSONPath.read(patchInfo._target[kThSecond].getStr());

			auto isEmbedded = patchInfo._target[kThThird].getBool();

			if (isEmbedded) {
				Str groupID, subGroupID;
				$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo rsbStruct;
				rsbStruct.read($JSON::read(Path(tgtDir).add("StructInfo.json"), $JSON::Obj().setNull()));
				findFileAtRSBStruct(rsbStruct, rawJSONPath, groupID, subGroupID);
				if (groupID.isNull() || subGroupID.isNull()) {
					wWarn("Can not found embedded target file : %s", rawJSONPath._sz);
					wCmt("Finish Patch");
					return;
				}
				Path rsgpDir; rsgpDir.cpy(tgtDir).add("Group").add(groupID).add(subGroupID);
				Path packetFile; packetFile.cpy(rsgpDir).add("Packet.rsgp");
				jsonPath.cpy(rsgpDir).add("Res").add(rawJSONPath);
				if ($DevUtil::existFile(packetFile)) {
					wCmt("Found Packet File , Now Extract ...");
					$ResPkgUtil::$RSGPUtil::$Packer::$UnPack::expUnPackDflt(packetFile, rsgpDir);
					$DevUtil::ReMovFile(packetFile);
				}
			} else {
				jsonPath.cpy(tgtDir).add(rawJSONPath);
				if (!$DevUtil::existFile(jsonPath)) {
					wWarn("Can not found target file : %s", jsonPath.getFullPath()._str);
					wCmt("Finish Patch");
					return;
				}
			}

			$JSON::Val json;
			Bool isRTON; isRTON = true;

			if (isRTON) {
				Byte * rtonData; SzI rtonSz;
				$DevUtil::ForceReadData(jsonPath, rtonData, rtonSz);
				$RTONUtil::$Encoder::$DeCode::decFull(rtonData, rtonSz, json.setObj().getObj());
				delPtrArr(rtonData);
				newPtr(rtonData, env._memSzWhenRTONEnCode * kSzBinMega);
				$JSON::$Patch::apply(jsonPatch, json);
				eraseUndef(json);
				$RTONUtil::$Encoder::$EnCode::encFull(json.getObj(), rtonData, rtonSz, kTrue, kTrue);
				$DevUtil::WriteData(jsonPath, rtonData, rtonSz);
				delPtrArr(rtonData);
			} else {
				$JSON::read(jsonPath, json);
				$JSON::$Patch::apply(jsonPatch, json);
				eraseUndef(json);
				$JSON::write(jsonPath, json);
			}
		}
		wCmt("Finish Patch");
		return;
	}

	static auto apply(Mod const & patchInfo, EnvCfg const & env, Path const & modDir) {
		for_criter(e, patchInfo) {
			apply(e, env, modDir);
		}
		return;
	}

	Void expApply(Path const & modDir, Path const & envFile) {
		apply(Mod().read($JSON::read(Path(modDir).add("Config.json"), $JSON::Arr().setNull())), EnvCfg().read($JSON::read(envFile, $JSON::Obj().setNull())), modDir);
		return;
	}

	Void expIntro(Path const & envFile) {
		intro(EnvCfg().read($JSON::read(envFile, $JSON::Obj().setNull())));
		return;
	}
	Void expOutro(Path const & envFile) {
		outro(EnvCfg().read($JSON::read(envFile, $JSON::Obj().setNull())));
		return;
	}
}
