#pragma once

#include "./Struct.hpp"
#include "./Common.hpp"
#include "./RSGP/Info.hpp"

#include "../../../Type.hpp"
#include "../../../UserLiteral.hpp"
#include "../../../Type/InfoType.hpp"
#include "../../../Type/ImgType.hpp"
#include "../../../Type/JSON.hpp"
#include "../../Image/Type.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$Info {

	struct AtlasInfo {

		ImgSz $sz;
		Sz $fmt;

		auto & read($JSON::Obj const & t) {
			$sz.read(t["sz"_r]);
			t["fmt"_r].get($fmt);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$sz.write(t("sz"_r));
			t("fmt"_r).set($fmt);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct Res {

		Path $path;
		ResType $type;
		Void * $expand;

		auto & freeExpand() {
			if ($type == ResType::kNormal)
				$expand = kNullPtr;
			else if ($type == ResType::kAtlas)
				pvFreeS<AtlasInfo>($expand);
			$type = ResType::kNormal;
			return thiz;
		}
		auto & free() {
			$path.free();
			freeExpand();
			return thiz;
		}

		auto isType(ResType const & t) const {
			return $type == t;
		}
		auto isNormalType() const {
			return isType(ResType::kNormal);
		}
		auto isAtlasType() const {
			return isType(ResType::kAtlas);
		}

		auto & getAtlasInfo() {
			return *static_cast<AtlasInfo *>($expand);
		}
		auto & getAtlasInfo() const {
			return *static_cast<AtlasInfo const *>($expand);
		}

		auto & setType(ResType const & t) {
			$type = t;
			if (t == ResType::kNormal)
				$expand = kNullPtr;
			else if (t == ResType::kAtlas)
				$expand = new AtlasInfo();
			return thiz;
		}
		auto & setTypeNormal() {
			return setType(ResType::kNormal);
		}
		auto & setTypeAtlas() {
			return setType(ResType::kAtlas);
		}

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			if (!t.exist("atlas"_r))
				setTypeNormal();
			else
				setTypeAtlas().getAtlasInfo().read(t["atlas"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			if ($type == ResType::kAtlas)
				getAtlasInfo().write(t("atlas"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & operator=(Res const & t) {
			$path = t.$path;
			$type = t.$type;
			if (t.$type == ResType::kAtlas) {
				$expand = new AtlasInfo();
				getAtlasInfo() = t.getAtlasInfo();
			}
			return thiz;
		}

		~Res() {
			freeExpand();
		}

	};

	struct ResList : InfoList<Res> {

		auto calcResSum(ResType const & ty) const {
			return range().eachSum(
				[&ty](auto & v) {
					return v.$type == ty ? kSz1 : kSzN;
				}, kSzN
			);
		}

		auto calcNormalResSum() const {
			return calcResSum(ResType::kNormal);
		}
		auto calcAtlasResSum() const {
			return calcResSum(ResType::kAtlas);
		}

	};

	struct SubGroup {

		Category $category;
		Sz $headerType;
		ResStoreMethod $resStoreMethod;
		ResList $res;

		auto & init() {
			$headerType = 0;
			$resStoreMethod.set(kBoF, kBoF);
			$res.setNull();
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			$category.read(t["category"_r]);
			t["headerType"_r].get($headerType);
			$resStoreMethod.read(t["resStoreMethod"_r]);
			$res.read(t["res"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			$category.write(t("category"_r));
			t("headerType"_r).set($headerType);
			$resStoreMethod.write(t("resStoreMethod"_r));
			$res.write(t("res"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & toRSGPInfo($RSGP::$Info::Info & rsgpInfo) const {
			rsgpInfo.init();
			rsgpInfo.$headerType = $headerType;
			rsgpInfo.$resStoreMethod = $resStoreMethod;
			rsgpInfo.$res.alloc($res.$sz);
			auto atlasIdx = kNo1;
			for (auto const & res : $res) {
				auto & dstRes = rsgpInfo.$res.appendX().last();
				dstRes.$path.cpy(res.$path);
				if (res.$type == ResType::kNormal) {
					dstRes.setTypeNormal();
				} else if (res.$type == ResType::kAtlas) {
					dstRes.setTypeAtlas();
					dstRes.getAtlasInfo().$idx = atlasIdx;
					dstRes.getAtlasInfo().$sz = res.getAtlasInfo().$sz;
					++atlasIdx;
				}
			}
			return rsgpInfo;
		}

	};

	struct Group {

		Bool $composite;
		InfoMap<SubGroup> $subGroup;

		auto & read($JSON::Obj const & t) {
			t["composite"_r].get($composite);
			$subGroup.read(t["subGroup"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			t("composite"_r).set($composite);
			$subGroup.write(t("subGroup"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct Info {

		Sz $headerType;
		Bool $enableEmbeddedResInfo;
		Bool $enableAtlasInfoExpand;
		InfoMap<Group> $group;

		auto & init() {
			$headerType = 0;
			$enableEmbeddedResInfo = kBoF;
			$enableAtlasInfoExpand = kBoF;
			$group.setNull();
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			t["headerType"_r].get($headerType);
			t["enableEmbeddedResInfo"_r].get($enableEmbeddedResInfo);
			t["enableAtlasInfoExpand"_r].get($enableAtlasInfoExpand);
			$group.read(t["group"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			t("headerType"_r).set($headerType);
			t("enableEmbeddedResInfo"_r).set($enableEmbeddedResInfo);
			t("enableAtlasInfoExpand"_r).set($enableAtlasInfoExpand);
			$group.write(t("group"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & cntSum(Sz & groupSum, Sz & subGroupSum, Sz & resSum, Sz & atlasResSum) const {
			groupSum = $group.$sz;
			subGroupSum = kSzN;
			resSum = kSzN;
			atlasResSum = kSzN;
			for (auto const & group : $group) {
				subGroupSum += group.$v.$subGroup.$sz;
				for (auto const & subGroup : group.$v.$subGroup) {
					resSum += subGroup.$v.$res.$sz;
					atlasResSum += subGroup.$v.$res.calcAtlasResSum();
				}
			}
			return thiz;
		}

	};

	namespace $PartInfo {

		struct PartInfo {

			Str $id;
			List<Str> $group;

			auto & read($JSON::Obj const & t) {
				t["id"_r].get($id);
				$JSON::getListFromArr(t["group"_r], $group);
				return thiz;
			}
			auto & write($JSON::Obj & t) const {
				t.alloc(2);
				t("id"_r).set($id);
				$JSON::setListToArr($group, t("group"_r));
				return t;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}
			auto & write($JSON::Val & t) const {
				return write(t.setObj().getObj());
			}

		};

		using PartInfoList = InfoList<PartInfo>;

	}

	/*
	namespace $ResInfoList {

		struct Res_Append {

			Str $k;
			Str $v;
			IU32 $unk;

			auto & read($JSON::Obj const & t) {
				t.first().$k.get($k);
				t.first().$v.get($v);
				t["unk"_r].get($unk);
				return thiz;
			}
			auto & write($JSON::Obj & t) const {
				t.alloc(2);
				t($k).set($v);
				t("unk"_r).set($unk);
				return t;
			}

		};

		struct Res_Expand {

			Sz _unKnown_A;
			Sz _unKnown_C;
			Sz _unKnown_D;
			Str $parent;
			Sz _flag;
			$Image::ImagePos _pos;
			$ImageUtil::ImageSize _sz;
			$ImageUtil::ImageSgdPos _padding;

			auto & read($JSON::Obj const & src) {
				src["UnKnown_A"].getX(_unKnown_A);
				src["UnKnown_C"].getX(_unKnown_C);
				src["UnKnown_D"].getX(_unKnown_D);
				src["Parent"].getX($parent, kNullStr);
				goif(_unKnown_A == 2)
					_sz.read(src["AtlasInfo"].getArr());
				elif(_unKnown_A == 4) {
					src["Flag"].getX(_flag);
					_pos.read(src["ImageInfo"].getArr(), kThFirst, kThSecond);
					_sz.read(src["ImageInfo"].getArr(), kThThird, kThFourth);
					_padding.read(src["ImageInfo"].getArr(), kThFifth, kThSixth);
				}
				return thiz;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("UnKnown_A", _unKnown_A);
				dst.add("UnKnown_C", _unKnown_C);
				dst.add("UnKnown_D", _unKnown_D);
				dst.add("Parent", $parent, $parent.isNull());
				goif(_unKnown_A == 2)
					_sz.write(dst.addArr("AtlasInfo").last()._val.getArr());
				elif(_unKnown_A == 4) {
					dst.add("Flag", _flag);
					auto & imageInfo = dst.addArr("ImageInfo").last()._val.getArr();
					_pos.write(imageInfo);
					_sz.write(imageInfo);
					_padding.write(imageInfo);
				}
				return dst;
			}

		};

		struct Res {
			Str $id;
			Path _path;
			Str $type;
			Bool _existExpand;
			Res_Expand _expand;
			InfoList<Res_Append> $expand;

			auto & cpy(Res const & src) {
				$id = src.$id;
				_path = src._path;
				$type = src.$type;
				_existExpand = src._existExpand;
				_expand.cpy(src._expand);
				$expand = src.$expand;
				return thiz;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX($id);
				_path.read(src["Path"].getArr());
				src["Type"].getX($type);
				if (_existExpand)
					_expand.read(src["Expand"].getObj());
				$expand.read(src["Append"].getArr());
				return thiz;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", $id);
				_path.write(dst.addArr("Path").last()._val.getArr());
				dst.add("Type", $type);
				if (_existExpand)
					_expand.write(dst.addObj("Expand").last()._val.getObj());
				$expand.write(dst.addArr("Append").last()._val.getArr());
				return dst;
			}
		};
		struct SubGroup {
			Str $id;
			Category $category;
			ListOfInfo<Res> $res;

			auto & cpy(SubGroup const & src) {
				$id = src.$id;
				$category = src.$category;
				$res = src.$res;
				return thiz;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX($id);
				$category.read(src["Category"].getArr());
				$res.read(src["Res"].getArr());
				return thiz;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", $id);
				$category.write(dst.addArr("Category").last()._val.getArr());
				$res.write(dst.addArr("Res").last()._val.getArr());
				return dst;
			}
		};
		struct Group {
			Str $id;
			UI32 _unKnown;
			ListOfInfo<SubGroup> $subGroup;

			auto & cpy(Group const & src) {
				$id = src.$id;
				_unKnown = src._unKnown;
				$subGroup = src.$subGroup;
				return thiz;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX($id);
				src["UnKnown"].getX(_unKnown);
				$subGroup.read(src["SubGroup"].getArr());
				return thiz;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", $id);
				dst.add("UnKnown", _unKnown);
				$subGroup.write(dst.addArr("SubGroup").last()._val.getArr());
				return dst;
			}
		};
		struct ResInfoList {
			ListOfInfo<Group> $group;

			auto & cpy(ResInfoList const & src) {
				$group = src.$group;
				return thiz;
			}
			auto & read($JSON::Obj const & src) {
				$group.read(src["Group"].getArr());
				return thiz;
			}
			auto & write($JSON::Obj & dst) const {
				$group.write(dst.addArr("Group").last()._val.getArr());
				return dst;
			}
		};

	}
*/

}
