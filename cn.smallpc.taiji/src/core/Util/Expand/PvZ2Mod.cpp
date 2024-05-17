#include "./PvZ2Mod.hpp"

#include "./JSONPatch.hpp"
#include "./RSBPatchX.hpp"
#include "../File/File.hpp"
#include "../ResPkg/RSB/Packer.hpp"
#include "../RTON/Encoder.hpp"
#include "../Info/Info.hpp"

namespace $TJ::$Util::$Expand::$PvZ2Mod {

	static auto intro(EnvCfg const & env) {
		for (auto const & e : env.$rsb) {
			$ResPkg::$RSB::$Packer::$UnPack::unpackExport(e.$file, e.$dir + "info.json"_r, Path(), e.$dir + "packet"_r, Path(), Path(), Path(), Path(), Path(), Path());
			$File::makeDir(e.$dir + "res"_r);
			$ResPkg::$RSB::$Info::Info rsbInfo;
			rsbInfo.read($JSON::read(e.$dir + "info.json"_r, crv($JSON::Val())));
			auto packetID = $RSBPatchX::findResPacket(e.$oriInfoFile, rsbInfo);
			auto packetFile((e.$dir + "packet"_r + packetID).setExtension("rsgp"_r));
			$ResPkg::$RSB::$RSGP::$Packer::$UnPack::unpackExport(packetFile, Path(), e.$dir + "res"_r);
			$File::remove(packetFile);
			$JSON::Obj infoXOri;
			{
				ByteSection rton;
				$File::readFileForce(e.$dir + "res"_r + e.$oriInfoFile, rton);
				$RTON::$Encoder::$DeCode::decWhole(ByteStream().ref(rton).setPosBeg(), infoXOri);
			}
			$Info::$RSBInfoX::Info infoX;
			$Info::$Cnv::cnv($Info::$RSBInfoXOri::Info().read(infoXOri), infoX);
			$JSON::write(e.$dir + "infoX.json"_r, infoX.write(crv($JSON::Val())));
		}
		return;
	}
	static auto outro(EnvCfg const & env) {
		for (auto const & e : env.$rsb) {
			$Info::$RSBInfoXOri::Info infoXOri;
			$Info::$Cnv::cnv($Info::$RSBInfoX::Info().read($JSON::read(e.$dir + "infoX.json"_r, crv($JSON::Val()))), infoXOri, kNo1);
			{
				ByteStream rton;
				rton.allocThenUse(env.$memSzWhenRTONEnCode * kSzBM).setPosBeg();
				$RTON::$Encoder::$EnCode::encWhole(infoXOri.write(crv($JSON::Val())), rton, env.$enableStrIdxWhenRTONEnCode, kBoF);
				rton.contractSzUntilPos();
				$File::writeFile(e.$dir + "res"_r + e.$oriInfoFile, rton);
			}
			$ResPkg::$RSB::$Packer::$Pack::packExport(e.$file, e.$dir + "info.json"_r, Path(), e.$dir + "packet"_r, e.$dir + "res"_r, Path(), Path(), Path(), kBoF, kBoF, kBoT, env.$memSzWhenRSBPack * kSzBM);
		}
		return;
	}

	static auto & getAllFileFromRSBInfo($ResPkg::$RSB::$Info::Info const & info, List<Path> & result) {
		Sz resSum, atlasResSum;
		info.cntSum(crv(Sz()), crv(Sz()), resSum, atlasResSum);
		result.alloc(resSum + atlasResSum);
		for (auto const & e : info.$group) {
			for (auto const & ee : e.$v.$subGroup) {
				for (auto const & eee : ee.$v.$res) {
					result.append(eee.$path);
				}
			}
		}
		return result;
	}

	declaration static Void apply(PatchInfo const & patchInfo, EnvCfg const & env, Path const & modDir);
	declaration static Void apply(Mod const & patchInfo, EnvCfg const & env, Path const & modDir);

	static Void apply(PatchInfo const & patchInfo, EnvCfg const & env, Path const & modDir) {
		if (!patchInfo.$enable)
			return;
		log.cmt("start patch : %s", patchInfo.$name.$data);
		auto tgtDir(env.$path[patchInfo.$target.first().getStr()]);
		switch (patchInfo.$type.hashByPtr(operator""_sh)) {
			default:
				throw VAMGException("PvZ2Mod::apply", "error patch type");
				break;
			case "PvZ2Mod"_sh: {
				auto theModDir((modDir + patchInfo.$name).setExtension("tjpvz2mod"_r));
				apply(Mod().read($JSON::read(theModDir + "mod.json"_r, crv($JSON::Val()))), env, theModDir);
				break;
			}
			case "RSBPatchX"_sh: {
				auto patchDir((modDir + patchInfo.$name).setExtension("tjrsbpatchx"_r));
				$RSBPatchX::$Apply::applyExport(patchDir + "patch.json"_r,
												tgtDir + "infoX.json"_r, tgtDir + "info.json"_r,
												tgtDir + "infoX.json"_r, tgtDir + "info.json"_r,
												kBoT, patchDir + "res"_r, tgtDir + "packet"_r, tgtDir + "res"_r);
				break;
			}
			case "JSONPatch"_sh: {
				InfoList<$JSONPatch::Patch> jsonPatch;
				jsonPatch.read($JSON::read((modDir + patchInfo.$name).setExtension("tjjsonpatch"_r), crv($JSON::Arr())));

				List<Path> targetFile;
				$ResPkg::$RSB::$Info::Info rsbInfo;

				auto isEmbedded = patchInfo.$target[kNo3].getBool();
				Bool isRTON; isRTON = kBoT;

				if (isEmbedded) {
					rsbInfo.read($JSON::read(tgtDir + "info.json"_r, crv($JSON::Val())));
					$Path::select($JSON::getListFromArr(patchInfo.$target[kNo2], crv(List<Str>())), getAllFileFromRSBInfo(rsbInfo, crv(List<Path>())), targetFile);
				} else {
					$Path::select($JSON::getListFromArr(patchInfo.$target[kNo2], crv(List<Str>())), tgtDir, targetFile);
				}
				for (auto const & e : targetFile) {
					Path jsonPath = isEmbedded ? (tgtDir + "res"_r + e) : (tgtDir + e);
					if (isEmbedded) {
						Str packetID = $RSBPatchX::findResPacket(e, rsbInfo);
						if (packetID.isNull()) {
							log.war("can not found embedded target file : %s", e.fullName().$data);
							log.cmt("finish patch");
							return;
						}
						auto packetFile((tgtDir + "packet"_r + packetID).setExtension("rsgp"_r));
						if ($File::existFile(packetFile)) {
							log.cmt("unpack rsgp [%s] ...", packetID.$data);
							$ResPkg::$RSB::$RSGP::$Packer::$UnPack::unpackExport(packetFile, Path(), tgtDir + "res"_r);
							$File::remove(packetFile);
						}
					}
					$JSON::Val json, jsonPatched;
					if (isRTON) {
						ByteStream rton;
						$File::readFileForce(jsonPath, rton);
						rton.setPosBeg();
						$RTON::$Encoder::$DeCode::decWhole(rton, json.setObj().getObj());
						rton.free().alloc(env.$memSzWhenRTONEnCode * kSzBM);
						$JSONPatch::$Apply::apply(jsonPatch, json);
						$JSON::eraseUndef(json, jsonPatched);
						rton.setPosBeg();
						$RTON::$Encoder::$EnCode::encWhole(jsonPatched.getObj(), rton, kBoT, kBoT);
						rton.contractSzUntilPos();
						$File::writeFile(jsonPath, rton);
						rton.free();
					} else {
						$JSON::read(jsonPath, json);
						$JSONPatch::$Apply::apply(jsonPatch, json);
						$JSON::eraseUndef(json, jsonPatched);
						$JSON::write(jsonPath, json);
					}
				}
				break;
			}
		}
		log.cmt("finish patch");
		return;
	}

	static Void apply(Mod const & patchInfo, EnvCfg const & env, Path const & modDir) {
		for (auto const & e : patchInfo) {
			apply(e, env, modDir);
		}
		return;
	}

	extern Void introExport(Path const & envFile) {
		intro(EnvCfg().read($JSON::read(envFile, crv($JSON::Val()))));
		return;
	}
	extern Void outroExport(Path const & envFile) {
		outro(EnvCfg().read($JSON::read(envFile, crv($JSON::Val()))));
		return;
	}

	extern Void applyExport(Path const & envFile, Path const & modDir) {
		apply(Mod().read($JSON::read(modDir + "mod.json"_r, crv($JSON::Val()))), EnvCfg().read($JSON::read(envFile, crv($JSON::Val()))), modDir);
		return;
	}

}
