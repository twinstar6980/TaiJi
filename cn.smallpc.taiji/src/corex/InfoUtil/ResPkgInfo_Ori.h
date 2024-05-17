# pragma once

# include "../MyType/ListOfOther.h"
# include "../MyType/JSON.h"
# include "./ResPkgInfo.h"

namespace $TJ::$InfoUtil::$ResPkgInfo_Ori {
	Str const kResIDStrFmt("%u");
	struct subgroups_info_item {
		Str _id;
		Str _res;
		Str _loc;

		auto & cpy(subgroups_info_item const & src) {
			_id = src._id;
			_res = src._res;
			_loc = src._loc;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["id"].getX(_id);
			src.getByStrIf("res")._val.getX(_res, kNullStr);
			src.getByStrIf("loc")._val.getX(_loc, kNullStr);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("id", _id);
			if (!_res.isNull())
				dst.add("res", _res);
			if (!_loc.isNull())
				dst.add("loc", _loc);
			return dst;
		}
	};
	struct group_info_append {
		ListOfInfo<subgroups_info_item> _subgroups;

		auto & cpy(group_info_append const & src) {
			_subgroups = src._subgroups;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_subgroups.read(src["subgroups"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_subgroups.write(dst.addArr("subgroups").last()._val.getArr());
			return dst;
		}
	};
	struct resource_item_base {
		SzI _slot;
		Str _id;
		Path _path;
		Str _type;

		auto & tfree() {
			_id.tfree();
			_path.tfree();
			_type.tfree();
			return *this;
		}
		auto & cpy(resource_item_base const & src) {
			_slot = src._slot;
			_id = src._id;
			_path = src._path;
			_type = src._type;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["slot"].getX(_slot);
			src["id"].getX(_id);
			_path.read(src["path"].getArr());
			src["type"].getX(_type);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("slot", _slot);
			dst.add("id", _id);
			_path.write(dst.addArr("path").last()._val.getArr());
			dst.add("type", _type);
			return dst;
		}
		protected:
		auto & getBase() { return *this; }
		auto & getBase() const { return *this; }
	};
	struct image_info_append {
		Str _parent;
		SzI _ax, _ay;
		SzI _aw, _ah;
		SzSI _x, _y;

		auto & cpy(image_info_append const & src) {
			_parent = src._parent;
			_ax = src._ax;
			_ay = src._ay;
			_aw = src._aw;
			_ah = src._ah;
			_x = src._x;
			_y = src._y;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["parent"].getX(_parent);
			src["ax"].getX(_ax);
			src["ay"].getX(_ay);
			src["aw"].getX(_aw);
			src["ah"].getX(_ah);
			src.getByStrIf("x")._val.getX(_x, kSzSNull);
			src.getByStrIf("y")._val.getX(_y, kSzSNull);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("parent", _parent);
			dst.add("ax", _ax);
			dst.add("ay", _ay);
			dst.add("aw", _aw);
			dst.add("ah", _ah);
			dst.add("x", _x == kSzSNull ? kSzSZero : _x);
			dst.add("y", _y == kSzSNull ? kSzSZero : _y);
			return dst;
		}
	};
	struct atlas_info_append {
		SzI _width, _height;

		auto & cpy(atlas_info_append const & src) {
			_width = src._width;
			_height = src._height;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["width"].getX(_width);
			src["height"].getX(_height);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("width", _width);
			dst.add("height", _height);
			return dst;
		}
	};
	struct resource_item : resource_item_base {
		$ResPkgInfo::ResType _resType;
		Void * _append;

		~resource_item() {
			goif(_resType == $ResPkgInfo::ResType::kAtlas)
				safeDelVoidPtrSingle<atlas_info_append>(_append);
			elif(_resType == $ResPkgInfo::ResType::kImage)
				safeDelVoidPtrSingle<image_info_append>(_append);
			return;
		}
		auto & tfree() {
			getBase().tfree();
			goif(_resType == $ResPkgInfo::ResType::kAtlas)
				safeDelVoidPtrSingle<atlas_info_append>(_append);
			elif(_resType == $ResPkgInfo::ResType::kImage)
				safeDelVoidPtrSingle<image_info_append>(_append);
			return *this;
		}

		auto & getAtlasInfo() { return *static_cast<atlas_info_append *>(_append); }
		auto & getAtlasInfo() const { return *static_cast<atlas_info_append const *>(_append); }
		auto & GetImageInfo() { return *static_cast<image_info_append *>(_append); }
		auto & GetImageInfo() const { return *static_cast<image_info_append const *>(_append); }

		auto & cpy(resource_item const & src) {
			getBase().cpy(src);
			_resType = src._resType;
			if (_resType == $ResPkgInfo::ResType::kAtlas) {
				_append = new atlas_info_append();
				getAtlasInfo().cpy(src.getAtlasInfo());
			} elif(_resType == $ResPkgInfo::ResType::kImage) {
				_append = new image_info_append();
				GetImageInfo().cpy(src.GetImageInfo());
			}
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			getBase().read(src);
			if (src.exist("atlas")) {
				_resType = $ResPkgInfo::ResType::kAtlas;
				_append = new atlas_info_append();
				getAtlasInfo().read(src);
			} elif(src.exist("parent")) {
				_resType = $ResPkgInfo::ResType::kImage;
				_append = new image_info_append();
				GetImageInfo().read(src);
			} else
				_resType = $ResPkgInfo::ResType::kNormal;
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			getBase().write(dst);
			if (_resType == $ResPkgInfo::ResType::kAtlas) {
				dst.add("atlas", kTrue);
				getAtlasInfo().write(dst);
			} elif(_resType == $ResPkgInfo::ResType::kImage)
				GetImageInfo().write(dst);
			return dst;
		}
	};
	struct subgroup_info_append {
		Str _parent;
		Str _res;
		Str _loc;
		ListOfInfo<resource_item> _resources;

		auto & cpy(subgroup_info_append const & src) {
			_parent = src._parent;
			_res = src._res;
			_loc = src._loc;
			_resources = src._resources;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src.getByStrIf("parent")._val.getX(_parent, kNullStr);
			src.getByStrIf("res")._val.getX(_res, kNullStr);
			src.getByStrIf("loc")._val.getX(_loc, kNullStr);
			_resources.read(src["resources"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			if (!_parent.isNull())
				dst.add("parent", _parent, _parent.isNull());
			if (!_res.isNull())
				dst.add("res", _res);
			if (!_loc.isNull())
				dst.add("loc", _loc);
			_resources.write(dst.addArr("resources").last()._val.getArr());
			return dst;
		}
	};
	struct group_item {
		Str _id;
		Str _type;
		Void * _append;

		~group_item() {
			goif(_type.cmp("composite"))
				safeDelVoidPtrSingle<group_info_append>(_append);
			elif(_type.cmp("simple"))
				safeDelVoidPtrSingle<subgroup_info_append>(_append);
			return;
		}

		auto & getGroupInfo() { return *static_cast<group_info_append *>(_append); }
		auto & getGroupInfo() const { return *static_cast<group_info_append const *>(_append); }
		auto & getSubGroupInfo() { return *static_cast<subgroup_info_append *>(_append); }
		auto & getSubGroupInfo() const { return *static_cast<subgroup_info_append const *>(_append); }

		auto & cpy(group_item const & src) {
			_id.cpy(src._id);
			_type.cpy(src._type);
			if (_type.cmp("composite")) {
				_append = new group_info_append();
				getGroupInfo().cpy(src.getGroupInfo());
			} elif(_type.cmp("simple")) {
				_append = new subgroup_info_append();
				getSubGroupInfo().cpy(src.getSubGroupInfo());
			}
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["id"].getX(_id);
			src["type"].getX(_type);
			if (_type.cmp("composite")) {
				_append = new group_info_append();
				getGroupInfo().read(src);
			} elif(_type.cmp("simple")) {
				_append = new subgroup_info_append();
				getSubGroupInfo().read(src);
			} else wErr("UnKnown Type");
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("id", _id);
			dst.add("type", _type);
			goif(_type.cmp("composite"))
				getGroupInfo().write(dst);
			elif(_type.cmp("simple"))
				getSubGroupInfo().write(dst);
			return dst;
		}
	};
	struct ResPkgInfo_Ori {
		Str _id;
		SzI _slot_count;
		ListOfInfo<group_item> _groups;

		auto & getThis() {
			return *this;
		}
		auto & tfree() {
			_id.tfree();
			_groups.tfree();
			return *this;
		}
		auto & cpy(ResPkgInfo_Ori const & src) {
			_id = src._id;
			_slot_count = src._slot_count;
			_groups = src._groups;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src.getByStrIf("id")._val.getX(_id, kNullStr);
			src["slot_count"].getX(_slot_count);
			_groups.read(src["groups"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("version", 1);
			if (!_id.isNull())
				dst.add("id", _id);
			dst.add("slot_count", _slot_count);
			_groups.write(dst.addArr("groups").last()._val.getArr());
			return dst;
		}
	};
}
