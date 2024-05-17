#pragma once

#include "../../Type/InfoType.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/Map.hpp"
#include "../Image/Type.hpp"
#include "../ResPkg/RSB/Info.hpp"

namespace $TJ::$Util::$Info::$RSBInfoX {

	using $ResPkg::$RSB::Category;

	enum class ResType : IU8 { kNormal, kAtlas, kImage };

	struct ResBase {

		Path $path;
		Str $type;

		auto & free() {
			$path.free();
			$type.free();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			t["type"_r].get($type, kNullStr);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			t("type"_r).set($type, $type.isNull());
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		ResBase & operator=(ResBase const & t) = default;

	};

	struct ImageInfo {

		ImgArea $area;
		ImgSgnPos $padding;

		auto & read($JSON::Obj const & t) {
			$area.read(t["area"_r]);
			$padding.read(t["padding"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$area.write(t("area"_r));
			$padding.write(t("padding"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct ImageRes : ResBase {

		ImageInfo $img;

		auto & read($JSON::Obj const & t) {
			ResBase::read(t);
			$img.read(t["image"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			ResBase::write(t);
			$img.write(t("image"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct AtlasInfo {

		ImgSz $sz;
		InfoMap<ImageRes> $img;

		auto & read($JSON::Obj const & src) {
			$sz.read(src["sz"_r]);
			$img.read(src["img"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$sz.write(t("sz"_r));
			$img.write(t("img"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct Res : ResBase {

		ResType $resType;
		Void * $expand;

		auto & freeExpand() {
			if ($resType == ResType::kNormal)
				$expand = kNullPtr;
			else if ($resType == ResType::kAtlas)
				pvFreeS<AtlasInfo>($expand);
			$resType = ResType::kNormal;
			return thiz;
		}
		auto & free() {
			ResBase::free();
			freeExpand();
			return thiz;
		}

		auto isType(ResType const & t) const {
			return $resType == t;
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
			$resType = t;
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
			ResBase::read(t);
			if (!t.exist("atlas"_r))
				setTypeNormal();
			else
				setTypeAtlas().getAtlasInfo().read(t["atlas"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			ResBase::write(t);
			if ($resType == ResType::kAtlas)
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
			ResBase::operator=(t);
			$resType = t.$resType;
			if (t.$resType == ResType::kAtlas) {
				$expand = new AtlasInfo();
				getAtlasInfo() = t.getAtlasInfo();
			}
			return thiz;
		}

		~Res() {
			freeExpand();
		}

	};

	struct SubGroup {

		Category $category;
		InfoMap<Res> $res;

		auto & read($JSON::Obj const & t) {
			$category.read(t["category"_r]);
			$res.read(t["res"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$category.write(t("category"_r));
			$res.write(t("res"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
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

		InfoMap<Group> $group;

		auto & read($JSON::Obj const & t) {
			$group.read(t["group"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(1);
			$group.write(t("group"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

}
