#pragma once

#include "../../Type/InfoType.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/Map.hpp"
#include "../Image/Type.hpp"
#include "../ResPkg/RSB/Info.hpp"
#include "./RSBInfoX.hpp"

namespace $TJ::$Util::$Info::$RSBInfoXOri {

	inline Str const kResIDStrFmt("%u"_r);
	using $RSBInfoX::ResType;

	struct GroupInfo {

		struct SubGroupInfoItem {

			Str $id;
			Str $res;
			Str $loc;

			auto & read($JSON::Obj const & t) {
				t["id"_r].get($id);
				$res.setNull();
				if (t.exist("res"_r))
					t["res"_r].get($res);
				$loc.setNull();
				if (t.exist("loc"_r))
					t["loc"_r].get($loc);
				return thiz;
			}
			auto & write($JSON::Obj & t) const {
				t.alloc(3);
				t("id"_r).set($id);
				if ($res != kNullStr)
					t("res"_r).set($res);
				if ($loc != kNullStr)
					t("loc"_r).set($loc);
				return t;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getObj());
			}
			auto & write($JSON::Val & t) const {
				return write(t.setObj().getObj());
			}

		};

		InfoList<SubGroupInfoItem> $subgroups;

		auto & read($JSON::Obj const & t) {
			$subgroups.read(t["subgroups"_r]);
			return thiz;
		}
		auto & writeByAppend($JSON::Obj & t) const {
			t.expand(1);
			$subgroups.write(t("subgroups"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & writeByAppend($JSON::Val & t) const {
			return writeByAppend(t.getObj());
		}

	};

	struct ResBase {

		Sz $slot;
		Str $id;
		Path $path;
		Str $type;

		auto & free() {
			$slot = kNoX;
			$id.free();
			$path.free();
			$type.free();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			t["slot"_r].get($slot);
			t["id"_r].get($id);
			cnvJSONArrToPath(t["path"_r], $path);
			t["type"_r].get($type);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			t("slot"_r).set($slot);
			t("id"_r).set($id);
			cnvPathToJSONArr($path, t("path"_r));
			t("type"_r).set($type);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct ImageInfo {

		Str $parent;
		Sz $ax, $ay, $aw, $ah;
		SSz $x, $y;

		auto & read($JSON::Obj const & t) {
			t["parent"_r].get($parent);
			t["ax"_r].get($ax);
			t["ay"_r].get($ay);
			t["aw"_r].get($aw);
			t["ah"_r].get($ah);
			$x = kSNoX;
			if (t.exist("x"_r))
				t["x"_r].get($x);
			$y = kSNoX;
			if (t.exist("y"_r))
				t["y"_r].get($y);
			return thiz;
		}
		auto & writeByAppend($JSON::Obj & t) const {
			t.expand(7);
			t("parent"_r).set($parent);
			t("ax"_r).set($ax);
			t("ay"_r).set($ay);
			t("aw"_r).set($aw);
			t("ah"_r).set($ah);
			if ($x != kSNoX)
				t("x"_r).set($x);
			if ($y != kSNoX)
				t("y"_r).set($y);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & writeByAppend($JSON::Val & t) const {
			return writeByAppend(t.getObj());
		}

	};

	struct AtlasInfo {

		Sz $width, $height;

		auto & read($JSON::Obj const & t) {
			t["width"_r].get($width);
			t["height"_r].get($height);
			return thiz;
		}
		auto & writeByAppend($JSON::Obj & t) const {
			t.expand(2);
			t("width"_r).set($width);
			t("height"_r).set($height);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & writeByAppend($JSON::Val & t) const {
			return writeByAppend(t.setObj().getObj());
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
			else if ($resType == ResType::kImage)
				pvFreeS<ImageInfo>($expand);
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
		auto isImageType() const {
			return isType(ResType::kImage);
		}

		auto & getAtlasInfo() {
			return *static_cast<AtlasInfo *>($expand);
		}
		auto & getAtlasInfo() const {
			return *static_cast<AtlasInfo const *>($expand);
		}
		auto & getImageInfo() {
			return *static_cast<ImageInfo *>($expand);
		}
		auto & getImageInfo() const {
			return *static_cast<ImageInfo const *>($expand);
		}

		auto & setType(ResType const & t) {
			$resType = t;
			if (t == ResType::kNormal)
				$expand = kNullPtr;
			else if (t == ResType::kAtlas)
				$expand = new AtlasInfo();
			else if (t == ResType::kImage)
				$expand = new ImageInfo();
			return thiz;
		}
		auto & setTypeNormal() {
			return setType(ResType::kNormal);
		}
		auto & setTypeAtlas() {
			return setType(ResType::kAtlas);
		}
		auto & setTypeImage() {
			return setType(ResType::kImage);
		}

		auto & read($JSON::Obj const & t) {
			ResBase::read(t);
			if (t.exist("atlas"_r)) {
				setTypeAtlas().getAtlasInfo().read(t);
			} else if (t.exist("parent"_r)) {
				setTypeImage().getImageInfo().read(t);
			} else {
				setTypeNormal();
			}
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			ResBase::write(t);
			if ($resType == ResType::kAtlas) {
				t("atlas"_r).set(kBoT);
				getAtlasInfo().writeByAppend(t);
			} else if ($resType == ResType::kImage) {
				getImageInfo().writeByAppend(t);
			}
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
				$expand = new AtlasInfo(t.getAtlasInfo());
			} else if (t.$resType == ResType::kImage) {
				$expand = new ImageInfo(t.getImageInfo());
			}
			return thiz;
		}

		~Res() {
			freeExpand();
		}

	};

	struct SubGroupInfo {

		Str $parent;
		Str $res;
		Str $loc;
		InfoList<Res> $resources;

		auto & read($JSON::Obj const & t) {
			$parent.setNull();
			if (t.exist("parent"_r))
				t["parent"_r].get($parent);
			$res.setNull();
			if (t.exist("res"_r))
				t["res"_r].get($res);
			$loc.setNull();
			if (t.exist("loc"_r))
				t["loc"_r].get($loc);
			$resources.read(t["resources"_r]);
			return thiz;
		}
		auto & writeByAppend($JSON::Obj & t) const {
			t.expand(4);
			if (!$parent.isNull())
				t("parent"_r).set($parent);
			if (!$res.isNull())
				t("res"_r).set($res);
			if (!$loc.isNull())
				t("loc"_r).set($loc);
			$resources.write(t("resources"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & writeByAppend($JSON::Val & t) const {
			return writeByAppend(t.getObj());
		}

	};

	struct GroupItem {

		Str $id;
		Str $type;
		Void * $expand;

		auto & freeExpand() {
			if ($type == "composite"_r)
				pvFreeS<GroupInfo>($expand);
			else if ($type == "simple"_r)
				pvFreeS<SubGroupInfo>($expand);
			$type.free();
			return thiz;
		}
		auto & free() {
			$id.free();
			freeExpand();
			return thiz;
		}

		auto & getGroupInfo() {
			return *static_cast<GroupInfo *>($expand);
		}
		auto & getGroupInfo() const {
			return *static_cast<GroupInfo const *>($expand);
		}
		auto & getSubGroupInfo() {
			return *static_cast<SubGroupInfo *>($expand);
		}
		auto & getSubGroupInfo() const {
			return *static_cast<SubGroupInfo const *>($expand);
		}

		auto & read($JSON::Obj const & t) {
			t["id"_r].get($id);
			t["type"_r].get($type);
			if ($type == "composite"_r) {
				$expand = new GroupInfo();
				getGroupInfo().read(t);
			} else if ($type == "simple"_r) {
				$expand = new SubGroupInfo();
				getSubGroupInfo().read(t);
			} else {
				throw VAMGException("RSBInfoXOri", "unknown group-item type");
			}
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			t("id"_r).set($id);
			t("type"_r).set($type);
			if ($type == "composite"_r)
				getGroupInfo().writeByAppend(t);
			else if ($type == "simple"_r)
				getSubGroupInfo().writeByAppend(t);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & operator=(GroupItem const & t) {
			$id = t.$id;
			$type = t.$type;
			if ($type == "composite"_r) {
				$expand = new GroupInfo(t.getGroupInfo());
			} else if ($type == "simple"_r) {
				$expand = new SubGroupInfo(t.getSubGroupInfo());
			}
			return thiz;
		}

		~GroupItem() {
			freeExpand();
		}

	};

	struct Info {

		Sz $slot_count;
		InfoList<GroupItem> $groups;

		auto & read($JSON::Obj const & t) {
			t["slot_count"_r].get($slot_count);
			$groups.read(t["groups"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(3);
			t("version"_r).set(1);
			t("slot_count"_r).set($slot_count);
			$groups.write(t("groups"_r));
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
