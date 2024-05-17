# include "./RSBPatch.h"

# include "../../MyType/Map.h"
# include "../../UtilKit/DevUtil.h"
# include "../../InfoUtil/InfoUtil.h"
# include "../../RTONUtil/Encoder_Export.h"
# include "../../ImageUtil/PopTexEncoder/PopTexEncoder_Export.h"

# include "../../ResPkgUtil/RSGPUtil/Packer.h"
# include "../../ResPkgUtil/RSGPUtil/Packer_Export.h"

namespace $TJ::$SpecialMod::$PvZ_2::$RSBPatch {

	namespace $Diff {
		Void makePatch(
			$InfoUtil::$ResPkgInfo::ResPkgInfo & ori,
			$InfoUtil::$ResPkgInfo::ResPkgInfo & modify,
			$PatchInfo::PatchInfo & patch) {

			for_criter(e, modify._group) {
				auto groupIdx = ori._group.findKey(e._key);
				if (groupIdx == kThNull) {
					// ... easy copy
				} else {
					for_criter(ee, e._val._subGroup) {
						auto subGroupIdx = e._val._subGroup.findKey(ee._key);
						if (subGroupIdx == kThNull) {
							// ... easy copy
						} else {

						}
					}

				}
			}

			return;
		}
	}












	auto findRes(Path const & path, ListP<$ResPkgUtil::$RSBUtil::$StructInfo::Res> const & resInfoList, SzI & idx) {
		for_cnt_ev(idx, kThFirst, resInfoList._sz)
			if (pathIsEqual(resInfoList[idx]._path, path))
				break;
		return idx < resInfoList._sz;
	}
	namespace $PatchResInfo {
		auto patchRes(Str const & id, $PatchInfo::Res const & src, MapOfInfo<$InfoUtil::$ResPkgInfo::Res> & infoList) {
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info._val._path.setNull();
					info._val._type.setNull();
					info._val.setNormalType();
				}
				if (src._path._exist) {
					info._val._path = src._path._val;
				}
				if (src._type._exist) {
					info._val._type = src._type._val;
				}
				if (src.isAtlasType()) {
					info._val.setAtlasType();
					if (src.getAtlasInfo()._sz._exist) {
						info._val.getAtlasInfo()._sz = src.getAtlasInfo()._sz._val;
					}
					if (src.getAtlasInfo()._img._exist) {
						info._val.getAtlasInfo()._img = src.getAtlasInfo()._img._val;
					}
				}
			}
			return;
		}
		auto patchSubGroup(Str const & id, $PatchInfo::SubGroup const & src, MapOfInfo<$InfoUtil::$ResPkgInfo::SubGroup> & infoList) {
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info._val._category.setNull();
					info._val._res.setNull();
				}
				if (src._category._exist) {
					info._val._category = src._category._val;
				}
				if (src._res._exist) {
					for_criter(res, src._res._val)
						patchRes(res._key, res._val, info._val._res);
				}
			}
			return;
		}
		auto patchGroup(Str const & id, $PatchInfo::Group const & src, MapOfInfo<$InfoUtil::$ResPkgInfo::Group> & infoList) {
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info._val._isComposite = kTrue;
					info._val._subGroup.setNull();
				}
				if (src._isComposite._exist) {
					info._val._isComposite = src._isComposite._val;
				}
				if (src._subGroup._exist) {
					for_criter(subGroup, src._subGroup._val)
						patchSubGroup(subGroup._key, subGroup._val, info._val._subGroup);
				}
			}
			return;
		}
		Void patch($PatchInfo::PatchInfo const & patch, $InfoUtil::$ResPkgInfo::ResPkgInfo & info) {
			if (patch._group._exist) {
				for_criter(group, patch._group._val)
					patchGroup(group._key, group._val, info._group);
			}
			return;
		}
	}
	namespace $PatchStructInfo {
		auto patchRes(Str const & id, $PatchInfo::Res const & src, $ResPkgUtil::$RSBUtil::$StructInfo::ResList & infoList) {
			Path actualPath; actualPath = src._path._val;
			if (src.isAtlasType())
				actualPath.setSuffix("ptx");
			SzI idx; auto exist = findRes(actualPath, infoList, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif((!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) && src._path._val.getFullPath() != "!program") {
				auto & info = exist ? infoList.get(idx) : infoList.add().getLast();
				if (!exist) {
					info._path = actualPath;
				}
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info.setNormalType();
				}
				if (src.isAtlasType()) {
					info.setAtlasType();
					info.getAtlasInfo()._idx = infoList.GetAtlasResSum() - kThSecond;
					if (src.getAtlasInfo()._sz._exist) {
						info.getAtlasInfo()._sz = src.getAtlasInfo()._sz._val;
					}
					if (src.getAtlasInfo()._texFmt._exist) {
						info.getAtlasInfo()._texFmt = src.getAtlasInfo()._texFmt._val;
					}
				}
			}
			return;
		}
		auto patchSubGroup(Str const & id, $PatchInfo::SubGroup const & src, MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::SubGroup> & infoList) {
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info._val._headerType = 4u;
					info._val._compressNormalRes = kTrue;
					info._val._compressAtlasRes = kTrue;
					info._val._category.setNull();
					info._val._res.setNull();
				}
				if (src._category._exist) {
					info._val._category = src._category._val;
				}
				if (src._res._exist) {
					for_criter(res, src._res._val)
						patchRes(res._key, res._val, info._val._res);
				}
			}
			return;
		}
		auto patchGroup(Str const & id, $PatchInfo::Group const & src, MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::Group> & infoList) {
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				if (exist) infoList.erase(idx);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					info._val._isComposite = kTrue;
					info._val._subGroup.setNull();
				}
				if (src._isComposite._exist) {
					info._val._isComposite = src._isComposite._val;
				}
				if (src._subGroup._exist) {
					for_criter(subGroup, src._subGroup._val)
						patchSubGroup(subGroup._key, subGroup._val, info._val._subGroup);
				}
			}
			return;
		}
		Void patch($PatchInfo::PatchInfo const & patch, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & info) {
			if (patch._group._exist) {
				for_criter(group, patch._group._val)
					patchGroup(group._key, group._val, info._group);
			}
			return;
		}
	}
	namespace $PatchRes {
		static Path gSrcResDir;
		auto patchRes(Str const & id, $PatchInfo::Res const & src, $ResPkgUtil::$RSBUtil::$StructInfo::ResList & infoList, Path const & parentDir) {
			Path actualPath; actualPath = src._path._val;
			if (src.isAtlasType())
				actualPath.setSuffix("ptx");
			SzI idx; auto exist = findRes(actualPath, infoList, idx);
			Path resFile; resFile.cpy(parentDir).add(actualPath);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				$DevUtil::ReMovFile(resFile);
			} elif((!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) && src._path._val.getFullPath() != "!program") {
				if (src._tactic == $PatchInfo::Tactic::kOverWrite) {
					$DevUtil::CpyFile(Path(gSrcResDir).add(actualPath), resFile);
				}
			}
			return;
		}
		auto patchSubGroup(Str const & id, $PatchInfo::SubGroup const & src, MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::SubGroup> & infoList, Path const & parentDir) {
			Path subGroupDir; subGroupDir.cpy(parentDir).add(id);
			Path resDir; resDir.cpy(subGroupDir).add("Res");
			Path packetFile; packetFile.cpy(subGroupDir).add("Packet.rsgp");
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				$DevUtil::ReMovDir(subGroupDir);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if ($DevUtil::existFile(packetFile)) {
					$ResPkgUtil::$RSGPUtil::$Packer::$UnPack::expUnPackDflt(packetFile, subGroupDir);
					$DevUtil::ReMovFile(packetFile);
				}
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					$DevUtil::ReMovDir(subGroupDir);
					info._val._res.setNull();
				}
				$DevUtil::makeDir(subGroupDir);
				$DevUtil::makeDir(resDir);
				if (src._res._exist) {
					for_criter(res, src._res._val)
						patchRes(res._key, res._val, info._val._res, resDir);
				}
			}
			return;
		}
		auto patchGroup(Str const & id, $PatchInfo::Group const & src, MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::Group> & infoList, Path const & parentDir) {
			Path groupDir; groupDir.cpy(parentDir).add(id);
			SzI idx; auto exist = infoList.findKey(id, idx);
			if (src._tactic == $PatchInfo::Tactic::kRemove) {
				$DevUtil::ReMovDir(groupDir);
			} elif(!exist || src._tactic != $PatchInfo::Tactic::kPassIfExist) {
				auto & info = exist ? infoList.get(idx) : infoList.addByKey(id).getLast();
				if (src._tactic != $PatchInfo::Tactic::kBaseOn) {
					$DevUtil::ReMovDir(groupDir);
					$DevUtil::makeDir(groupDir);
					info._val._subGroup.setNull();
				}
				if (src._subGroup._exist) {
					for_criter(subGroup, src._subGroup._val)
						patchSubGroup(subGroup._key, subGroup._val, info._val._subGroup, groupDir);
				}
			}
			return;
		}
		Void patch($PatchInfo::PatchInfo const & patchInfo, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & info, Path const & srcResDir, Path const & dstGroupDir) {
			gSrcResDir = srcResDir;
			if (patchInfo._group._exist) {
				for_criter(group, patchInfo._group._val)
					patchGroup(group._key, group._val, info._group, dstGroupDir);
			}
			return;
		}
	}
}
