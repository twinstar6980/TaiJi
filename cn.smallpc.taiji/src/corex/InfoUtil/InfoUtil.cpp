# include "./InfoUtil.h"

namespace $TJ::$InfoUtil {
	namespace $Cnv {
		namespace $OriToResPkgInfo {
			inline auto ParseRes_Base($ResPkgInfo_Ori::resource_item_base const & src, Str & dstID, $ResPkgInfo::ResBase & dst) {
				dstID = src._id;
				dst._path = src._path;
				dst._type = src._type;
				return;
			}
			inline auto ParseRes_ImageInfo($ResPkgInfo_Ori::image_info_append const & src, $ResPkgInfo::ImageInfo & dst) {
				dst._pos.set(src._ax, src._ay);
				dst._sz.set(src._aw, src._ah);
				dst._padding.set(src._x, src._y);
				return;
			}
			inline auto ParseRes_AtlasInfo($ResPkgInfo_Ori::atlas_info_append const & src, $ResPkgInfo::AtlasInfo & dst) {
				dst._sz.set(src._width, src._height);
				dst._img.setNull();
				return;
			}
			static auto ParseResList(ListP<$ResPkgInfo_Ori::resource_item> const & src, MapOfInfo<$ResPkgInfo::Res> & dst) {
				dst.alloc(src._sz);
				for_criter(srcRes, src)
					switch (srcRes._resType) {
						default: wErr("UnKnown ResType"); break;
						case $ResPkgInfo::ResType::kNormal: {
							auto & dstRes = dst.add().getLast();
							ParseRes_Base(srcRes, dstRes._key, dstRes._val);
							dstRes._val.setNormalType();
							break;
						}
						case $ResPkgInfo::ResType::kAtlas: {
							auto & dstRes = dst.add().getLast();
							ParseRes_Base(srcRes, dstRes._key, dstRes._val);
							dstRes._val.setAtlasType();
							ParseRes_AtlasInfo(srcRes.getAtlasInfo(), dstRes._val.getAtlasInfo());
							break;
						}
						case $ResPkgInfo::ResType::kImage:
							for (auto resIdx = kThFirst;; ++resIdx) {
								if (resIdx == dst._sz) {
									wErr("Can Not find parent-Atlas");
									break;
								}
								if (srcRes.GetImageInfo()._parent == dst[resIdx]._key) {
									auto & dstImageRes = dst[resIdx]._val.getAtlasInfo()._img.add().getLast();
									ParseRes_Base(srcRes, dstImageRes._key, dstImageRes._val.getResBase());
									ParseRes_ImageInfo(srcRes.GetImageInfo(), dstImageRes._val._imageInfo);
									break;
								}
							}
							break;
					}
				dst.eraseSurplusCap();
				return;
			}
			static auto ParseSubGroup($ResPkgInfo_Ori::subgroup_info_append const & src, $ResPkgInfo::SubGroup & dst, Str const & id) {
				dst._category.setNull();
				if (!src._res.isNull())
					src._res.scanFmt($ResPkgInfo_Ori::kResIDStrFmt._str, &dst._category._res);
				if (!src._loc.isNull())
					dst._category._loc = src._loc;
				ParseResList(src._resources, dst._res);
				return;
			}
			static auto ParseGroupList(ListP<$ResPkgInfo_Ori::group_item> const & src, MapOfInfo<$ResPkgInfo::Group> & dst) {
				dst.alloc(src._sz);
				for_criter(srcGroup, src) {
					if (srcGroup._type.cmp("composite")) {
						auto & dstGroup = dst.add().getLast();
						dstGroup._key = srcGroup._id;
						dstGroup._val._isComposite = kTrue;
						dstGroup._val._subGroup.setNull();
					} elif(srcGroup._type.cmp("simple")) {
						if (srcGroup.getSubGroupInfo()._parent.isNull()) {
							auto & dstGroup = dst.add().getLast();
							dstGroup._key = srcGroup._id;
							dstGroup._val._isComposite = kFalse;
							dstGroup._val._subGroup.allocThenNew(kSzOne);
							dstGroup._val._subGroup[kSzZero]._key = srcGroup._id;
							ParseSubGroup(srcGroup.getSubGroupInfo(), dstGroup._val._subGroup[kSzZero]._val, srcGroup._id);
						} else {
							for (auto resIdx = kThFirst; ; ++resIdx) {
								if (resIdx == dst._sz) {
									wErr("Can Not Found parent-group of %s", srcGroup._id._str);
									break;
								}
								if (srcGroup.getSubGroupInfo()._parent == dst[resIdx]._key) {
									auto & dstSubGroup = dst[resIdx]._val._subGroup.add().getLast();
									dstSubGroup._key = srcGroup._id;
									ParseSubGroup(srcGroup.getSubGroupInfo(), dstSubGroup._val, srcGroup._id);
									break;
								}
							}
						}
					}
				}
				dst.eraseSurplusCap();
				return;
			}
			$ResPkgInfo::ResPkgInfo & ParseInfo($ResPkgInfo_Ori::ResPkgInfo_Ori const & src, $ResPkgInfo::ResPkgInfo & dst) {
				ParseGroupList(src._groups, dst._group);
				return dst;
			}
		}
		namespace $ResPkgInfoToOri {
			inline auto ParseRes_Base(Str const & srcID, $ResPkgInfo::ResBase const & src, $ResPkgInfo_Ori::resource_item_base & dst) {
				dst._slot = kSzNull;
				dst._id = srcID;
				dst._path = src._path;
				dst._type = src._type;
				return;
			}
			inline auto ParseRes_ImageInfo($ResPkgInfo::ImageInfo const & src, $ResPkgInfo_Ori::image_info_append & dst, Str const & parent) {
				dst._parent = parent;
				dst._ax = src._pos._x;
				dst._ay = src._pos._y;
				dst._aw = src._sz._w;
				dst._ah = src._sz._h;
				dst._x = src._padding._x;
				dst._y = src._padding._y;
				return;
			}
			inline auto ParseRes_AtlasInfo($ResPkgInfo::AtlasInfo const & src, $ResPkgInfo_Ori::atlas_info_append & dst) {
				dst._width = src._sz._w;
				dst._height = src._sz._h;
				return;
			}
			static auto ParseRes_AtlasInfo_ImageList(MapOfInfo<$ResPkgInfo::ImageRes> const & src, ListP<$ResPkgInfo_Ori::resource_item> & dst, Str const & parent) {
				for_criter(srcImageRes, src) {
					auto & dstRes = dst.add().getLast();
					dstRes._resType = $ResPkgInfo::ResType::kImage;
					ParseRes_Base(srcImageRes._key, srcImageRes._val, dstRes);
					dstRes._append = new $ResPkgInfo_Ori::image_info_append();
					ParseRes_ImageInfo(srcImageRes._val._imageInfo, dstRes.GetImageInfo(), parent);
				}
				return;
			}
			static auto ParseResList(MapOfInfo<$ResPkgInfo::Res> const & src, ListP<$ResPkgInfo_Ori::resource_item> & dst) {
				dst.alloc(src._sz);
				for_criter(srcRes, src) {
					auto & dstRes = dst.add().getLast();
					switch (srcRes._val._resType) {
						default: wErr("UnKnown ResType"); break;
						case $ResPkgInfo::ResType::kNormal:
							ParseRes_Base(srcRes._key, srcRes._val, dstRes);
							dstRes._resType = $ResPkgInfo::ResType::kNormal;
							dstRes._append = kNullPtr;
							break;
						case $ResPkgInfo::ResType::kAtlas:
							ParseRes_Base(srcRes._key, srcRes._val, dstRes);
							dstRes._resType = $ResPkgInfo::ResType::kAtlas;
							dstRes._append = new $ResPkgInfo_Ori::atlas_info_append();
							ParseRes_AtlasInfo(srcRes._val.getAtlasInfo(), dstRes.getAtlasInfo());
							dst.expand(srcRes._val.getAtlasInfo()._img._sz);
							ParseRes_AtlasInfo_ImageList(srcRes._val.getAtlasInfo()._img, dst, srcRes._key);
							break;
					}
				}
				return;
			}
			static auto ParseSubGroupAppendInfo($ResPkgInfo::SubGroup const & src, $ResPkgInfo_Ori::subgroup_info_append & dst, Str const & parent) {
				dst._parent = parent;
				if (src._category.isValidRes())
					dst._res.alloc($MaxStrLenDef::kInt32Str).printFmtByAppend($ResPkgInfo_Ori::kResIDStrFmt._str, src._category._res);
				else dst._res.setNull();
				dst._loc = src._category._loc;
				ParseResList(src._res, dst._resources);
				return;
			}
			static auto ParseSubGroupList(MapOfInfo<$ResPkgInfo::SubGroup> const & src, ListP<$ResPkgInfo_Ori::group_item> & dst, Str const & parent) {
				for_criter(srcSubGroup, src) {
					auto & dstSubGroup = dst.add().getLast();
					dstSubGroup._id = srcSubGroup._key;
					dstSubGroup._type.cpy("simple");
					dstSubGroup._append = new $ResPkgInfo_Ori::subgroup_info_append();
					ParseSubGroupAppendInfo(srcSubGroup._val, dstSubGroup.getSubGroupInfo(), parent);
				}
				return;
			}
			static auto ParseGroupAppendInfo_SubGroupList(MapOfInfo<$ResPkgInfo::SubGroup> const & src, ListP<$ResPkgInfo_Ori::subgroups_info_item> & dst) {
				dst.alloc(src._sz);
				for_criter(srcSubGroup, src) {
					auto & dstSubGroup = dst.add().getLast();
					dstSubGroup._id = srcSubGroup._key;
					if (srcSubGroup._val._category.isValidRes())
						dstSubGroup._res.alloc($MaxStrLenDef::kInt32Str).printFmt($ResPkgInfo_Ori::kResIDStrFmt._str, srcSubGroup._val._category._res);
					else dstSubGroup._res.setNull();
					dstSubGroup._loc.cpy(srcSubGroup._val._category._loc);
				}
				return;
			}
			static auto CalcGroupItemSumLimit(MapOfInfo<$ResPkgInfo::Group> const & src) {
				auto sum = src._sz;
				for_criter(group, src)
					sum += group._val._subGroup._sz;
				return sum;
			}
			static auto ParseGroupList(MapOfInfo<$ResPkgInfo::Group> const & src, ListP<$ResPkgInfo_Ori::group_item> & dst) {
				dst.alloc(CalcGroupItemSumLimit(src));
				for_criter(srcGroup, src) {
					if (!srcGroup._val._isComposite) ParseSubGroupList(srcGroup._val._subGroup, dst, Str().setNull());
					else {
						auto & dstGroup = dst.add().getLast();
						dstGroup._id.cpy(srcGroup._key);
						dstGroup._type.cpy("composite");
						dstGroup._append = new $ResPkgInfo_Ori::group_info_append();
						ParseGroupAppendInfo_SubGroupList(srcGroup._val._subGroup, dstGroup.getGroupInfo()._subgroups);
						ParseSubGroupList(srcGroup._val._subGroup, dst, srcGroup._key);
					}
				}
				dst.eraseSurplusCap();
				return;
			}
			static auto SetSlotVal($ResPkgInfo_Ori::ResPkgInfo_Ori & dst, SzI const & beginNum) {
				StrList resIDList;
				SzI resIDIdx;
				resIDList.setNull();
				for_riter(group, dst._groups)
					if (group._type.cmp("simple"))
						for_riter(res, group.getSubGroupInfo()._resources) {
						if (resIDList.find(res._id, resIDIdx))
							res._slot = beginNum + resIDIdx;
						else res._slot = beginNum + resIDList.add(res._id).lastIdx();
					}
				dst._slot_count = beginNum + resIDList._sz;
				resIDList.tfree();
				return;
			}
			$ResPkgInfo_Ori::ResPkgInfo_Ori & ParseInfo($ResPkgInfo::ResPkgInfo const & src, $ResPkgInfo_Ori::ResPkgInfo_Ori & dst, SzI const & slotBeginNum) {
				ParseGroupList(src._group, dst._groups);
				SetSlotVal(dst, slotBeginNum);
				return dst;
			}
		}
		/*namespace $RSBResInfoListToResPkgInfo {
			inline Void ParseRes_Base($ResPkgUtil::$RSBUtil::$ResInfoList::Res const & src, $ResPkgInfo::Res & dst) {
				dst._id = src._id;
				dst._path = src._path;
				dst._type = src._type;
				return;
			}
			inline Void ParseRes_ImageInfo($ResPkgUtil::$RSBUtil::$ResInfoList::Res_Expand const & src, $ResPkgInfo::ImageInfo & dst) {
				dst._pos = src._pos;
				dst._sz = src._sz;
				dst._padding = src._padding;
				return;
			}
			inline Void ParseRes_AtlasInfo($ResPkgUtil::$RSBUtil::$ResInfoList::Res_Expand const & src, $ResPkgInfo::AtlasInfo & dst) {
				dst._sz = src._sz;
				dst._img.setNull();
				return;
			}
			Void ParseResList(ListP<$ResPkgUtil::$RSBUtil::$ResInfoList::Res> const & src, ListP<$ResPkgInfo::Res> & dst) {
				dst.alloc(src._sz);
				for_criter(srcRes, src) {
					auto & dstRes = dst.add().getLast();
					ParseRes_Base(srcRes, dstRes);
					if (!srcRes._type.cmp("Image")) {
						dstRes._resType = $ResPkgInfo::ResType::kNormal;
						dstRes._append = kNullPtr;
					} else {
						if (!srcRes._expand._parent.isNull()) {
							dstRes._resType = $ResPkgInfo::ResType::kAtlas;
							dstRes._append = new $ResPkgInfo::AtlasInfo();
							ParseRes_AtlasInfo(srcRes._expand, dstRes.getAtlasInfo());
						} else {
							for (auto resIdx = kThFirst; ; ++resIdx) {
								if (resIdx == dst._sz) {
									wError("Can Not find parent-Atlas");
									break;
								}
								if (srcRes._expand._parent.cmp(dst[resIdx]._id)) {
									auto & dstImageRes = dst[resIdx].getAtlasInfo()._img.add().getLast();
									ParseRes_ImageInfo(srcRes._expand, dstImageRes._imageInfo);
									break;
								}
							}
						}
					}
				}
				dst.eraseSurplusCap();
				return;
			}
			Void ParseGroupList(ListP<$ResPkgUtil::$RSBUtil::$ResInfoList::Group> const & src, ListP<$ResPkgInfo::Group> & dst) {
				dst.alloc(src._sz);
				for_criter(srcGroup, src) {
					auto & dstGroup = dst.add().getLast();
					dstGroup._id.cpy(srcGroup._id);
					dstGroup._isComposite = kTrue; // TODO
					dstGroup._subGroup.alloc(srcGroup._subGroup._sz);
					for_criter(srcSubGroup, srcGroup._subGroup) {
						auto & dstSubGroup = dstGroup._subGroup.add().getLast();
						dstSubGroup._id.cpy(srcSubGroup._id);
						dstSubGroup._resID = srcSubGroup._resID;
						dstSubGroup._locID.cpy(srcSubGroup._locID);
						ParseResList(srcSubGroup._res, dstSubGroup._res);
					}
				}
				return;
			}
			$ResPkgInfo::ResPkgInfo & ParseInfo($ResPkgUtil::$RSBUtil::$ResInfoList::ResInfoList const & src, $ResPkgInfo::ResPkgInfo & dst) {
				dst._id.setNull();
				ParseGroupList(src._group, dst._group);
				return dst;
			}
		}*/
		namespace $ResPkgInfoToRSBStructInfo {
			static auto ParseSubGroup($ResPkgInfo::SubGroup const & src, $ResPkgUtil::$RSBUtil::$StructInfo::SubGroup & dst, $ImageUtil::TexFmtIdxList const & texFmtIdxList) {
				dst._category = src._category;
				dst._compressNormalRes = kTrue;
				dst._compressAtlasRes = kTrue;
				dst._res.alloc(src._res._sz);
				for_criter(srcRes, src._res) {
					auto & dstRes = dst._res.add().getLast();
					dstRes._path.cpy(srcRes._val._path);
					switch (srcRes._val._resType) {
						default: wErr("UnKnown ResType"); break;
						case $ResPkgInfo::ResType::kNormal:
							dstRes.setNormalType();
							break;
						case $ResPkgInfo::ResType::kAtlas:
							dstRes.setAtlasType();
							dstRes._path.cpy(srcRes._val._path).setSuffix("ptx");
							dstRes.getAtlasInfo()._idx = dst._res.GetAtlasResSum() - kThSecond;
							dstRes.getAtlasInfo()._sz.cpy(srcRes._val.getAtlasInfo()._sz);
							dstRes.getAtlasInfo()._texFmt = texFmtIdxList.FindTexFmt(kEmptyStr);
							break;
					}
				}
				return;
			}
			static auto ParseGroup($ResPkgInfo::Group const & src, $ResPkgUtil::$RSBUtil::$StructInfo::Group & dst, $ImageUtil::TexFmtIdxList const & texFmtIdxList) {
				dst._isComposite = src._isComposite;
				dst._subGroup.alloc(src._subGroup._sz);
				for_criter(srcSubGroup, src._subGroup) {
					auto & dstSubGroup = dst._subGroup.add().getLast();
					dstSubGroup._key = srcSubGroup._key;
					ParseSubGroup(srcSubGroup._val, dstSubGroup._val, texFmtIdxList);
				}
				return;
			}
			static auto ParseGroupList(MapOfInfo<$ResPkgInfo::Group> const & src, MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::Group> & dst, $ImageUtil::TexFmtIdxList const & texFmtIdxList) {
				dst.alloc(src._sz);
				for_criter(srcGroup, src) {
					auto & dstGroup = dst.add().getLast();
					dstGroup._key = srcGroup._key;
					ParseGroup(srcGroup._val, dstGroup._val, texFmtIdxList);
				}
				return;
			}
			$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & ParseInfo($ResPkgInfo::ResPkgInfo const & src, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & dst, $ImageUtil::TexFmtIdxList const & texFmtIdxList) {
				ParseGroupList(src._group, dst._group, texFmtIdxList);
				return dst;
			}
		}
		namespace $ResPkgInfoSubGroupToRSGPStructInfo {
			$ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo & ParseInfo($ResPkgInfo::SubGroup const & src, $ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo & dst) {
				dst._headerType = 0u; // TODO
				dst._compressNormalRes = kTrue;
				dst._compressAtlasRes = kTrue;
				dst._res.alloc(src._res._sz);
				for_criter(srcRes, src._res) {
					auto & dstRes = dst._res.add().getLast();
					dstRes._path.cpy(srcRes._val._path);
					switch (srcRes._val._resType) {
						default: wErr("UnKnown ResType"); break;
						case $ResPkgInfo::ResType::kNormal:
							dstRes.setNormalType();
							break;
						case $ResPkgInfo::ResType::kAtlas:
							dstRes.setAtlasType();
							dstRes.getAtlasInfo()._idx = dst._res.GetAtlasResSum() - kThSecond;
							dstRes.getAtlasInfo()._sz.cpy(srcRes._val.getAtlasInfo()._sz);
							break;
					}
				}
				return dst;
			}
		}
		namespace $ResPkgInfoToAtlasInfoList {
			static auto CalcAtlasSumLimit(MapOfInfo<$ResPkgInfo::Group> const & src) {
				auto sum = kSzZero;
				for_criter(group, src)
					for_criter(subGroup, group._val._subGroup)
					sum += subGroup._val._res._sz;
				return sum;
			}
			inline auto ParseImageRes($ResPkgInfo::ImageRes const & src, $ImageUtil::ImageInfo & dst) {
				dst._path = src._path;
				dst._pos = src._imageInfo._pos;
				dst._sz = src._imageInfo._sz;
				return;
			}
			static auto ParseAtlasRes($ResPkgInfo::Res const & src, $ImageUtil::AtlasInfo & dst) {
				dst._path = src._path;
				dst._sz = src.getAtlasInfo()._sz;
				dst._texFmt = $ImageUtil::TexFmt::kUnKnown;
				dst._img.alloc(src.getAtlasInfo()._img._sz);
				for_criter(srcImageRes, src.getAtlasInfo()._img) {
					auto & dstImageInfo = dst._img.add().getLast();
					ParseImageRes(srcImageRes._val, dstImageInfo);
				}
				return;
			}
			static auto ParseAtlasList(MapOfInfo<$ResPkgInfo::Group> const & src, ListP<$ImageUtil::AtlasInfo> & dst) {
				dst.alloc(CalcAtlasSumLimit(src));
				for_criter(group, src)
					for_criter(subGroup, group._val._subGroup)
					for_criter(res, subGroup._val._res)
					if (res._val._resType == $ResPkgInfo::ResType::kAtlas)
						ParseAtlasRes(res._val, dst.add().getLast());
				dst.eraseSurplusCap();
				return;
			}
			ListOfInfo<$ImageUtil::AtlasInfo> & ParseInfo($ResPkgInfo::ResPkgInfo const & src, ListOfInfo<$ImageUtil::AtlasInfo> & dst) {
				ParseAtlasList(src._group, dst);
				return dst;
			}
		}
		namespace $RSBStructInfoToAtlasInfoList {
			static auto CalcAtlasSum(MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::Group> const & src) {
				auto sum = kSzZero;
				for_criter(group, src)
					for_criter(subGroup, group._val._subGroup)
					sum += subGroup._val._res.GetAtlasResSum();
				return sum;
			}
			static auto ParseAtlasRes($ResPkgUtil::$RSBUtil::$StructInfo::Res const & src, $ImageUtil::AtlasInfo & dst, $ImageUtil::TexFmtIdxList const & texFmtList) {
				dst._path.cpy(src._path).eraseSuffix();
				dst._sz.cpy(src.getAtlasInfo()._sz);
				dst._texFmt = $ImageUtil::GetTexFmt(texFmtList.FindTexFmt(src.getAtlasInfo()._texFmt));
				dst._img.setNull();
				return;
			}
			static auto ParseAtlasList(MapOfInfo<$ResPkgUtil::$RSBUtil::$StructInfo::Group> const & src, ListP<$ImageUtil::AtlasInfo> & dst, $ImageUtil::TexFmtIdxList const & texFmtList) {
				dst.alloc(CalcAtlasSum(src));
				for_criter(group, src)
					for_criter(subGroup, group._val._subGroup)
					for_criter(res, subGroup._val._res)
					if (res.isAtlasType())
						ParseAtlasRes(res, dst.add().getLast(), texFmtList);
				return;
			}
			ListOfInfo<$ImageUtil::AtlasInfo> & ParseInfo($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src, ListOfInfo<$ImageUtil::AtlasInfo> & dst, $ImageUtil::TexFmtIdxList const & texFmtList) {
				ParseAtlasList(src._group, dst, texFmtList);
				return dst;
			}
		}
	}
	namespace $Lite {
		$ResPkgInfo::ResPkgInfo & Lite($ResPkgInfo::ResPkgInfo const & src, $ResPkgInfo::ResPkgInfo & dst, ListB<SzI> const & resIDList) {
			dst._group.alloc(src._group._sz);
			for_criter(srcGroup, src._group) {
				auto & dstGroup = dst._group.add().getLast();
				dstGroup._key.cpy(srcGroup._key);
				dstGroup._val._isComposite = srcGroup._val._isComposite;
				dstGroup._val._subGroup.alloc(srcGroup._val._subGroup._sz);
				for_criter(srcSubGroup, srcGroup._val._subGroup)
					if (resIDList.exist(srcSubGroup._val._category._res))
						dstGroup._val._subGroup.add().getLast().cpy(srcSubGroup);
				dstGroup._val._subGroup.eraseSurplusCap();
			}
			return dst;
		}
		$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & Lite($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & dst, ListB<SzI> const & resIDList) {
			dst._headerType = src._headerType;
			dst._setResInfoListInHeader = src._setResInfoListInHeader;
			dst._useMoreAtlasInfo = src._useMoreAtlasInfo;
			dst._group.alloc(src._group._sz);
			for_criter(srcGroup, src._group) {
				auto & dstGroup = dst._group.add().getLast();
				dstGroup._key.cpy(srcGroup._key);
				dstGroup._val._isComposite = srcGroup._val._isComposite;
				dstGroup._val._subGroup.alloc(srcGroup._val._subGroup._sz);
				for_criter(srcSubGroup, srcGroup._val._subGroup)
					if (resIDList.exist(srcSubGroup._val._category._res))
						dstGroup._val._subGroup.add().getLast().cpy(srcSubGroup);
				dstGroup._val._subGroup.eraseSurplusCap();
			}
			return dst;
		}
	}
}
