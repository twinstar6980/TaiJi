# include "./Packer.h"

# include "./StructInfo.h"
# include "./Struct.h"
# include "../RSGPUtil/StructInfo.h"
# include "../RSGPUtil/Struct.h"
# include "../RSGPUtil/Packer.h"
# include "./CipherText.h"

# include "../../UtilKit/MemoryUtil.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../RSGPUtil/Packer_Export.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$Packer {
	namespace $Pack {
		Void ParseRSGPHeaderToSubGroupInfo(
			$RSGPUtil::$Struct::$Header::Member const & rsgpHeader,
			$Struct::$SubGroupInfo::Member & subGroupInfo, $Struct::$AutoPoolInfo::Member & autoPoolInfo
		) {
			subGroupInfo._rsgpResDataCompressMethod = rsgpHeader._resDataCompressMethod;
			subGroupInfo._rsgpHeaderAndResInfoListSize = rsgpHeader._headerAndResInfoListSize;
			subGroupInfo._rsgpNormalResData_Ofs = rsgpHeader._normalResData_Ofs;
			subGroupInfo._rsgpNormalResData_InitSize = rsgpHeader._normalResData_InitSize;
			subGroupInfo._rsgpNormalResData_InitSize_2 = rsgpHeader._normalResData_InitSize;
			subGroupInfo._rsgpNormalResData_Size = rsgpHeader._normalResData_Size;
			subGroupInfo._rsgpAtlasResData_Ofs = rsgpHeader._atlasResData_Ofs;
			subGroupInfo._rsgpAtlasResData_InitSize = rsgpHeader._atlasResData_InitSize;
			subGroupInfo._rsgpAtlasResData_Size = rsgpHeader._atlasResData_Size;
			autoPoolInfo._atlasResData_Ofs = rsgpHeader._headerAndResInfoListSize + rsgpHeader._normalResData_InitSize;
			autoPoolInfo._atlasResData_Size = rsgpHeader._atlasResData_InitSize;
			autoPoolInfo._unKnown_1 = $Struct::$AutoPoolInfo::$MemberConst::kUnKnown_1;
			return;
		}
		Void Pack(
			Byte * const & data, SzI & sz,
			$StructInfo::StructInfo const & structInfo,
			Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket,
			Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir,
			Bool const & cipherTextUpperCase
		) {
			SzI groupSum, subGroupSum, resSum, atlasResSum;
			StrList tmpGroupIDList, tmpSubGroupIDList, tmpResPathList;
			structInfo.CntSum(groupSum, subGroupSum, resSum, atlasResSum);
			{
				tmpGroupIDList.alloc(groupSum);
				tmpSubGroupIDList.alloc(subGroupSum);
				tmpResPathList.alloc(resSum);
				for_criter(group, structInfo._group) {
					tmpGroupIDList.add(group._key);
					if (!group._val._isComposite)
						tmpGroupIDList.getLast().cat($Struct::kSuffixOfCompositeShell);
					for_criter(subGroup, group._val._subGroup) {
						tmpSubGroupIDList.add(subGroup._key);
						for_criter(res, subGroup._val._res)
							tmpResPathList.add(res._path.getFullPath(Str().getThis(), $PathDelimiter::kWindowsStyle));
					}
				}
			}
			{
				if (cipherTextUpperCase) {
					tmpGroupIDList.upperCase();
					tmpSubGroupIDList.upperCase();
					tmpResPathList.upperCase();
				}
				tmpGroupIDList.sortUp();
				tmpSubGroupIDList.sortUp();
				tmpResPathList.sortUp();
			}
			$Struct::$Header::Member header;
			CipherText<$Struct::StrIdxInfoSlot> groupIDList, subGroupIDList, resPathList;
			ListOfBlock<$Struct::$GroupInfo::GroupInfo> groupInfoList;
			ListOfBlock<$Struct::$SubGroupInfo::Member> subGroupInfoList;
			ListOfBlock<$Struct::$AutoPoolInfo::Member> autoPoolInfoList;
			ListOfBlock<$Struct::$AtlasResInfo::Member> atlasResInfoList;
			ListOfBlock<$Struct::$AtlasResInfo_Append::Member> atlasResInfoAppendList;
			{
				sz = kSzZero;
				sz += header.getSz();
				if (structInfo._headerType == 4) // TODO
					sz += szOf<UI32>();
				header._id = $Struct::$Header::$MemberConst::kID;
				header._headerType = structInfo._headerType;
				{
					header._resPathList_Ofs = sz;
					header._resPathList_Size = CalcCipherTextMinSize(tmpResPathList) + ($Struct::$StrIdxInfo::kBlockSize * tmpResPathList._sz);
					sz += header._resPathList_Size;
				}
				{
					header._subGroupIDList_Ofs = sz;
					header._subGroupIDList_Size = CalcCipherTextMinSize(tmpSubGroupIDList) + ($Struct::$StrIdxInfo::kBlockSize * tmpSubGroupIDList._sz);
					sz += header._subGroupIDList_Size;
				}
				{
					header._groupInfoList_Ofs = sz;
					header._groupInfoList_Sum = groupSum;
					header._groupInfoList_Size = $Struct::$GroupInfo::kBlockSize;
					sz += (header._groupInfoList_Size * header._groupInfoList_Sum);
				}
				{
					header._groupIDList_Ofs = sz;
					header._groupIDList_Size = CalcCipherTextMinSize(tmpGroupIDList) + ($Struct::$StrIdxInfo::kBlockSize * tmpGroupIDList._sz);
					sz += header._groupIDList_Size;
				}
				{
					header._subGroupInfoList_Ofs = sz;
					header._subGroupInfoList_Sum = subGroupSum;
					header._subGroupInfoList_Size = $Struct::$SubGroupInfo::kBlockSize;
					sz += (header._subGroupInfoList_Size * header._subGroupInfoList_Sum);
				}
				{
					header._autoPoolInfoList_Ofs = sz;
					header._autoPoolInfoList_Sum = subGroupSum;
					header._autoPoolInfoList_Size = $Struct::$AutoPoolInfo::kBlockSize;
					sz += (header._autoPoolInfoList_Size * header._autoPoolInfoList_Sum);
				}
				{
					header._atlasResInfoList_Ofs = sz;
					header._atlasResInfoList_Sum = atlasResSum;
					header._atlasResInfoList_Size = $Struct::$AtlasResInfo::kBlockSize;
					if (structInfo._useMoreAtlasInfo)
						header._atlasResInfoList_Size += $Struct::$AtlasResInfo_Append::kBlockSize;
					sz += (header._atlasResInfoList_Size * header._atlasResInfoList_Sum);
				}
				{
					// TODO
					header._resInfoList_Group_Ofs = 0;
					header._resInfoList_Res_Ofs = 0;
					header._resInfoList_Str_Ofs = 0;
				}
				sz = getEnoughSz(sz);
				header._sz = sz;
			}
			{
				tmpGroupIDList.tfree();
				tmpSubGroupIDList.tfree();
				tmpResPathList.tfree();
			}
			{
				groupIDList.alloc(groupSum);
				subGroupIDList.alloc(subGroupSum);
				resPathList.alloc(resSum);
				groupInfoList.alloc(groupSum);
				subGroupInfoList.alloc(subGroupSum);
				autoPoolInfoList.alloc(subGroupSum);
				atlasResInfoList.alloc(atlasResSum);
				if (structInfo._useMoreAtlasInfo)
					atlasResInfoAppendList.alloc(atlasResSum);
			}
			SzI groupIdx, subGroupIdx, atlasResIdx;
			{
				groupIdx = kThFirst;
				subGroupIdx = kThFirst;
				atlasResIdx = kThFirst;
			}
			for_criter(group, structInfo._group) {
				{
					auto & groupID = groupIDList.add().getLast();
					groupID._key.cpy(group._key);
					if (!group._val._isComposite)
						groupID._key.cat($Struct::kSuffixOfCompositeShell);
					groupID._val._strIdxInfo._idx = groupIdx;
				}
				auto & groupInfo = groupInfoList.add().getLast();
				{
					$Struct::SetCID128(groupInfo._id, group._val._isComposite ? group._key : Str(group._key).cat($Struct::kSuffixOfCompositeShell));
					groupInfo._subGroupList.setNull();
				}
				for_criter(subGroup, group._val._subGroup) {
					{
						auto & subGroupID = subGroupIDList.add().getLast();
						subGroupID._key.cpy(subGroup._key);
						subGroupID._val._strIdxInfo._idx = subGroupIdx;
					}
					{
						auto & subGroupInfoItem = groupInfo._subGroupList.add().getLast();
						subGroupInfoItem._idx = subGroupIdx;
						subGroupInfoItem._resID = subGroup._val._category._res;
						$Struct::SetLocID(subGroupInfoItem._locID, subGroup._val._category._loc);
					}
					auto & subGroupInfo = subGroupInfoList.add().getLast();
					{
						$Struct::SetCID128(subGroupInfo._id, subGroup._key);
						subGroupInfo._idx = subGroupIdx;
					}
					auto & autoPoolInfo = autoPoolInfoList.add().getLast();
					{
						$Struct::SetCID128(autoPoolInfo._id, Str(subGroup._key).cat($Struct::kSuffixOfAutoPool));
					}
					auto thisAtlasResSum = subGroup._val._res.GetAtlasResSum();
					{
						subGroupInfo._rsgpAtlasResIdxBegin = atlasResIdx;
						subGroupInfo._rsgpAtlasResSum = thisAtlasResSum;
						atlasResInfoList.add(thisAtlasResSum);
						if (structInfo._useMoreAtlasInfo)
							atlasResInfoAppendList.add(thisAtlasResSum);
					}
					for_criter(res, subGroup._val._res) {
						{
							auto & resPath = resPathList.add().getLast();
							resPath._key.cpy(res._path.getFullPath(Str().getThis(), $PathDelimiter::kWindowsStyle));
							resPath._val._strIdxInfo._idx = subGroupIdx;
						}
						if (res.isAtlasType()) {
							auto & atlasResInfo = atlasResInfoList[atlasResIdx + res.getAtlasInfo()._idx];
							{
								atlasResInfo._texFmt = res.getAtlasInfo()._texFmt;
								atlasResInfo._w = res.getAtlasInfo()._sz._w;
								atlasResInfo._h = res.getAtlasInfo()._sz._h;
								switch (atlasResInfo._texFmt) { // TODO
									default: atlasResInfo._unKnown_w4 = res.getAtlasInfo()._sz._w * 4; break;
									case 0x1E: atlasResInfo._unKnown_w4 = res.getAtlasInfo()._sz._w / 2; break;
								}
							}
							if (structInfo._useMoreAtlasInfo) {
								auto & atlasResInfoAppend = atlasResInfoAppendList[atlasResIdx + res.getAtlasInfo()._idx];
								switch (res.getAtlasInfo()._texFmt) {
									default: atlasResInfoAppend._alphaSize = kSzZero; break;
									case 0x93: case 0x96:
										if (packFromGroup)
											atlasResInfoAppend._alphaSize = $DevUtil::getFileSize(Path(groupDir).add(group._key).add(subGroup._key).add($RSGPUtil::$Packer::$DfltPath::kResDir).add(res._path)) - ((res.getAtlasInfo()._sz._w * res.getAtlasInfo()._sz._h) / 2);
										else
											atlasResInfoAppend._alphaSize = $DevUtil::getFileSize(Path(resDir).add(res._path)) - ((res.getAtlasInfo()._sz._w * res.getAtlasInfo()._sz._h) / 2);
										break;
								}
								atlasResInfoAppend._unKnown_100 = $Struct::$AtlasResInfo_Append::$MemberConst::kUnKnown_100;
							}
						}
					}
					atlasResIdx += thisAtlasResSum;
					{
						$RSGPUtil::$Struct::$Header::Member rsgpHeader;
						subGroupInfo._rsgpOfs = sz;
						if (packFromGroup) {
							if (useReadyPacket && $DevUtil::existFile(Path(groupDir).add(group._key).add(subGroup._key).add("Packet").setSuffix("rsgp")))
								$DevUtil::ReadData(Path(groupDir).add(group._key).add(subGroup._key).add("Packet").setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
							else {
								if (packRSGPByMainInfo)
									$RSGPUtil::$Packer::$Pack::Pack(data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize, subGroup._val.ToRSGPStructInfo($RSGPUtil::$StructInfo::StructInfo().getThis()), Path(groupDir).add(group._key).add(subGroup._key).add("Res"), cipherTextUpperCase);
								else $RSGPUtil::$Packer::$Pack::expPackDflt(data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize, cipherTextUpperCase, Path(groupDir).add(group._key).add(subGroup._key));
								if (outputNewPacket)
									$DevUtil::WriteData(Path(groupDir).add(group._key).add(subGroup._key).add("Packet").setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
							}
						} else {
							if (useReadyPacket && $DevUtil::existFile(Path(packetDir).add(subGroup._key).setSuffix("rsgp")))
								$DevUtil::ReadData(Path(packetDir).add(subGroup._key).setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
							else {
								if (packRSGPByMainInfo)
									$RSGPUtil::$Packer::$Pack::Pack(data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize, subGroup._val.ToRSGPStructInfo($RSGPUtil::$StructInfo::StructInfo().getThis()), resDir, cipherTextUpperCase);
								else $RSGPUtil::$Packer::$Pack::Pack(data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize, $RSGPUtil::$StructInfo::StructInfo().read($JSON::read(Path(packetInfoDir).add(subGroup._key).setSuffix("json"), $JSON::Obj().setNull())), resDir, cipherTextUpperCase);
								if (outputNewPacket)
									$DevUtil::WriteData(Path(packetDir).add(subGroup._key).setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
							}
						}
						$DataBlockUtil::read(data + subGroupInfo._rsgpOfs, rsgpHeader);
						ParseRSGPHeaderToSubGroupInfo(rsgpHeader, subGroupInfo, autoPoolInfo);
						sz += subGroupInfo._rsgpSize;
					}
					++subGroupIdx;
				}
				++groupIdx;
			}
			{
				if (cipherTextUpperCase) {
					groupIDList.AllStrUpperCase();
					subGroupIDList.AllStrUpperCase();
					resPathList.AllStrUpperCase();
				}
				groupIDList.sort().EnCrypt(data + header._groupIDList_Ofs, header._groupIDList_Size);
				subGroupIDList.sort().EnCrypt(data + header._subGroupIDList_Ofs, header._subGroupIDList_Size);
				resPathList.sort().EnCrypt(data + header._resPathList_Ofs, header._resPathList_Size);
				groupInfoList.write(data + header._groupInfoList_Ofs);
				$DataBlockUtil::writeArr(data + header._subGroupInfoList_Ofs, subGroupInfoList);
				$DataBlockUtil::writeArr(data + header._autoPoolInfoList_Ofs, autoPoolInfoList);
				$DataBlockUtil::writeArr(data + header._atlasResInfoList_Ofs, atlasResInfoList, header._atlasResInfoList_Size);
				if (structInfo._useMoreAtlasInfo)
					$DataBlockUtil::writeArrNoClearBlock(data + header._atlasResInfoList_Ofs + $Struct::$AtlasResInfo::kBlockSize, atlasResInfoAppendList, header._atlasResInfoList_Size);
				$DataBlockUtil::write(data + kThFirst, header);
				if (structInfo._headerType == 4U) // TODO
					*reinterpret_cast<UI32 *>(data + header.getSz()) = header._sz;
			}
			{
				groupIDList.tfree();
				subGroupIDList.tfree();
				resPathList.tfree();
				groupInfoList.tfree();
				subGroupInfoList.tfree();
				autoPoolInfoList.tfree();
				atlasResInfoList.tfree();
			}
			return;
		}
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
					partStructInfo._headerType = structInfo._headerType;
					partStructInfo._setResInfoListInHeader = structInfo._setResInfoListInHeader;
					partStructInfo._useMoreAtlasInfo = structInfo._useMoreAtlasInfo;
					for_criter(groupID, partInfo._group)
						for_criter(group, structInfo._group)
						if (groupID == group._key) {
							partStructInfo._group.addByKV(group._key, group._val);
							break;
						}
				}
				Pack(data, sz, partStructInfo, packFromGroup, packRSGPByMainInfo, outputNewPacket, useReadyPacket, resDir, packetDir, packetInfoDir, groupDir, cipherTextUpperCase);
				$DevUtil::WriteData(Path(partDir).add(partInfo._id), data, sz);
			}
			delPtrArr(data);
			return;
		}
	}
	namespace $UnPack {
		static auto OutPutResFile(
			Byte const * const & data,
			ListOfBlock<$Struct::$GroupInfo::GroupInfo> & groupInfoList,
			ListP<$Struct::$SubGroupInfo::Member> & subGroupInfoList,
			ListP<$RSGPUtil::$Struct::$Header::Member>	const & rsgpHeaderList,
			ListP<CipherText<$RSGPUtil::$Struct::ResInfoSlot>> const & rsgpResInfoList,
			Bool const & outputRes, Path const & resDir,
			Bool const & outputPacket, Path const & packetDir,
			Bool const & outputPacketInfo, Path const & packetInfoDir,
			Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & groupDir
		) {
			for_criter(groupInfo, groupInfoList) {
				for_criter(subGroupInfoItem, groupInfo._subGroupList) {
					auto const & subGroupInfo = subGroupInfoList[subGroupInfoItem._idx];
					/*if (subGroupInfo._rsgpOfs >= 0x241a5000u || subGroupInfo._rsgpOfs < 0x1d2000u || *(UI32*)(data + subGroupInfo._rsgpOfs) != 'rsgp') {
						wWarn("pass"); continue;
					}*/
					if (outputRes)
						$RSGPUtil::$Packer::$UnPack::OutPutResFile(data + subGroupInfo._rsgpOfs, rsgpHeaderList[subGroupInfoItem._idx]._resDataCompressMethod, rsgpHeaderList[subGroupInfoItem._idx]._normalResData_Ofs, rsgpHeaderList[subGroupInfoItem._idx]._normalResData_InitSize, rsgpHeaderList[subGroupInfoItem._idx]._normalResData_Size, rsgpHeaderList[subGroupInfoItem._idx]._atlasResData_Ofs, rsgpHeaderList[subGroupInfoItem._idx]._atlasResData_InitSize, rsgpHeaderList[subGroupInfoItem._idx]._atlasResData_Size, rsgpResInfoList[subGroupInfoItem._idx], resDir);
					if (outputPacket)
						$DevUtil::WriteData(Path(packetDir).add(subGroupInfo._id).setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
					if (outputPacketInfo)
						$JSON::write(Path(packetInfoDir).add(subGroupInfo._id).setSuffix("json"), $RSGPUtil::$Packer::$UnPack::ParseInfo($RSGPUtil::$StructInfo::StructInfo().getThis(), rsgpHeaderList[subGroupInfoItem._idx]._headerType, rsgpHeaderList[subGroupInfoItem._idx]._resDataCompressMethod, rsgpResInfoList[subGroupInfoItem._idx]).write($JSON::Obj().setNull()));
					if (outputGroup) {
						Str groupID;
						{
							StrRef oriID(groupInfo._id);
							if (oriID._len > $Struct::kSuffixOfCompositeShell._len && StrRef(oriID._str + (oriID._len - $Struct::kSuffixOfCompositeShell._len)) == $Struct::kSuffixOfCompositeShell)
								groupID.cpy(oriID, kThFirst, oriID._len - $Struct::kSuffixOfCompositeShell._len);
							else groupID.cpy(oriID);
							oriID.clear();
						}
						if (outputGroupRes)
							$RSGPUtil::$Packer::$UnPack::expUnPackDflt(data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize, Path(groupDir).add(groupID).add(subGroupInfo._id));
						if (outputGroupPacket)
							$DevUtil::WriteData(Path(groupDir).add(groupID).add(subGroupInfo._id).add("Packet").setSuffix("rsgp"), data + subGroupInfo._rsgpOfs, subGroupInfo._rsgpSize);
						groupID.tfree();
					}
				}
			}
			return;
		}
		static auto & ParseRSGPInfo(
			$StructInfo::SubGroup & subGroupInfo,
			SzI const & resID, FourCC const & locID,
			SzI const & headerType, UI32 const & resDataCompressMethod,
			Map<Str, $RSGPUtil::$Struct::ResInfoSlot> const & resInfoList,
			ListP<$Struct::$AtlasResInfo::Member> const & atlasResInfoList,
			SzI const & atlasResInfoIdxBegin
		) {
			subGroupInfo._category._res = resID;
			$Struct::GetLocID(subGroupInfo._category._loc, locID);
			subGroupInfo._headerType = headerType;
			subGroupInfo._compressNormalRes = resDataCompressMethod & $RSGPUtil::$Struct::$ExistBit_CompressMethod::kCompressNormalData;
			subGroupInfo._compressAtlasRes = resDataCompressMethod & $RSGPUtil::$Struct::$ExistBit_CompressMethod::kCompressAtlasData;
			subGroupInfo._res.alloc(resInfoList._sz);
			for_criter(resInfo, resInfoList) {
				auto & res = subGroupInfo._res.add().getLast();
				res._path.read(resInfo._key);
				switch (resInfo._val._dataInfo._resType) {
					default: wErr("UnKnown ResType"); break;
					case $RSGPUtil::$Struct::$ResTypeIdx::kNormal:
						res.setNormalType();
						break;
					case $RSGPUtil::$Struct::$ResTypeIdx::kAtlas:
						res.setAtlasType();
						res.getAtlasInfo()._idx = resInfo._val._atlasInfo._idx;
						res.getAtlasInfo()._sz.set(resInfo._val._atlasInfo._w, resInfo._val._atlasInfo._h);
						res.getAtlasInfo()._texFmt = atlasResInfoList[atlasResInfoIdxBegin + resInfo._val._atlasInfo._idx]._texFmt;
						break;
				}
			}
			return subGroupInfo;
		}
		static auto & ParseInfo(
			$StructInfo::StructInfo & structInfo,
			SzI const & headerType,
			Bool const & setResInfoListInHeader, Bool const & useMoreAtlasInfo,
			ListP<$Struct::$GroupInfo::GroupInfo> const & groupInfoList,
			ListP<$Struct::$SubGroupInfo::Member> const & subGroupInfoList,
			Map<Str, $Struct::StrIdxInfoSlot> const & resPathList,
			ListP<$Struct::$AtlasResInfo::Member> const & atlasResInfoList,
			ListP<$RSGPUtil::$Struct::$Header::Member> const & rsgpHeaderList,
			ListP<CipherText<$RSGPUtil::$Struct::ResInfoSlot>> const & rsgpResInfoList
		) {
			structInfo._headerType = headerType;
			structInfo._setResInfoListInHeader = setResInfoListInHeader;
			structInfo._useMoreAtlasInfo = useMoreAtlasInfo;
			structInfo._group.alloc(groupInfoList._sz);
			for_criter(groupInfo, groupInfoList) {
				auto & group = structInfo._group.add().getLast();
				{
					StrRef oriID(groupInfo._id);
					if (oriID._len > $Struct::kSuffixOfCompositeShell._len && StrRef(oriID._str + (oriID._len - $Struct::kSuffixOfCompositeShell._len)) == $Struct::kSuffixOfCompositeShell) {
						group._key.cpy(oriID, kThFirst, oriID._len - $Struct::kSuffixOfCompositeShell._len);
						group._val._isComposite = kFalse;
					} else {
						group._key.cpy(oriID);
						group._val._isComposite = kTrue;
					}
					oriID.clear();
				}
				group._val._subGroup.alloc(groupInfo._subGroupList._sz);
				for_criter(subGroupItem, groupInfo._subGroupList) {
					auto & subGroup = group._val._subGroup.add().getLast();
					subGroup._key.cpy(subGroupInfoList[subGroupItem._idx]._id);
					ParseRSGPInfo(subGroup._val, subGroupItem._resID, subGroupItem._locID, rsgpHeaderList[subGroupItem._idx]._headerType, rsgpHeaderList[subGroupItem._idx]._resDataCompressMethod, rsgpResInfoList[subGroupItem._idx], atlasResInfoList, subGroupInfoList[subGroupItem._idx]._rsgpAtlasResIdxBegin);
				}
			}
			return structInfo;
		}
		static auto AnalysisRSGP(
			Byte const * const & data,
			ListP<$Struct::$SubGroupInfo::Member> const & subGroupInfoList,
			ListP<$RSGPUtil::$Struct::$Header::Member> & rsgpHeaderList,
			ListP<CipherText<$RSGPUtil::$Struct::ResInfoSlot>> & rsgpResInfoList
		) {
			rsgpHeaderList.alloc(subGroupInfoList._sz);
			rsgpResInfoList.alloc(subGroupInfoList._sz);

			for_criter(subGroupInfo, subGroupInfoList) {
				$RSGPUtil::$Packer::$UnPack::Analysis(data + subGroupInfo._rsgpOfs, rsgpHeaderList.add().getLast(), rsgpResInfoList.add().getLast());
			}/*
			for_criter(subGroupInfo, subGroupInfoList) {
				rsgpHeaderList.add();
				rsgpResInfoList.add();
				wLogLn("%X, %s", subGroupInfo._rsgpOfs, subGroupInfo._id);
				if (subGroupInfo._rsgpOfs >= 0x241a5000u || subGroupInfo._rsgpOfs < 0x1d2000u) {
					wWarn("pass"); continue;
				}
				$RSGPUtil::$Packer::$UnPack::Analysis(data + subGroupInfo._rsgpOfs, rsgpHeaderList.getLast(), rsgpResInfoList.getLast());
			}*/
			return;
		}
		static auto ReadResInfoList(
			Byte const * const & groupInfoData, SzI const & groupInfoDataSz,
			Byte const * const & resInfoData, Byte const * const & strList, $ResInfoList::ResInfoList & dst
		) {
			dst._group.setNull();
			ListOfBlock<$Struct::ResList_Group> groupInfoList;
			for (SzI groupPos = kThFirst; groupPos < groupInfoDataSz; ) {
				auto & groupInfo = groupInfoList.add().getLast();
				groupInfo.read(groupInfoData + groupPos);
				groupPos += groupInfo.getSz();
				auto & groupItem = dst._group.add().getLast();
				groupItem._id.cpy(reinterpret_cast<Ch const *>(strList + groupInfo._info._id_Ofs));
				groupItem._unKnown = groupInfo._info._unKnown;
				groupItem._subGroup.alloc(groupInfo._info._subGroup_Sum);
				for_criter(subGroup, groupInfo._subGroup) {
					auto & subGroupItem = groupItem._subGroup.add().getLast();
					subGroupItem._id.cpy(reinterpret_cast<Ch const *>(strList + subGroup._info._id_Ofs));
					subGroupItem._category._res = subGroup._info._resID;
					$Struct::GetLocID(subGroupItem._category._loc, subGroup._info._locID);
					subGroupItem._res.alloc(subGroup._info._res_Sum);
					ListOfBlock<$Struct::ResList_Res> resInfoList;
					resInfoList.setNull();
					for_criter(res, subGroup._res) {
						auto & resInfo = resInfoList.add().getLast();
						resInfo.read(resInfoData + res._info_Ofs, res._info_Ofs);
						auto & resItem = subGroupItem._res.add().getLast();
						resItem._id.cpy(reinterpret_cast<Ch const *>(strList + resInfo._info._id_Ofs));
						resItem._path.read(reinterpret_cast<Ch const *>(strList + resInfo._info._path_Ofs));
						if (resInfo._info._type < 8u)
							resItem._type.cpy($Struct::$ResList_Res_Base::kTypeStr[resInfo._info._type]);
						else resItem._type.setNull();
						resItem._existExpand = (resInfo._info._expandMember_Ofs != kThNone);
						if (resItem._existExpand) {
							resItem._expand._unKnown_A = resInfo._expandMember._unKnown_A;
							resItem._expand._unKnown_C = resInfo._expandMember._unKnown_C;
							resItem._expand._unKnown_D = resInfo._expandMember._unKnown_D;
							resItem._expand._flag = resInfo._expandMember._flag;
							resItem._expand._pos.set(resInfo._expandMember._x, resInfo._expandMember._y);
							resItem._expand._sz.set(resInfo._expandMember._w, resInfo._expandMember._h);
							resItem._expand._padding.set(resInfo._expandMember._paddingX, resInfo._expandMember._paddingY);
							if (resInfo._expandMember._parent_Ofs != kThNone)
								resItem._expand._parent.cpy(reinterpret_cast<Ch const *>(strList + resInfo._expandMember._parent_Ofs));
							else resItem._expand._parent.setNull();
						}
						resItem._append.setNull();
						for_criter(appendMember, resInfo._appendMember) {
							auto & appendItem = resItem._append.add().getLast();
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
		static auto Analysis(
			Byte const * const & data,
			Bool & setResInfoListInHeader, Bool & useMoreAtlasInfo,
			$Struct::$Header::Member & header,
			CipherText<$Struct::StrIdxInfoSlot> & groupIDList,
			CipherText<$Struct::StrIdxInfoSlot> & subGroupIDList,
			CipherText<$Struct::StrIdxInfoSlot> & resPathList,
			ListOfBlock<$Struct::$GroupInfo::GroupInfo> & groupInfoList,
			ListOfBlock<$Struct::$SubGroupInfo::Member> & subGroupInfoList,
			ListOfBlock<$Struct::$AutoPoolInfo::Member> & autoPoolInfoList,
			ListOfBlock<$Struct::$AtlasResInfo::Member> & atlasResInfoList,
			ListOfBlock<$Struct::$AtlasResInfo_Append::Member> & atlasResInfoAppendList,
			ListOfBlock<$RSGPUtil::$Struct::$Header::Member> & rsgpHeaderList,
			ListP<CipherText<$RSGPUtil::$Struct::ResInfoSlot>> & rsgpResInfoList,
			$ResInfoList::ResInfoList & resInfoList
		) {
			$DataBlockUtil::read(data + kThFirst, header);
			groupIDList.DeCrypt(data + header._groupIDList_Ofs, header._groupIDList_Size);
			subGroupIDList.DeCrypt(data + header._subGroupIDList_Ofs, header._subGroupIDList_Size);
			resPathList.DeCrypt(data + header._resPathList_Ofs, header._resPathList_Size);
			subGroupInfoList.allocThenNew(header._subGroupInfoList_Sum);
			autoPoolInfoList.allocThenNew(header._autoPoolInfoList_Sum);
			atlasResInfoList.allocThenNew(header._atlasResInfoList_Sum);
			groupInfoList.read(data + header._groupInfoList_Ofs, header._groupInfoList_Sum);
			$DataBlockUtil::readArr(data + header._subGroupInfoList_Ofs, subGroupInfoList);
			$DataBlockUtil::readArr(data + header._autoPoolInfoList_Ofs, autoPoolInfoList);
			$DataBlockUtil::readArr(data + header._atlasResInfoList_Ofs, atlasResInfoList, header._atlasResInfoList_Size);
			if (header._atlasResInfoList_Size == $Struct::$AtlasResInfo::kBlockSize) {
				useMoreAtlasInfo = kFalse;
				atlasResInfoAppendList.setNull();
			} elif(header._atlasResInfoList_Size == ($Struct::$AtlasResInfo::kBlockSize + $Struct::$AtlasResInfo_Append::kBlockSize)) {
				useMoreAtlasInfo = kTrue;
				atlasResInfoAppendList.allocThenNew(header._atlasResInfoList_Sum);
				$DataBlockUtil::readArr(data + header._atlasResInfoList_Ofs + $Struct::$AtlasResInfo::kBlockSize, atlasResInfoAppendList, $Struct::$AtlasResInfo::kBlockSize + $Struct::$AtlasResInfo_Append::kBlockSize);
			} else wErr("AtlasResInfo sz Error");
			if (header._headerType > 4) wWarn("headerType > 4");
			setResInfoListInHeader = (header._resInfoList_Group_Ofs != kThNone);
			if (setResInfoListInHeader)
				ReadResInfoList(data + header._resInfoList_Group_Ofs, header._resInfoList_Res_Ofs - header._resInfoList_Group_Ofs, data + header._resInfoList_Res_Ofs, data + header._resInfoList_Str_Ofs, resInfoList);
			AnalysisRSGP(data, subGroupInfoList, rsgpHeaderList, rsgpResInfoList);
			return;
		}
		Void UnPack(
			Byte const * const & data, SzI const & sz,
			Path const & structInfoFile, Path const & resInfoListFile,
			Bool const & outputRes, Path const & resDir,
			Bool const & outputPacket, Path const & packetDir,
			Bool const & outputPacketInfo, Path const & packetInfoDir,
			Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & groupDir
		) {
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
				$JSON::write(structInfoFile, ParseInfo($StructInfo::StructInfo().getThis(), header._headerType, setResInfoListInHeader, useMoreAtlasInfo, groupInfoList, subGroupInfoList, resPathList, atlasResInfoList, rsgpHeaderList, rsgpResInfoList).write($JSON::Obj().setNull()));
				if (setResInfoListInHeader)
					$JSON::write(resInfoListFile, resInfoList.write($JSON::Obj().setNull()));
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
			return;
		}
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
							wErr("Diff headerType of This Part");
						if (fullStructInfo._setResInfoListInHeader != structInfo._setResInfoListInHeader)
							wErr("Diff setResInfoListInHeader of This Part");
						if (fullStructInfo._useMoreAtlasInfo != structInfo._useMoreAtlasInfo)
							wErr("Diff useMoreAtlasInfo of This Part");
					}
					fullStructInfo._group.add(structInfo._group);
					{
						auto & partInfo = partInfoList.add().getLast();
						partInfo._id = PartID;
						for_criter(group, structInfo._group)
							partInfo._group.add(group._key);
					}
					if (setResInfoListInHeader) {
						existResInfoList = kTrue;
						fullResInfoList._group.add(resInfoList._group);
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
		}
	}
}
