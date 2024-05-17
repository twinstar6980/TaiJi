#include "./RSBPatchX.hpp"

#include "../File/File.hpp"
#include "../RTON/Encoder.hpp"
#include "../Image/Encoder/Encoder.hpp"
#include "../Anim/PopcapAnim/Encoder.hpp"
#include "../ResPkg/RSB/Packer.hpp"
#include "../ResPkg/RSB/RSGP/Packer.hpp"

namespace $TJ::$Util::$Expand::$RSBPatchX {

	namespace $Diff {

		Void diff(ByteSection const & src, ByteSection const & dst) {
			return;
		}

		Void makePatch($Info::$RSBInfoX::Info & ori,
					   $Info::$RSBInfoX::Info & modify,
					   $PatchInfo::Info & patch
		) {
			for (auto const & e : modify.$group) {
				if (Sz i; ori.$group.findKey(e.$k, i)) {
					for (auto const & ee : e.$v.$subGroup) {
						if (Sz ii; e.$v.$subGroup.findKey(ee.$k, ii)) {
						} else {
							// ... easy copy
						}
					}
				} else {
					// ... easy copy
				}
			}
			return;
		}

	}

	namespace $Apply {

		static Bool gApplyToResFile;
		static Path gSrcResDir, gDstPacketDir, gDstResDir;

		auto removeAllFile($ResPkg::$RSB::$Info::Res const & info) {
			if (!info.$path.isNull())
				$File::remove(gDstResDir + info.$path);
			return;
		}
		auto removeAllFile(Str const & id, $ResPkg::$RSB::$Info::SubGroup const & info) {
			$File::remove((gDstPacketDir + id).setExtension("rsgp"_r));
			if (info.$res.isNull())
				return;
			for (auto const & e : info.$res)
				$File::remove(gDstResDir + e.$path);
			return;
		}
		auto removeAllFile($ResPkg::$RSB::$Info::Group const & info) {
			if (info.$subGroup.isNull())
				return;
			for (auto const & e : info.$subGroup)
				removeAllFile(e.$k, e.$v);
			return;
		}

		auto const applyShell = [](auto const & fun, auto const & patch, auto & listX, auto & list) {
			constexpr auto isResList = kTSame<TRR<decltype(list)>, $ResPkg::$RSB::$Info::ResList>;
			auto doApply = [&fun, &patch, &listX, &list](Bool const & allowCreate, Str const & id, Bool const & exist, Sz const & i, Sz const & ii) {
				constexpr auto isResList = kTSame<TRR<decltype(list)>, $ResPkg::$RSB::$Info::ResList>;
				switch (patch.$tactic) {
					default:
						break;
					case $PatchInfo::Tactic::kBase:
						if (exist) {
							if constexpr (isResList)
								fun(patch.$reset, id, patch.$data, listX.get(i).$v, list[ii], list.calcAtlasResSum());
							else
								fun(patch.$reset, id, patch.$data, listX.get(i).$v, list.get(ii).$v);
						}
						break;
					case $PatchInfo::Tactic::kCreate:
						if (allowCreate && !exist) {
							if constexpr (isResList)
								fun(kBoT, id, patch.$data, listX.appendX().last().setKey(patch.$target).$v, list.appendX().last(), list.calcAtlasResSum());
							else
								fun(kBoT, id, patch.$data, listX.appendX().last().setKey(patch.$target).$v, list.appendX().last().setKey(patch.$target).$v);
						}
						break;
					case $PatchInfo::Tactic::kCreateForce:
						if (exist) {
							if constexpr (isResList)
								fun(kBoT, id, patch.$data, listX.get(i).$v, list[ii], list.calcAtlasResSum());
							else
								fun(kBoT, id, patch.$data, listX.get(i).$v, list.get(ii).$v);
						} else if (allowCreate) {
							if constexpr (isResList)
								fun(kBoT, id, patch.$data, listX.appendX().last().setKey(patch.$target).$v, list.appendX().last(), list.calcAtlasResSum());
							else
								fun(kBoT, id, patch.$data, listX.appendX().last().setKey(patch.$target).$v, list.appendX().last().setKey(patch.$target).$v);
						}
						break;
					case $PatchInfo::Tactic::kRemove:
						if (exist) {
							if (gApplyToResFile) {
								if constexpr (isResList) {
									removeAllFile(list[ii]);
								} else if constexpr (kTSame<TRR<decltype(list)>, InfoMap<$ResPkg::$RSB::$Info::Group>>) {
									removeAllFile(list.get(ii).$v);
								} else if constexpr (kTSame<TRR<decltype(list)>, InfoMap<$ResPkg::$RSB::$Info::SubGroup>>) {
									removeAllFile(list.get(ii).$k, list.get(ii).$v);
								} else {
									static_assert_failed("err ty");
								}
							}
							listX.eraseOne(i);
							list.eraseOne(ii);
						}
						break;
				}
				return;
			};
			if (!$Str::selectorIsRegex(patch.$target)) {
				Sz i; auto exist = listX.findKey(patch.$target, i);
				Sz ii; Bool exist2;
				if constexpr (isResList)
					exist2 = exist && findRes(listX.get(i).$v.$path, list, ii);
				else
					exist2 = exist && list.findKey(patch.$target, ii);
				if (exist != exist2)
					throw VAMGException("RSBPatchX", "exist != exist2");
				doApply(kBoT, patch.$target, exist, i, ii);
			} else {
				auto i = listX.$sz;
				while (i > kSzN) {
					auto j = --i;
					if ($Str::selectorMatch(patch.$target, listX.get(j).$k)) {
						Sz ii; Bool exist2;
						if constexpr (isResList)
							exist2 = findRes(listX.get(j).$v.$path, list, ii);
						else
							exist2 = list.findKey(listX.get(j).$k, ii);
						if (!exist2)
							throw VAMGException("RSBPatchX", "!exist2");
						doApply(kBoF, listX.get(j).$k, exist2, j, ii);
					}
				}
			}
			return;
		};

		auto cpyRes(Path const & path, Path const & pathOption, $ResPkg::$RSB::$Info::Res & info) {
			Path 真正源文件, 真正目标文件;
			真正目标文件 = path;
			if (!pathOption.isNull())
				真正源文件 = pathOption;
			else
				真正源文件 = path;
			if ($File::existFile(gSrcResDir + 真正源文件)) {
				$File::cpy(gSrcResDir + 真正源文件, gDstResDir + 真正目标文件);
			} else {
				auto 文件类型 = 真正源文件.getExtension();
				auto 存在原始文件 = kBoF;
				if ($Str::cmpIgnoreCase(文件类型, "rton"_r)) {
					Path 原始源文件(Path(真正源文件).eraseExtension().setExtension("json"_r));
					if ($File::existFile(gSrcResDir + 原始源文件)) {
						存在原始文件 = kBoT;
						$RTON::$Encoder::$EnCode::encExport(gSrcResDir + 原始源文件, gDstResDir + 真正目标文件, kBoT, kBoT, kBoT, 64 * kSzBM);
					}
				} else if ($Str::cmpIgnoreCase(文件类型, "ptx"_r)) {
					Path 原始源文件(Path(真正源文件).eraseExtension().setExtension("png"_r));
					if ($File::existFile(gSrcResDir + 原始源文件)) {
						存在原始文件 = kBoT;
						$Image::$Encoder::$EnCode::encExport(gSrcResDir + 原始源文件, gDstResDir + 真正目标文件, info.getAtlasInfo().$sz, "RGBAd32x8888eB"_r);
					}
				} else if ($Str::cmpIgnoreCase(文件类型, "pam"_r)) {
					Path 原始源文件(Path(真正源文件).setExtension("json"_r));
					if ($File::existFile(gSrcResDir + 原始源文件)) {
						存在原始文件 = kBoT;
						$Anim::$PopcapAnim::$Encoder::$EnCode::encExport(gDstResDir + 真正目标文件, gSrcResDir + 原始源文件, 64 * kSzBM);
					}
				}
				if (!存在原始文件)
					throw VAMGException("RSBPatchX::cpyRes()", "file not found : %s", 真正源文件.fullName().$data);
			}
			return;
		}

		auto applyRes(Bool const & reset, Str const & id, $PatchInfo::ResData const & patch, $Info::$RSBInfoX::Res & infoX, $ResPkg::$RSB::$Info::Res & info, Sz const & preAtlasSum) {
			if (reset) {
				if (gApplyToResFile)
					removeAllFile(info);
				infoX.$path.setNull();
				infoX.$type.setNull();
				infoX.setTypeNormal();
				info.$path.setNull();
				info.setTypeNormal();
			}
			if (patch.$path.$state) {
				infoX.$path = patch.$path.$data;
				info.$path = patch.$path.$data;
				if (patch.isAtlasType()) {
					info.$path.setExtension("ptx"_r);
				}
			}
			if (patch.$type.$state) {
				infoX.$type = patch.$type.$data;
			}
			if (patch.isAtlasType()) {
				info.setTypeAtlas();
				infoX.setTypeAtlas();
				if (patch.getAtlasInfo().$fmt.$state) {
					//patch.getAtlasInfo().$fmt.$data;
					info.getAtlasInfo().$fmt = 0;
				}
				if (patch.getAtlasInfo().$sz.$state) {
					infoX.getAtlasInfo().$sz = patch.getAtlasInfo().$sz.$data;
					info.getAtlasInfo().$sz = patch.getAtlasInfo().$sz.$data;
				}
				if (patch.getAtlasInfo().$img.$state) {
					infoX.getAtlasInfo().$img = patch.getAtlasInfo().$img.$data;
				}
			}
			if (patch.$src.$state) {
				cpyRes(info.$path, patch.$src.$data, info);
			}
			return;
		}
		auto applySubGroup(Bool const & reset, Str const & id, $PatchInfo::SubGroupData const & patch, $Info::$RSBInfoX::SubGroup & infoX, $ResPkg::$RSB::$Info::SubGroup & info) {
			if (reset) {
				if (gApplyToResFile)
					removeAllFile(id, info);
				infoX.$category.setNull();
				infoX.$res.alloc().setEmpty();
				info.$category.setNull();
				info.$res.alloc().setEmpty();
				info.$headerType = 4;
				info.$resStoreMethod.set(kBoF, kBoF);
			}
			if (gApplyToResFile) {
				auto packetFile((gDstPacketDir + id).setExtension("rsgp"_r));
				if ($File::existFile(packetFile)) {
					$ResPkg::$RSB::$RSGP::$Packer::$UnPack::unpackExport(packetFile, Path(), gDstResDir);
					$File::remove(packetFile);
				}
			}
			if (patch.$category.$state) {
				infoX.$category = patch.$category.$data;
				info.$category = patch.$category.$data;
			}
			if (patch.$resStoreMethod.$state) {
				info.$resStoreMethod = patch.$resStoreMethod.$data;
			}
			if (patch.$res.$state) {
				for (auto const & e : patch.$res.$data)
					applyShell(applyRes, e, infoX.$res, info.$res);
			}
			return;
		}
		auto applyGroup(Bool const & reset, Str const & id, $PatchInfo::GroupData const & patch, $Info::$RSBInfoX::Group & infoX, $ResPkg::$RSB::$Info::Group & info) {
			if (reset) {
				if (gApplyToResFile)
					removeAllFile(info);
				infoX.$composite = kBoT;
				infoX.$subGroup.alloc().setEmpty();
				info.$composite = kBoT;
				info.$subGroup.alloc().setEmpty();
			}
			if (patch.$composite.$state) {
				infoX.$composite = patch.$composite.$data;
				info.$composite = patch.$composite.$data;
			}
			if (patch.$subGroup.$state) {
				for (auto const & e : patch.$subGroup.$data)
					applyShell(applySubGroup, e, infoX.$subGroup, info.$subGroup);
			}
			return;
		}
		Void apply($PatchInfo::Info const & patch, $Info::$RSBInfoX::Info & infoX, $ResPkg::$RSB::$Info::Info & info, Bool const & applyToResFile, Path const & srcResDir, Path const & dstPacketDir, Path const & dstResDir) {
			gApplyToResFile = applyToResFile;
			gSrcResDir = srcResDir;
			gDstPacketDir = dstPacketDir;
			gDstResDir = dstResDir;
			if (patch.$group.$state) {
				for (auto const & e : patch.$group.$data)
					applyShell(applyGroup, e, infoX.$group, info.$group);
			}
			{
				// remove !program
				for (auto const & e : info.$group) {
					for (auto const & ee : e.$v.$subGroup) {
						for (auto i = ee.$v.$res.$sz; i > kNo1; --i) {
							auto j = i - kNo2;
							if (ee.$v.$res[j].$path.fullName() == "!program"_r)
								ee.$v.$res.eraseOne(j);
						}
					}
				}
			}
			return;
		}

		extern Void applyExport(Path const & patchFile, Path const & srcInfoXFile, Path const & srcInfoFile, Path const & dstInfoXFile, Path const & dstInfoFile, Bool const & applyToResFile, Path const & srcResDir, Path const & dstPacketDir, Path const & dstResDir) {
			$PatchInfo::Info patch;
			$Info::$RSBInfoX::Info infoX;
			$ResPkg::$RSB::$Info::Info info;
			patch.read($JSON::read(patchFile, crv($JSON::Val())));
			infoX.read($JSON::read(srcInfoXFile, crv($JSON::Val())));
			info.read($JSON::read(srcInfoFile, crv($JSON::Val())));
			apply(patch, infoX, info, applyToResFile, srcResDir, dstPacketDir, dstResDir);
			$JSON::write(dstInfoXFile, infoX.write(crv($JSON::Val())));
			$JSON::write(dstInfoFile, info.write(crv($JSON::Val())));
			return;
		}

	}

}
