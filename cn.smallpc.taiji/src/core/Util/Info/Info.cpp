#include "./Info.hpp"

namespace $TJ::$Util::$Info {

	namespace $Filter {

		$RSBInfoX::Info & filter($RSBInfoX::Info const & src, $RSBInfoX::Info & dst, List<Sz> const & resIDList) {
			dst.$group.alloc(src.$group.$sz);
			for (auto const & srcGroup : src.$group) {
				auto & dstGroup = dst.$group.appendX().last();
				dstGroup.$k = srcGroup.$k;
				dstGroup.$v.$composite = srcGroup.$v.$composite;
				dstGroup.$v.$subGroup.alloc(srcGroup.$v.$subGroup.$sz);
				for (auto const & srcSubGroup : srcGroup.$v.$subGroup)
					if (resIDList.range().exist(srcSubGroup.$v.$category.$res))
						dstGroup.$v.$subGroup.append(srcSubGroup);
				dstGroup.$v.$subGroup.lite();
			}
			return dst;
		}

		$ResPkg::$RSB::$Info::Info & filter($ResPkg::$RSB::$Info::Info const & src, $ResPkg::$RSB::$Info::Info & dst, List<Sz> const & resIDList) {
			dst.$headerType = src.$headerType;
			dst.$enableAtlasInfoExpand = src.$enableAtlasInfoExpand;
			dst.$enableEmbeddedResInfo = src.$enableEmbeddedResInfo;
			dst.$group.alloc(src.$group.$sz);
			for (auto const & srcGroup : src.$group) {
				auto & dstGroup = dst.$group.appendX().last();
				dstGroup.$k = srcGroup.$k;
				dstGroup.$v.$composite = srcGroup.$v.$composite;
				dstGroup.$v.$subGroup.alloc(srcGroup.$v.$subGroup.$sz);
				for (auto const & srcSubGroup : srcGroup.$v.$subGroup)
					if (resIDList.range().exist(srcSubGroup.$v.$category.$res))
						dstGroup.$v.$subGroup.append(srcSubGroup);
				dstGroup.$v.$subGroup.lite();
			}
			return dst;
		}

	}

	namespace $Cnv {

		namespace $RSBInfoXFromOri {

			inline auto cnvResBase($RSBInfoXOri::ResBase const & src, Str & dstID, $RSBInfoX::ResBase & dst) {
				dstID = src.$id;
				dst.$path = src.$path;
				dst.$type = src.$type;
				return;
			}
			inline auto cnvImageInfo($RSBInfoXOri::ImageInfo const & src, $RSBInfoX::ImageInfo & dst) {
				dst.$area.set(src.$ax, src.$ay, src.$aw, src.$ah);
				dst.$padding.set(src.$x, src.$y);
				return;
			}
			inline auto cnvAtlasInfo($RSBInfoXOri::AtlasInfo const & src, $RSBInfoX::AtlasInfo & dst) {
				dst.$sz.set(src.$width, src.$height);
				dst.$img.alloc();
				return;
			}
			static auto cnvResList(List<$RSBInfoXOri::Res> const & src, InfoMap<$RSBInfoX::Res> & dst) {
				dst.alloc(src.$sz);
				for (auto const & srcRes : src) {
					switch (srcRes.$resType) {
						default:
							throw VAMGException("Info::Cnv::RSBInfoXFromOri", "unknown res-type");
							break;
						case $RSBInfoXOri::ResType::kNormal: {
							auto & t = dst.appendX().last();
							t.$v.setTypeNormal();
							cnvResBase(srcRes, t.$k, t.$v);
							break;
						}
						case $RSBInfoXOri::ResType::kAtlas: {
							auto & t = dst.appendX().last();
							t.$v.setTypeAtlas();
							cnvResBase(srcRes, t.$k, t.$v);
							cnvAtlasInfo(srcRes.getAtlasInfo(), t.$v.getAtlasInfo());
							break;
						}
						case $RSBInfoXOri::ResType::kImage:
							if (Sz i; dst.findKey(srcRes.getImageInfo().$parent, i)) {
								auto & t = dst.get(i).$v.getAtlasInfo().$img.appendX().last();
								cnvResBase(srcRes, t.$k, t.$v);
								cnvImageInfo(srcRes.getImageInfo(), t.$v.$img);
							} else {
								throw VAMGException("Info::Cnv::RSBInfoXFromOri", "can't found parent-atlas : %s", srcRes.getImageInfo().$parent.$data);
							}
							break;
					}
				}
				return;
			}
			static auto cnvSubGroup($RSBInfoXOri::SubGroupInfo const & src, $RSBInfoX::SubGroup & dst, Str const & id) {
				dst.$category.parseRaw(src.$res, src.$loc);
				cnvResList(src.$resources, dst.$res);
				return;
			}
			static auto cnvGroupList(List<$RSBInfoXOri::GroupItem> const & src, InfoMap<$RSBInfoX::Group> & dst) {
				dst.alloc(src.$sz);
				for (auto const & srcGroup : src) {
					if (srcGroup.$type == "composite"_r) {
						auto & t = dst.appendX().last();
						t.$k = srcGroup.$id;
						t.$v.$composite = kBoT;
						t.$v.$subGroup.alloc();
					} else if (srcGroup.$type == "simple"_r) {
						if (srcGroup.getSubGroupInfo().$parent.isNull()) {
							auto & t = dst.appendX().last();
							t.$k = srcGroup.$id;
							t.$v.$composite = kBoF;
							t.$v.$subGroup.allocThenUse(kSz1);
							t.$v.$subGroup.first().$k = srcGroup.$id;
							cnvSubGroup(srcGroup.getSubGroupInfo(), t.$v.$subGroup.first().$v, srcGroup.$id);
						} else {
							if (Sz i; dst.findKey(srcGroup.getSubGroupInfo().$parent, i)) {
								auto & t = dst.get(i).$v.$subGroup.appendX().last();
								t.$k = srcGroup.$id;
								cnvSubGroup(srcGroup.getSubGroupInfo(), t.$v, srcGroup.$id);
							} else {
								throw VAMGException("Info::Cnv::RSBInfoXFromOri", "can't found parent-group : %s", srcGroup.getSubGroupInfo().$parent.$data);
							}
						}
					}
				}
				return;
			}
			$RSBInfoX::Info & cnv($RSBInfoXOri::Info const & src, $RSBInfoX::Info & dst) {
				cnvGroupList(src.$groups, dst.$group);
				return dst;
			}

		}

		$RSBInfoX::Info & cnv($RSBInfoXOri::Info const & src, $RSBInfoX::Info & dst) {
			return $RSBInfoXFromOri::cnv(src, dst);
		}

		namespace $RSBInfoXToOri {

			inline auto cnvResBase(Str const & srcID, $RSBInfoX::ResBase const & src, $RSBInfoXOri::ResBase & dst) {
				dst.$slot = kNoX;
				dst.$id = srcID;
				dst.$path = src.$path;
				dst.$type = src.$type;
				return;
			}
			inline auto cnvImageInfo($RSBInfoX::ImageInfo const & src, $RSBInfoXOri::ImageInfo & dst, Str const & parent) {
				dst.$parent = parent;
				dst.$ax = src.$area.$pos.$x;
				dst.$ay = src.$area.$pos.$y;
				dst.$aw = src.$area.$sz.$w;
				dst.$ah = src.$area.$sz.$h;
				dst.$x = src.$padding.$x;
				dst.$y = src.$padding.$y;
				return;
			}
			inline auto cnvAtlasInfo($RSBInfoX::AtlasInfo const & src, $RSBInfoXOri::AtlasInfo & dst) {
				dst.$width = src.$sz.$w;
				dst.$height = src.$sz.$h;
				return;
			}
			static auto cnvAtlasInfoImageList(InfoMap<$RSBInfoX::ImageRes> const & src, List<$RSBInfoXOri::Res> & dst, Str const & parent) {
				for (auto const & e : src) {
					auto & t = dst.appendX().last();
					t.setTypeImage();
					cnvResBase(e.$k, e.$v, t);
					cnvImageInfo(e.$v.$img, t.getImageInfo(), parent);
				}
				return;
			}
			static auto cnvResList(InfoMap<$RSBInfoX::Res> const & src, List<$RSBInfoXOri::Res> & dst) {
				dst.alloc(src.$sz);
				for (auto const & e : src) {
					auto & t = dst.appendX().last();
					switch (e.$v.$resType) {
						default:
							throw VAMGException("Info::Cnv::RSBInfoXToOri", "unknown res-type");
							break;
						case $RSBInfoX::ResType::kNormal:
							t.setTypeNormal();
							cnvResBase(e.$k, e.$v, t);
							break;
						case $RSBInfoX::ResType::kAtlas:
							t.setTypeAtlas();
							cnvResBase(e.$k, e.$v, t);
							cnvAtlasInfo(e.$v.getAtlasInfo(), t.getAtlasInfo());
							dst.expand(e.$v.getAtlasInfo().$img.$sz);
							cnvAtlasInfoImageList(e.$v.getAtlasInfo().$img, dst, e.$k);
							break;
					}
				}
				return;
			}
			static auto cnvSubGroupInfo($RSBInfoX::SubGroup const & src, $RSBInfoXOri::SubGroupInfo & dst, Str const & parent) {
				dst.$parent = parent;
				src.$category.parseToRaw(dst.$res, dst.$loc);
				cnvResList(src.$res, dst.$resources);
				return;
			}
			static auto cnvSubGroupList(InfoMap<$RSBInfoX::SubGroup> const & src, List<$RSBInfoXOri::GroupItem> & dst, Str const & parent) {
				for (auto const & e : src) {
					auto & t = dst.appendX().last();
					t.$id = e.$k;
					t.$type = "simple"_s;
					t.$expand = new $RSBInfoXOri::SubGroupInfo();
					cnvSubGroupInfo(e.$v, t.getSubGroupInfo(), parent);
				}
				return;
			}
			static auto cnvGroupInfoSubGroupList(InfoMap<$RSBInfoX::SubGroup> const & src, List<$RSBInfoXOri::GroupInfo::SubGroupInfoItem> & dst) {
				dst.alloc(src.$sz);
				for (auto const & srcSubGroup : src) {
					auto & dstSubGroup = dst.appendX().last();
					dstSubGroup.$id = srcSubGroup.$k;
					srcSubGroup.$v.$category.parseToRaw(dstSubGroup.$res, dstSubGroup.$loc);
				}
				return;
			}
			static auto calcGroupItemSumLimit(InfoMap<$RSBInfoX::Group> const & src) {
				auto sum = src.$sz;
				for (auto const & group : src)
					sum += group.$v.$subGroup.$sz;
				return sum;
			}
			static auto cnvGroupList(InfoMap<$RSBInfoX::Group> const & src, List<$RSBInfoXOri::GroupItem> & dst) {
				dst.alloc(calcGroupItemSumLimit(src));
				for (auto const & srcGroup : src) {
					if (!srcGroup.$v.$composite)
						cnvSubGroupList(srcGroup.$v.$subGroup, dst, Str().setNull());
					else {
						auto & dstGroup = dst.appendX().last();
						dstGroup.$id.cpy(srcGroup.$k);
						dstGroup.$type = "composite"_s;
						dstGroup.$expand = new $RSBInfoXOri::GroupInfo();
						cnvGroupInfoSubGroupList(srcGroup.$v.$subGroup, dstGroup.getGroupInfo().$subgroups);
						cnvSubGroupList(srcGroup.$v.$subGroup, dst, srcGroup.$k);
					}
				}
				return;
			}
			static auto calcResSumLimit($RSBInfoXOri::Info const & t) {
				auto sum = kSzN;
				for (auto const & e : t.$groups)
					if (e.$type == "simple"_r)
						sum += e.getSubGroupInfo().$resources.$sz;
				return sum;
			}
			static auto allocSlot($RSBInfoXOri::Info & dst, Sz const & begNum) {
				List<Str> resIDList;
				resIDList.alloc(calcResSumLimit(dst));
				for (auto const & group : dst.$groups) {
					if (group.$type == "simple"_r) {
						for (auto & res : group.getSubGroupInfo().$resources) {
							if (Sz i; resIDList.range().find(res.$id, i))
								res.$slot = begNum + i;
							else
								res.$slot = begNum + resIDList.append(res.$id).lastIdx();
						}
					}
				}
				dst.$slot_count = begNum + resIDList.$sz;
				resIDList.free();
				return;
			}
			$RSBInfoXOri::Info & cnv($RSBInfoX::Info const & src, $RSBInfoXOri::Info & dst, Sz const & slotBegNum) {
				cnvGroupList(src.$group, dst.$groups);
				allocSlot(dst, slotBegNum);
				return dst;
			}

		}

		$RSBInfoXOri::Info & cnv($RSBInfoX::Info const & src, $RSBInfoXOri::Info & dst, Sz const & slotBegNum) {
			return $RSBInfoXToOri::cnv(src, dst, slotBegNum);
		}

		$ResPkg::$RSB::$Info::Info & cnv($RSBInfoX::Info const & src, $ResPkg::$RSB::$Info::Info & dst, $Image::TexFmtIdxMap const & texFmtIdx) {
			dst.$group.alloc(src.$group.$sz);
			for (auto const & e : src.$group) {
				auto & t = dst.$group.appendX().last();
				t.$k = e.$k;
				t.$v.$composite = e.$v.$composite;
				t.$v.$subGroup.alloc(e.$v.$subGroup.$sz);
				for (auto const & ee : e.$v.$subGroup) {
					auto & tt = t.$v.$subGroup.appendX().last();
					tt.$k = ee.$k;
					tt.$v.$category = ee.$v.$category;
					tt.$v.$resStoreMethod.set(kBoT, kBoT);
					tt.$v.$res.alloc(ee.$v.$res.$sz);
					for (auto const & eee : ee.$v.$res) {
						auto & ttt = tt.$v.$res.appendX().last();
						ttt.$path.cpy(eee.$v.$path);
						switch (eee.$v.$resType) {
							default:
								throw VAMGException("Info::Cnv::RSBInfoXToRSBInfo", "unknown res-type");
								break;
							case $RSBInfoX::ResType::kNormal:
								ttt.setTypeNormal();
								break;
							case $RSBInfoX::ResType::kAtlas:
								ttt.setTypeAtlas();
								ttt.$path = Path(eee.$v.$path).setExtension("ptx"_r);
								ttt.getAtlasInfo().$sz = eee.$v.getAtlasInfo().$sz;
								ttt.getAtlasInfo().$fmt = texFmtIdx.findTexFmt(kEmptyStr);
								break;
						}
					}
				}
			}
			return dst;
		}

		InfoList<$Image::AtlasInfo> & cnv($RSBInfoX::Info const & src, InfoList<$Image::AtlasInfo> & dst) {
			{
				auto sum = kSzN;
				for (auto const & e : src.$group)
					for (auto const & ee : e.$v.$subGroup)
						sum += ee.$v.$res.$sz;
				dst.alloc(sum);
			}
			for (auto const & e : src.$group)
				for (auto const & ee : e.$v.$subGroup)
					for (auto const & eee : ee.$v.$res)
						if (eee.$v.isAtlasType()) {
							[]($RSBInfoX::Res const & src, $Image::AtlasInfo & dst) {
								dst.$path = src.$path;
								dst.$sz = src.getAtlasInfo().$sz;
								dst.$fmt = $Image::TexFmt::kNull;
								dst.$img.alloc(src.getAtlasInfo().$img.$sz);
								for (auto const & e : src.getAtlasInfo().$img) {
									auto & t = dst.$img.appendX().last();
									t.$path = e.$v.$path;
									t.$area = e.$v.$img.$area;
								}
							}(eee.$v, dst.appendX().last());
						}
			return dst;
		}

		InfoList<$Image::AtlasInfo> & cnv($ResPkg::$RSB::$Info::Info const & src, InfoList<$Image::AtlasInfo> & dst, $Image::TexFmtIdxMap const & texFmtIdx) {
			{
				auto sum = kSzN;
				for (auto const & e : src.$group)
					for (auto const & ee : e.$v.$subGroup)
						sum += ee.$v.$res.calcAtlasResSum();
				dst.alloc(sum);
			}
			for (auto const & e : src.$group)
				for (auto const & ee : e.$v.$subGroup)
					for (auto const & eee : ee.$v.$res)
						if (eee.isAtlasType()) {
							[&texFmtIdx]($ResPkg::$RSB::$Info::Res const & src, $Image::AtlasInfo & dst) {
								dst.$path = Path(src.$path).eraseExtension();
								dst.$sz = src.getAtlasInfo().$sz;
								dst.$fmt = $Image::getTexFmt(texFmtIdx.findTexFmt(src.getAtlasInfo().$fmt));
								dst.$img.setNull();
							}(eee, dst.appendX().last());
						}
			return dst;
		}

		$ResPkg::$RSB::$RSGP::$Info::Info & cnv($RSBInfoX::SubGroup const & src, $ResPkg::$RSB::$RSGP::$Info::Info & dst) {
			dst.$headerType = 0u; // TODO
			dst.$resStoreMethod.set(kBoT, kBoT);
			dst.$res.alloc(src.$res.$sz);
			for (auto const & e : src.$res) {
				auto & t = dst.$res.appendX().last();
				t.$path = e.$v.$path;
				switch (e.$v.$resType) {
					default:
						throw VAMGException("Info::Cnv::RSBInfoXSubGroupToRSGPInfo", "unknown res-type");
						break;
					case $RSBInfoX::ResType::kNormal:
						t.setTypeNormal();
						break;
					case $RSBInfoX::ResType::kAtlas:
						t.setTypeAtlas();
						t.getAtlasInfo().$idx = dst.$res.calcAtlasResSum() - kNo2;
						t.getAtlasInfo().$sz = e.$v.getAtlasInfo().$sz;
						break;
				}
			}
			return dst;
		}

	}

}
