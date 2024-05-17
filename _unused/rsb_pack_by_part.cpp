// ------------------------------------------------

	// ------------------------------------------------

	inline IntegerU32 random_integer_u32() {
		std::mt19937_64 gen { static_cast<unsigned long long>(std::chrono::system_clock::now().time_since_epoch().count()) };
		std::uniform_int_distribution<IntegerU32Z> distribution { 0u };
		return make_wrapper<IntegerU32>(distribution(gen));
	}

// ------------------------------------------------

namespace TwinKleS::Tool::Package::RSB {

	// ------------------------------------------------

	namespace Pack {

		// ------------------------------------------------

		// ------------------------------------------------

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

		// ------------------------------------------------

	}

	// ------------------------------------------------

	namespace Unpack {

		// ------------------------------------------------

		// ------------------------------------------------

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

		// ------------------------------------------------

	}

	// ------------------------------------------------

}

// ------------------------------------------------
