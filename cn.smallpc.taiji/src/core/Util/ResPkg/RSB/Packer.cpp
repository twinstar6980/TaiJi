#include "./Packer.hpp"

#include "./Info.hpp"
#include "./Struct.hpp"
#include "./RSGP/Info.hpp"
#include "./RSGP/Struct.hpp"
#include "./RSGP/Packer.hpp"
#include "./Common.hpp"

#include "../../../Type/JSON.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "../../File/File.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$Packer {

	namespace $Pack {

		static auto validateRSGP($RSGP::$Info::Info const & info, ByteSection const & data) {
			$RSGP::$Struct::Header header;
			$StaticBlock::read(header, data);
			if (header.$headerType != info.$headerType ||
				ResStoreMethod().parseInt(header.$resStoreMethod) != info.$resStoreMethod)
				return kBoF;
			$RSGP::$Info::Info oriInfo;
			Map<Str, $RSGP::$Struct::ResInfoSlot> resInfo;
			$SpecialStrBlock::dec(resInfo, ByteSection().ref(data, header.$resInfoSectOfs, header.$resInfoSectSz));
			$RSGP::$Packer::$UnPack::parseInfo(oriInfo, header.$headerType, header.$resStoreMethod, resInfo);
			if (info.$res != oriInfo.$res)
				return kBoF;
			return kBoT;
		}

		static Void parseRSGPHeaderToSubGroupInfo(
			$RSGP::$Struct::Header const & rsgpHeader,
			$Struct::SubGroupInfo & subGroupInfo, $Struct::AutoPoolInfo & autoPoolInfo
		) {
			subGroupInfo.$resStoreMethod = rsgpHeader.$resStoreMethod;
			subGroupInfo.$headerAndResInfoSectSz = rsgpHeader.$headerAndResInfoSectSz;
			subGroupInfo.$normalResDataOfs = rsgpHeader.$normalResDataOfs;
			subGroupInfo.$normalResDataOriSz = rsgpHeader.$normalResDataOriSz;
			subGroupInfo.$normalResDataOriSz2 = rsgpHeader.$normalResDataOriSz;
			subGroupInfo.$normalResDataSz = rsgpHeader.$normalResDataSz;
			subGroupInfo.$atlasResDataOfs = rsgpHeader.$atlasResDataOfs;
			subGroupInfo.$atlasResDataOriSz = rsgpHeader.$atlasResDataOriSz;
			subGroupInfo.$atlasResDataSz = rsgpHeader.$atlasResDataSz;
			autoPoolInfo.$atlasResDataOfs = rsgpHeader.$headerAndResInfoSectSz + rsgpHeader.$normalResDataOriSz;
			autoPoolInfo.$atlasResDataSz = rsgpHeader.$atlasResDataOriSz;
			autoPoolInfo.$unk_1 = 1;
			return;
		}

		extern Void pack(ByteSection & pkg, $Info::Info const & info, NullPtr const & embeddedResInfo,
						 Path const & packetDir, Path const & resDir,
						 Path const & groupDir, Path const & groupPacketFile, Path const & groupResDir,
						 Bool const & packFromGroup, Bool const & exportNewPacket, Bool const & useReadyPacket
		) {
			assert(pkg.isEmpty());
			Sz groupSum, subGroupSum, resSum, atlasResSum;
			List<Str> tmpGroupIDList, tmpSubGroupIDList, tmpResPathList;
			info.cntSum(groupSum, subGroupSum, resSum, atlasResSum);
			{
				tmpGroupIDList.alloc(groupSum);
				tmpSubGroupIDList.alloc(subGroupSum);
				tmpResPathList.alloc(resSum);
				for (auto const & group : info.$group) {
					tmpGroupIDList.append(group.$k);
					if (!group.$v.$composite)
						tmpGroupIDList.last().append(kSuffixOfCompositeShell);
					for (auto const & subGroup : group.$v.$subGroup) {
						tmpSubGroupIDList.append(subGroup.$k);
						for (auto const & res : subGroup.$v.$res) {
							tmpResPathList.append(res.$path.fullName($PathDelimiter::kOnWindows));
						}
					}
				}
			}
			{
				$Str::toUpper(tmpGroupIDList).range().sortUp();
				$Str::toUpper(tmpSubGroupIDList).range().sortUp();
				$Str::toUpper(tmpResPathList).range().sortUp();
			}
			$Struct::Header header;
			Map<Str, $Struct::StrIdxInfoSlot> groupIDList, subGroupIDList, resPathList;
			List<$Struct::GroupInfo> groupInfoList;
			List<$Struct::SubGroupInfo> subGroupInfoList;
			List<$Struct::AutoPoolInfo> autoPoolInfoList;
			List<$Struct::AtlasResInfo> atlasResInfoList;
			List<$Struct::AtlasResExpandInfo> atlasResExpandInfoList;
			{
				auto pos = kNo1;
				header.$id = $Struct::kHeaderID;
				header.$headerType = info.$headerType;
				pos += header.kSz;
				if (info.$headerType == 4) {
					// TODO
					pos += kTSz<IU32>;
				}
				{
					header.$resPathListOfs = pos;
					header.$resPathListSz = $SpecialStrBlock::calcMinSz(tmpResPathList) + tmpResPathList.$sz * $Struct::StrIdxInfo::kSz;
					pos += header.$resPathListSz;
				}
				{
					header.$subGroupIDListOfs = pos;
					header.$subGroupIDListSz = $SpecialStrBlock::calcMinSz(tmpSubGroupIDList) + tmpSubGroupIDList.$sz * $Struct::StrIdxInfo::kSz;
					pos += header.$subGroupIDListSz;
				}
				{
					header.$groupInfoListOfs = pos;
					header.$groupInfoListSum = groupSum;
					header.$groupInfoListSz = $Struct::GroupInfo::kSz;
					pos += header.$groupInfoListSz * header.$groupInfoListSum;
				}
				{
					header.$groupIDListOfs = pos;
					header.$groupIDListSz = $SpecialStrBlock::calcMinSz(tmpGroupIDList) + tmpGroupIDList.$sz * $Struct::StrIdxInfo::kSz;
					pos += header.$groupIDListSz;
				}
				{
					header.$subGroupInfoListOfs = pos;
					header.$subGroupInfoListSum = subGroupSum;
					header.$subGroupInfoListSz = $Struct::SubGroupInfo::kSz;
					pos += header.$subGroupInfoListSz * header.$subGroupInfoListSum;
				}
				{
					header.$autoPoolInfoListOfs = pos;
					header.$autoPoolInfoListSum = subGroupSum;
					header.$autoPoolInfoListSz = $Struct::AutoPoolInfo::kSz;
					pos += header.$autoPoolInfoListSz * header.$autoPoolInfoListSum;
				}
				{
					header.$atlasResInfoListOfs = pos;
					header.$atlasResInfoListSum = atlasResSum;
					header.$atlasResInfoListSz = $Struct::AtlasResInfo::kSz;
					if (info.$enableAtlasInfoExpand)
						header.$atlasResInfoListSz += $Struct::AtlasResExpandInfo::kSz;
					pos += header.$atlasResInfoListSz * header.$atlasResInfoListSum;
				}
				{
					// TODO
					header.$resInfoList_Group_Ofs = 0;
					header.$resInfoList_Res_Ofs = 0;
					header.$resInfoList_Str_Ofs = 0;
				}
				pos = calcEnoughSz(pos, kPaddingSz);
				header.$sz = pos;
			}
			{
				tmpGroupIDList.free();
				tmpSubGroupIDList.free();
				tmpResPathList.free();
			}
			{
				groupIDList.alloc(groupSum);
				subGroupIDList.alloc(subGroupSum);
				resPathList.alloc(resSum);
				groupInfoList.alloc(groupSum);
				subGroupInfoList.alloc(subGroupSum);
				autoPoolInfoList.alloc(subGroupSum);
				atlasResInfoList.alloc(atlasResSum);
				if (info.$enableAtlasInfoExpand)
					atlasResExpandInfoList.alloc(atlasResSum);
			}
			Sz groupIdx, subGroupIdx, atlasResIdx;
			{
				groupIdx = kNo1;
				subGroupIdx = kNo1;
				atlasResIdx = kNo1;
			}
			pkg.expandSz(header.$sz);
			for (auto const & group : info.$group) {
				{
					auto & groupID = groupIDList.appendX().last();
					groupID.$k = group.$k;
					if (!group.$v.$composite)
						groupID.$k += kSuffixOfCompositeShell;
					groupID.$v.$idx.$idx = groupIdx;
				}
				auto & groupInfo = groupInfoList.appendX().last();
				{
					if (group.$v.$composite)
						setCID128(group.$k, groupInfo.$id);
					else
						setCID128(group.$k + kSuffixOfCompositeShell, groupInfo.$id);
					groupInfo.$subGroup.alloc(group.$v.$subGroup.$sz);
				}
				for (auto const & subGroup : group.$v.$subGroup) {
					{
						auto & subGroupID = subGroupIDList.appendX().last();
						subGroupID.$k = subGroup.$k;
						subGroupID.$v.$idx.$idx = subGroupIdx;
					}
					{
						auto & subGroupInfoItem = groupInfo.$subGroup.appendX().last();
						subGroupInfoItem.$idx = subGroupIdx;
						subGroup.$v.$category.parseTo(subGroupInfoItem.$res, subGroupInfoItem.$loc);
					}
					auto & subGroupInfo = subGroupInfoList.appendX().last();
					{
						setCID128(subGroup.$k, subGroupInfo.$id);
						subGroupInfo.$idx = subGroupIdx;
					}
					auto & autoPoolInfo = autoPoolInfoList.appendX().last();
					{
						setCID128(subGroup.$k + kSuffixOfAutoPool, autoPoolInfo.$id);
					}
					{
						subGroupInfo.$atlasResIdxBeg = atlasResIdx;
						subGroupInfo.$atlasResSum = kSzN;
					}
					for (auto const & res : subGroup.$v.$res) {
						{
							auto & resPath = resPathList.appendX().last();
							resPath.$k = res.$path.fullName($PathDelimiter::kOnWindows);
							resPath.$v.$idx.$idx = subGroupIdx;
						}
						if (res.isAtlasType()) {
							auto & atlasResInfo = atlasResInfoList.appendX().last();
							{
								atlasResInfo.$fmt = res.getAtlasInfo().$fmt;
								atlasResInfo.$w = res.getAtlasInfo().$sz.$w;
								atlasResInfo.$h = res.getAtlasInfo().$sz.$h;
								switch (atlasResInfo.$fmt) { // TODO
									default:
										atlasResInfo.$unk_w4 = res.getAtlasInfo().$sz.$w * 4;
										break;
									case 0x1E:
										atlasResInfo.$unk_w4 = res.getAtlasInfo().$sz.$w / 2;
										break;
								}
							}
							if (info.$enableAtlasInfoExpand) {
								auto & atlasResInfoAppend = atlasResExpandInfoList.appendX().last();
								switch (res.getAtlasInfo().$fmt) {
									default:
										atlasResInfoAppend.$alphaSz = kSzN;
										break;
									case 0x93:
									case 0x96:
										atlasResInfoAppend.$alphaSz = $File::getFileSize(packFromGroup ? (groupDir + group.$k + subGroup.$k + groupResDir + res.$path) : (resDir + res.$path)) - (res.getAtlasInfo().$sz.calcSz() / 2);
										break;
								}
								atlasResInfoAppend.$unk_100 = 100;
							}
							++subGroupInfo.$atlasResSum;
							++atlasResIdx;
						}
					}
					{
						$RSGP::$Struct::Header rsgpHeader;
						subGroupInfo.$ofs = pkg.$sz;
						ByteSection curRSGPPkg;
						curRSGPPkg.ref(pkg, pkg.$sz, kSzN, pkg.getDiffOfFull());
						$RSGP::$Info::Info curRsgpInfo;
						subGroup.$v.toRSGPInfo(curRsgpInfo);
						if (packFromGroup) {
							Str groupID;
							cnvStdGroupID(groupInfo.$id, groupID);
							Path parentDir; $File::getCurDirThenChgDir(parentDir, groupDir + groupID + subGroup.$k);
							groupID.free();
							auto repack = kBoT;
							if (useReadyPacket && $File::existFile(groupPacketFile)) {
								$File::readFile(groupPacketFile, curRSGPPkg);
								repack = !validateRSGP(curRsgpInfo, curRSGPPkg);
								if (repack) {
									log.war("found packet, but info diff, repack \"%s\" ...", subGroup.$k.$data);
								}
							}
							if (repack) {
								$RSGP::$Packer::$Pack::pack(curRSGPPkg, curRsgpInfo, groupResDir);
								if (exportNewPacket) {
									$File::writeFile(groupPacketFile, curRSGPPkg);
								}
							}
							$File::chgDirThenFree(parentDir);
						} else {
							auto rsgpPacketPath((packetDir + subGroup.$k).setExtension("rsgp"_r));
							auto repack = kBoT;
							if (useReadyPacket && $File::existFile(rsgpPacketPath)) {
								$File::readFile(rsgpPacketPath, curRSGPPkg);
								repack = !validateRSGP(curRsgpInfo, curRSGPPkg);
								if (repack) {
									log.war("found packet, but info diff, repack \"%s\" ...", subGroup.$k.$data);
								}
							}
							if (repack) {
								$RSGP::$Packer::$Pack::pack(curRSGPPkg, curRsgpInfo, resDir);
								if (exportNewPacket) {
									$File::writeFile(rsgpPacketPath, curRSGPPkg);
								}
							}
						}
						subGroupInfo.$sz = curRSGPPkg.$sz;
						pkg.expandSz(curRSGPPkg.$sz);
						curRSGPPkg.unref();
						$StaticBlock::read(rsgpHeader, ByteSection().ref(pkg, subGroupInfo.$ofs, rsgpHeader.kSz));
						parseRSGPHeaderToSubGroupInfo(rsgpHeader, subGroupInfo, autoPoolInfo);
					}
					++subGroupIdx;
				}
				++groupIdx;
			}
			{
				{
					$Str::mapKeyUpper(groupIDList).sortUpByKey();
					$Str::mapKeyUpper(subGroupIDList).sortUpByKey();
					$Str::mapKeyUpper(resPathList).sortUpByKey();
					$SpecialStrBlock::enc(groupIDList, ByteSection().ref(pkg, header.$groupIDListOfs, kSzN, header.$groupIDListSz));
					$SpecialStrBlock::enc(subGroupIDList, ByteSection().ref(pkg, header.$subGroupIDListOfs, kSzN, header.$subGroupIDListSz));
					$SpecialStrBlock::enc(resPathList, ByteSection().ref(pkg, header.$resPathListOfs, kSzN, header.$resPathListSz));
				}
				{
					$DynamicBlock::write(groupInfoList, ByteSection().ref(pkg, header.$groupInfoListOfs, header.$groupInfoListSum * header.$groupInfoListSz));
					$StaticBlock::write(subGroupInfoList, ByteSection().ref(pkg, header.$subGroupInfoListOfs, header.$subGroupInfoListSum * header.$subGroupInfoListSz));
					$StaticBlock::write(autoPoolInfoList, ByteSection().ref(pkg, header.$autoPoolInfoListOfs, header.$autoPoolInfoListSum * header.$autoPoolInfoListSz));
					$StaticBlock::write(atlasResInfoList, ByteSection().ref(pkg, header.$atlasResInfoListOfs, header.$atlasResInfoListSum * header.$atlasResInfoListSz), header.$atlasResInfoListSz);
					if (info.$enableAtlasInfoExpand)
						$StaticBlock::write(atlasResExpandInfoList, ByteSection().ref(pkg, header.$atlasResInfoListOfs + $Struct::AtlasResInfo::kSz, header.$atlasResInfoListSum * header.$atlasResInfoListSz - $Struct::AtlasResInfo::kSz), header.$atlasResInfoListSz);
				}
				{
					$StaticBlock::write(header, ByteSection().ref(pkg, kNo1, header.kSz));
					if (info.$headerType == 4U) {
						 // TODO
						*reinterpret_cast<IU32 *>(pkg.$data + header.kSz) = header.$sz;
					}
				}
			}
			{
				groupIDList.free();
				subGroupIDList.free();
				resPathList.free();
				groupInfoList.free();
				subGroupInfoList.free();
				autoPoolInfoList.free();
				atlasResInfoList.free();
				atlasResExpandInfoList.free();
			}
			return;
		}

		extern Void packExport(Path const & pkgFile, Path const & infoFile, Path const & embeddedResInfoFile,
							   Path const & packetDir, Path const & resDir,
							   Path const & groupDir, Path const & groupPacketFile, Path const & groupResDir,
							   Bool const & packFromGroup, Bool const & exportNewPacket, Bool const & useReadyPacket, Sz const & memSz
		) {
			ByteSection pkg;
			$Info::Info info;
			NullPtr embeddedResInfo;
			pkg.alloc(memSz);
			info.read($JSON::read(infoFile, crv($JSON::Val())));
			pack(pkg, info, embeddedResInfo, packetDir, resDir, groupDir, groupPacketFile, groupResDir, packFromGroup, exportNewPacket, useReadyPacket);
			$File::writeFile(pkgFile, pkg);
			pkg.free();
			info.free();
			return;
		}

		/*
		Void PackToPart(
			Path const & partDir, $PartInfo::PartInfoList const & partInfoList,
			$StructInfo::StructInfo const & structInfo,
			Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket,
			Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir,
			Bool const & cipherTextUpperCase, SzI const & memSz
		) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			for_criter(partInfo, partInfoList) {
				$StructInfo::StructInfo partStructInfo;
				{
					partStructInfo.$headerType = structInfo.$headerType;
					partStructInfo.$setResInfoListInHeader = structInfo.$setResInfoListInHeader;
					partStructInfo.$useMoreAtlasInfo = structInfo.$useMoreAtlasInfo;
					for_criter(groupID, partInfo.$group)
						for_criter(group, structInfo.$group)
						if (groupID == group.$key) {
							partStructInfo.$group.addByKV(group.$key, group.$val);
							break;
						}
				}
				Pack(data, sz, partStructInfo, packFromGroup, packRSGPByMainInfo, outputNewPacket, useReadyPacket, resDir, packetDir, packetInfoDir, groupDir, cipherTextUpperCase);
				$DevUtil::WriteData(Path(partDir).add(partInfo.$id), data, sz);
			}
			delPtrArr(data);
			return;
		}*/

	}

	namespace $UnPack {

		static Void parseRSGPData(ByteSection const & data, List<$Struct::SubGroupInfo> const & subGroupInfo, List<$RSGP::$Struct::Header> & rsgpHeader, List<Map<Str, $RSGP::$Struct::ResInfoSlot>> & rsgpResInfo) {
			rsgpHeader.alloc(subGroupInfo.$sz);
			rsgpResInfo.alloc(subGroupInfo.$sz);
			for (auto const & e : subGroupInfo) {
				$RSGP::$Packer::$UnPack::parseData(ByteSection().ref(data, e.$ofs, e.$sz), rsgpHeader.appendX().last(), rsgpResInfo.appendX().last());
			}
			return;
		}
		extern Void parseData(ByteSection const & data, $Struct::Header & header,
							  Map<Str, $Struct::StrIdxInfoSlot> & groupID,
							  Map<Str, $Struct::StrIdxInfoSlot> & subGroupID,
							  Map<Str, $Struct::StrIdxInfoSlot> & resPath,
							  List<$Struct::GroupInfo> & groupInfo,
							  List<$Struct::SubGroupInfo> & subGroupInfo,
							  List<$Struct::AutoPoolInfo> & autoPoolInfo,
							  List<$Struct::AtlasResInfo> & atlasResInfo,
							  List<$Struct::AtlasResExpandInfo> & atlasResExpandInfo,
							  List<$RSGP::$Struct::Header> & rsgpHeader,
							  List<Map<Str, $RSGP::$Struct::ResInfoSlot>> & rsgpResInfo,
							  NullPtr const & embeddedResInfo
		) {
			$StaticBlock::read(header, data);
			{
				groupInfo.allocThenUse(header.$groupInfoListSum);
				subGroupInfo.allocThenUse(header.$subGroupInfoListSum);
				autoPoolInfo.allocThenUse(header.$autoPoolInfoListSum);
				atlasResInfo.allocThenUse(header.$atlasResInfoListSum);
			}
			{
				$SpecialStrBlock::dec(groupID, ByteSection().ref(data, header.$groupIDListOfs, header.$groupIDListSz));
				$SpecialStrBlock::dec(subGroupID, ByteSection().ref(data, header.$subGroupIDListOfs, header.$subGroupIDListSz));
				$SpecialStrBlock::dec(resPath, ByteSection().ref(data, header.$resPathListOfs, header.$resPathListSz));
			}
			{
				$DynamicBlock::read(groupInfo, ByteSection().ref(data, header.$groupInfoListOfs, header.$groupInfoListSum * header.$groupInfoListSz));
				$StaticBlock::read(subGroupInfo, ByteSection().ref(data, header.$subGroupInfoListOfs, header.$subGroupInfoListSum * header.$subGroupInfoListSz), header.$subGroupInfoListSz);
				$StaticBlock::read(autoPoolInfo, ByteSection().ref(data, header.$autoPoolInfoListOfs, header.$autoPoolInfoListSum * header.$autoPoolInfoListSz), header.$autoPoolInfoListSz);
				$StaticBlock::read(atlasResInfo, ByteSection().ref(data, header.$atlasResInfoListOfs, header.$atlasResInfoListSum * header.$atlasResInfoListSz), header.$atlasResInfoListSz);
				if (header.$atlasResInfoListSz == $Struct::AtlasResInfo::kSz) {
					atlasResExpandInfo.setNull();
				} else if (header.$atlasResInfoListSz == $Struct::AtlasResInfo::kSz + $Struct::AtlasResExpandInfo::kSz) {
					atlasResExpandInfo.allocThenUse(header.$atlasResInfoListSum);
					$StaticBlock::read(atlasResExpandInfo, ByteSection().ref(data, header.$atlasResInfoListOfs + $Struct::AtlasResInfo::kSz, header.$atlasResInfoListSum * header.$atlasResInfoListSz - $Struct::AtlasResInfo::kSz), header.$atlasResInfoListSz);
				} else {
					throw VAMGException("RSBUnpacker", "atlas-res-info size error : 0x%X", header.$atlasResInfoListSz);
				}
			}
			{
				if (header.$headerType > 4)
					log.war("headerType > 4");
				if (header.$resInfoList_Group_Ofs != 0)
					;// ReadResInfoList(data + header._resInfoList_Group_Ofs, header._resInfoList_Res_Ofs - header._resInfoList_Group_Ofs, data + header._resInfoList_Res_Ofs, data + header._resInfoList_Str_Ofs, resInfoList);
			}
			parseRSGPData(data, subGroupInfo, rsgpHeader, rsgpResInfo);
			return;
		}

		static Void parseRSGPInfo($Info::SubGroup & info,
								  Sz const & res, FourCC const & loc,
								  Sz const & headerType, IU32 const & resStoreMethod,
								  Map<Str, $RSGP::$Struct::ResInfoSlot> const & resInfo,
								  List<$Struct::AtlasResInfo> const & atlasResInfo, Sz const & atlasResInfoIdxBeg
		) {
			info.$category.parse(res, loc);
			info.$headerType = headerType;
			info.$resStoreMethod.parseInt(resStoreMethod);
			info.$res.alloc(resInfo.$sz);
			for (auto const e : resInfo) {
				auto & res = info.$res.appendX().last();
				res.$path.parse(e.$k);
				switch (e.$v.$data.$type) {
					default:
						throw VAMGException("RSBUnpacker", "unknown res-type : %u", e.$v.$data.$type);
						break;
					case $RSGP::$Struct::$ResTypeIdx::kNormal:
						res.setTypeNormal();
						break;
					case $RSGP::$Struct::$ResTypeIdx::kAtlas:
						res.setTypeAtlas();
						res.getAtlasInfo().$sz.set(e.$v.$atlas.$w, e.$v.$atlas.$h);
						res.getAtlasInfo().$fmt = atlasResInfo[atlasResInfoIdxBeg + e.$v.$atlas.$idx].$fmt;
						break;
				}
			}
			return;
		}
		extern Void parseInfo($Info::Info & info, Sz const & headerType,
							  Bool const & enableEmbeddedResInfo, Bool const & enableAtlasInfoExpand,
							  List<$Struct::GroupInfo> const & groupInfoList,
							  List<$Struct::SubGroupInfo> const & subGroupInfoList,
							  Map<Str, $Struct::StrIdxInfoSlot> const & resPathList,
							  List<$Struct::AtlasResInfo> const & atlasResInfoList,
							  List<$RSGP::$Struct::Header> const & rsgpHeaderList,
							  List<Map<Str, $RSGP::$Struct::ResInfoSlot>> const & rsgpResInfoList
		) {
			info.$headerType = headerType;
			info.$enableEmbeddedResInfo = enableEmbeddedResInfo;
			info.$enableAtlasInfoExpand = enableAtlasInfoExpand;
			info.$group.alloc(groupInfoList.$sz);
			for (auto const groupInfo : groupInfoList) {
				auto & group = info.$group.appendX().last();
				group.$v.$composite = !cnvStdGroupID(groupInfo.$id, group.$k);
				group.$v.$subGroup.alloc(groupInfo.$subGroup.$sz);
				for (auto const & subGroupItem : groupInfo.$subGroup) {
					auto & subGroup = group.$v.$subGroup.appendX().last();
					cpyCStr(subGroup.$k, subGroupInfoList[subGroupItem.$idx].$id);
					parseRSGPInfo(subGroup.$v, subGroupItem.$res, subGroupItem.$loc, rsgpHeaderList[subGroupItem.$idx].$headerType, rsgpHeaderList[subGroupItem.$idx].$resStoreMethod, rsgpResInfoList[subGroupItem.$idx], atlasResInfoList, subGroupInfoList[subGroupItem.$idx].$atlasResIdxBeg);
				}
			}
			return;
		}

		static auto exportRes(ByteSection const & data,
							  List<$Struct::GroupInfo> const & groupInfoList,
							  List<$Struct::SubGroupInfo> const & subGroupInfoList,
							  List<$RSGP::$Struct::Header> const & rsgpHeader,
							  List<Map<Str, $RSGP::$Struct::ResInfoSlot>> const & rsgpResInfo,
							  Path const & packetDir, Path const & packetInfoDir, Path const & resDir,
							  Path const & groupDir, Path const & groupPacketFile, Path const & groupPacketInfoFile, Path const & groupResDir
		) {
			for (auto const & groupInfo : groupInfoList) {
				for (auto const & subGroupInfoItem : groupInfo.$subGroup) {
					auto const & subGroupInfo = subGroupInfoList[subGroupInfoItem.$idx];
					ByteSection curRSGPPkg;
					curRSGPPkg.ref(data, subGroupInfo.$ofs, subGroupInfo.$sz);
					auto & curRSGPHeader = rsgpHeader[subGroupInfoItem.$idx];
					auto & curRSGPResInfo = rsgpResInfo[subGroupInfoItem.$idx];
					Str curSubGroupID;
					refCStr(curSubGroupID, subGroupInfo.$id);
					$RSGP::$Info::Info rsgpInfo;
					$RSGP::$Packer::$UnPack::parseInfo(rsgpInfo, curRSGPHeader.$headerType, curRSGPHeader.$resStoreMethod, curRSGPResInfo);
					if (!packetDir.isNull()) {
						$File::writeFile((packetDir + curSubGroupID).setExtension("rsgp"_r), ByteSection().ref(data, subGroupInfo.$ofs, subGroupInfo.$sz));
					}
					if (!packetInfoDir.isNull()) {
						$JSON::write((packetInfoDir + curSubGroupID).setExtension("json"_r), rsgpInfo.write(crv($JSON::Val())));
					}
					if (!resDir.isNull()) {
						$RSGP::$Packer::$UnPack::exportResEntry(curRSGPPkg, ResStoreMethod().parseInt(curRSGPHeader.$resStoreMethod),
																curRSGPHeader.$normalResDataOfs, curRSGPHeader.$normalResDataOriSz, curRSGPHeader.$normalResDataSz,
																curRSGPHeader.$atlasResDataOfs, curRSGPHeader.$atlasResDataOriSz, curRSGPHeader.$atlasResDataSz,
																curRSGPResInfo, resDir);
					}
					if (!groupDir.isNull()) {
						Str groupID;
						cnvStdGroupID(groupInfo.$id, groupID);
						Path parentDir; $File::getCurDirThenChgDirForce(parentDir, groupDir + groupID + curSubGroupID);
						groupID.free();
						if (!groupPacketFile.isNull()) {
							$File::writeFile(groupPacketFile, curRSGPPkg);
						}
						if (!groupPacketInfoFile.isNull()) {
							$JSON::write(groupPacketInfoFile, rsgpInfo.write(crv($JSON::Val())));
						}
						if (!groupResDir.isNull()) {
							$RSGP::$Packer::$UnPack::exportResEntry(curRSGPPkg, ResStoreMethod().parseInt(curRSGPHeader.$resStoreMethod),
																	curRSGPHeader.$normalResDataOfs, curRSGPHeader.$normalResDataOriSz, curRSGPHeader.$normalResDataSz,
																	curRSGPHeader.$atlasResDataOfs, curRSGPHeader.$atlasResDataOriSz, curRSGPHeader.$atlasResDataSz,
																	curRSGPResInfo, groupResDir);
						}
						$File::chgDirThenFree(parentDir);
					}
					curRSGPPkg.unref();

				}
			}
			return;
		}

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, NullPtr const & embeddedResInfo,
						   Path const & packetDir, Path const & packetInfoDir, Path const & resDir,
						   Path const & groupDir, Path const & groupPacketFile, Path const & groupPacketInfoFile, Path const & groupResDir
		) {
			$Struct::Header header;
			Map<Str, $Struct::StrIdxInfoSlot> groupID, subGroupID, resPath;
			List<$Struct::GroupInfo> groupInfo;
			List<$Struct::SubGroupInfo> subGroupInfo;
			List<$Struct::AutoPoolInfo> autoPoolInfo;
			List<$Struct::AtlasResInfo> atlasResInfo;
			List<$Struct::AtlasResExpandInfo> atlasResExpandInfo;
			List<$RSGP::$Struct::Header> rsgpHeader;
			List<Map<Str, $RSGP::$Struct::ResInfoSlot>> rsgpResInfo;
			{
				parseData(pkg, header, groupID, subGroupID, resPath, groupInfo, subGroupInfo, autoPoolInfo, atlasResInfo, atlasResExpandInfo, rsgpHeader, rsgpResInfo, embeddedResInfo);
				parseInfo(info, header.$headerType, kBoF, !atlasResExpandInfo.isNull(), groupInfo, subGroupInfo, resPath, atlasResInfo, rsgpHeader, rsgpResInfo);
				exportRes(pkg, groupInfo, subGroupInfo, rsgpHeader, rsgpResInfo, packetDir, packetInfoDir, resDir, groupDir, groupPacketFile, groupPacketInfoFile, groupResDir);
			}
			{
				groupID.free();
				subGroupID.free();
				resPath.free();
				groupInfo.free();
				subGroupInfo.free();
				autoPoolInfo.free();
				atlasResInfo.free();
				atlasResExpandInfo.free();
				rsgpHeader.free();
				rsgpResInfo.free();
			}
			return;
		}

		extern Void unpackExport(Path const & pkgFile, Path const & infoFile, Path const & embeddedResInfoFile,
								 Path const & packetDir, Path const & packetInfoDir, Path const & resDir,
								 Path const & groupDir, Path const & groupPacketFile, Path const & groupPacketInfoFile, Path const & groupResDir
		) {
			ByteSection pkg;
			$Info::Info info;
			NullPtr embeddedResInfo;
			$File::readFileForce(pkgFile, pkg);
			unpack(pkg, info, embeddedResInfo, packetDir, packetInfoDir, resDir, groupDir, groupPacketFile, groupPacketInfoFile, groupResDir);
			if (!infoFile.isNull())
				$JSON::write(infoFile, info.write(crv($JSON::Val())));
			if (!embeddedResInfoFile.isNull())
				;// $JSON::write(embeddedResInfoFile, embeddedResInfo.write(crv($JSON::Val())));
			pkg.free();
			info.free();
			return;
		}

		/*
		static auto ReadResInfoList(
			Byte const * const & groupInfoData, SzI const & groupInfoDataSz,
			Byte const * const & resInfoData, Byte const * const & strList, $ResInfoList::ResInfoList & dst
		) {
			dst.$group.setNull();
			ListOfBlock<$Struct::ResList_Group> groupInfoList;
			for (SzI groupPos = kThFirst; groupPos < groupInfoDataSz; ) {
				auto & groupInfo = groupInfoList.appendX().last();
				groupInfo.read(groupInfoData + groupPos);
				groupPos += groupInfo.getSz();
				auto & groupItem = dst.$group.appendX().last();
				groupItem.$id.cpy(reinterpret_cast<Ch const *>(strList + groupInfo._info._id_Ofs));
				groupItem._unKnown = groupInfo._info._unKnown;
				groupItem.$subGroup.alloc(groupInfo._info._subGroup_Sum);
				for_criter(subGroup, groupInfo.$subGroup) {
					auto & subGroupItem = groupItem.$subGroup.appendX().last();
					subGroupItem.$id.cpy(reinterpret_cast<Ch const *>(strList + subGroup._info._id_Ofs));
					subGroupItem.$category.$res = subGroup._info._resID;
					$Struct::GetLocID(subGroupItem.$category.$loc, subGroup._info._locID);
					subGroupItem.$res.alloc(subGroup._info._res_Sum);
					ListOfBlock<$Struct::ResList_Res> resInfoList;
					resInfoList.setNull();
					for_criter(res, subGroup.$res) {
						auto & resInfo = resInfoList.appendX().last();
						resInfo.read(resInfoData + res._info_Ofs, res._info_Ofs);
						auto & resItem = subGroupItem.$res.appendX().last();
						resItem.$id.cpy(reinterpret_cast<Ch const *>(strList + resInfo._info._id_Ofs));
						resItem._path.read(reinterpret_cast<Ch const *>(strList + resInfo._info._path_Ofs));
						if (resInfo._info.$type < 8u)
							resItem.$type.cpy($Struct::$ResList_Res_Base::kTypeStr[resInfo._info.$type]);
						else resItem.$type.setNull();
						resItem._existExpand = (resInfo._info._expandMember_Ofs != kThNone);
						if (resItem._existExpand) {
							resItem._expand._unKnown_A = resInfo._expandMember._unKnown_A;
							resItem._expand._unKnown_C = resInfo._expandMember._unKnown_C;
							resItem._expand._unKnown_D = resInfo._expandMember._unKnown_D;
							resItem._expand._flag = resInfo._expandMember._flag;
							resItem._expand._pos.set(resInfo._expandMember.$x, resInfo._expandMember.$y);
							resItem._expand._sz.set(resInfo._expandMember._w, resInfo._expandMember._h);
							resItem._expand._padding.set(resInfo._expandMember._paddingX, resInfo._expandMember._paddingY);
							if (resInfo._expandMember._parent_Ofs != kThNone)
								resItem._expand.$parent.cpy(reinterpret_cast<Ch const *>(strList + resInfo._expandMember._parent_Ofs));
							else resItem._expand.$parent.setNull();
						}
						resItem.$expand.setNull();
						for_criter(appendMember, resInfo._appendMember) {
							auto & appendItem = resItem.$expand.appendX().last();
							appendItem._key.cpy(reinterpret_cast<Ch const *>(strList + appendMember._keyOfs));
							appendItem._val.cpy(reinterpret_cast<Ch const *>(strList + appendMember._valOfs));
							appendItem._unKnown = appendMember._unKnown;
						}
					}
					resInfoList.setNull();
				}
			}
			return;
		}
	*/

			/*
			Void UnPackFromPart(
				Path const & partDir, Path const & partIDListFile, Path const & partInfoListFile,
				Path const & structInfoFile, Path const & resInfoListFile,
				Bool const & outputRes, Path const & resDir,
				Bool const & outputPacket, Path const & packetDir,
				Bool const & outputPacketInfo, Path const & packetInfoDir,
				Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & groupDir
			) {
				StrList partIDList;
				$PartInfo::PartInfoList partInfoList;
				$StructInfo::StructInfo fullStructInfo;
				$ResInfoList::ResInfoList fullResInfoList;
				$JSON::getArrStr($JSON::read(partIDListFile, $JSON::Arr().setNull()), partIDList);
				auto isFirstPart = kTrue;
				auto existResInfoList = kFalse;
				for_criter(PartID, partIDList) {
					Byte * data(kNullPtr); SzI sz;
					$DevUtil::ForceReadData(Path(partDir).add(PartID), data, sz);
					$Struct::$Header::Member header;
					CipherText<$Struct::StrIdxInfoSlot> groupIDList, subGroupIDList, resPathList;
					ListOfBlock<$Struct::$GroupInfo::GroupInfo> groupInfoList;
					ListOfBlock<$Struct::$SubGroupInfo::Member> subGroupInfoList;
					ListOfBlock<$Struct::$AutoPoolInfo::Member> autoPoolInfoList;
					ListOfBlock<$Struct::$AtlasResInfo::Member> atlasResInfoList;
					ListOfBlock<$Struct::$AtlasResInfo_Append::Member> atlasResInfoAppendList;
					$ResInfoList::ResInfoList resInfoList;
					ListOfBlock<$RSGPUtil::$Struct::$Header::Member> rsgpHeaderList;
					ListP<CipherText<$RSGPUtil::$Struct::ResInfoSlot>> rsgpResInfoList;
					{
						Bool setResInfoListInHeader, useMoreAtlasInfo;
						Analysis(data, setResInfoListInHeader, useMoreAtlasInfo, header, groupIDList, subGroupIDList, resPathList, groupInfoList, subGroupInfoList, autoPoolInfoList, atlasResInfoList, atlasResInfoAppendList, rsgpHeaderList, rsgpResInfoList, resInfoList);
						$StructInfo::StructInfo structInfo;
						ParseInfo(structInfo, header._headerType, setResInfoListInHeader, useMoreAtlasInfo, groupInfoList, subGroupInfoList, resPathList, atlasResInfoList, rsgpHeaderList, rsgpResInfoList);
						if (isFirstPart) {
							fullStructInfo._headerType = structInfo._headerType;
							fullStructInfo._setResInfoListInHeader = structInfo._setResInfoListInHeader;
							fullStructInfo._useMoreAtlasInfo = structInfo._useMoreAtlasInfo;
							isFirstPart = kFalse;
						} else {
							if (fullStructInfo._headerType != structInfo._headerType)
								log.err("Diff headerType of This Part");
							if (fullStructInfo._setResInfoListInHeader != structInfo._setResInfoListInHeader)
								log.err("Diff setResInfoListInHeader of This Part");
							if (fullStructInfo._useMoreAtlasInfo != structInfo._useMoreAtlasInfo)
								log.err("Diff useMoreAtlasInfo of This Part");
						}
						fullStructInfo.$group.add(structInfo.$group);
						{
							auto & partInfo = partInfoList.appendX().last();
							partInfo.$id = PartID;
							for_criter(group, structInfo.$group)
								partInfo.$group.add(group._key);
						}
						if (setResInfoListInHeader) {
							existResInfoList = kTrue;
							fullResInfoList.$group.add(resInfoList.$group);
						}
						OutPutResFile(data, groupInfoList, subGroupInfoList, rsgpHeaderList, rsgpResInfoList, outputRes, resDir, outputPacket, packetDir, outputPacketInfo, packetInfoDir, outputGroup, outputGroupRes, outputGroupPacket, groupDir);
					}
					{
						groupIDList.tfree();
						subGroupIDList.tfree();
						resPathList.tfree();
						groupInfoList.tfree();
						subGroupInfoList.tfree();
						autoPoolInfoList.tfree();
						atlasResInfoList.tfree();
						rsgpHeaderList.tfree();
						rsgpResInfoList.tfree();
					}
					delPtrArr(data);
				}
				$JSON::write(structInfoFile, fullStructInfo.write($JSON::Obj().setNull()));
				if (existResInfoList)
					$JSON::write(resInfoListFile, fullResInfoList.write($JSON::Obj().setNull()));
				$JSON::write(partInfoListFile, partInfoList.write($JSON::Arr().setNull()));
				return;
			}*/

	}

}
