# pragma once

# include "../MyType/ListOfOther.h"
# include "../MyType/JSON.h"
# include "../MyType/Map.h"
# include "../ImageUtil/ImageType.h"
# include "../ResPkgUtil/RSBUtil/StructInfo.h"

namespace $TJ::$InfoUtil::$ResPkgInfo {
	using Category = $ResPkgUtil::$RSBUtil::Category;
	enum class ResType : SzI { kNormal, kAtlas, kImage };
	struct ResBase {
		Path _path;
		Str _type;

		auto & tfree() {
			_path.tfree();
			_type.tfree();
			return *this;
		}
		auto & cpy(ResBase const & src) {
			_path = src._path;
			_type = src._type;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_path.read(src["Path"].getArr());
			src["Type"].getX(_type);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_path.write(dst.addArr("Path").last()._val.getArr());
			dst.add("Type", _type);
			return dst;
		}
		protected:
		auto & getBase() { return *this; }
		auto & getBase() const { return *this; }
	};
	struct ImageInfo {
		$ImageUtil::ImagePos _pos;
		$ImageUtil::ImageSize _sz;
		$ImageUtil::ImageSgdPos _padding;

		auto & cpy(ImageInfo const & src) {
			_pos = src._pos;
			_sz = src._sz;
			_padding = src._padding;
			return *this;
		}
		auto & read($JSON::Arr const & src) {
			_pos.read(src, kThFirst, kThSecond);
			_sz.read(src, kThThird, kThFourth);
			_padding.read(src, kThFifth, kThSixth);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			_pos.write(dst);
			_sz.write(dst);
			_padding.write(dst);
			return dst;
		}
	};
	struct ImageRes : ResBase {
		ImageInfo _imageInfo;

		auto & cpy(ImageRes const & src) {
			getBase().cpy(src);
			_imageInfo.cpy(src._imageInfo);
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			getBase().read(src);
			_imageInfo.read(src["ImageInfo"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			getBase().write(dst);
			_imageInfo.write(dst.addArr("ImageInfo").last()._val.getArr());
			return dst;
		}

		auto & getResBase() { return getBase(); }
		auto & getResBase() const { return getBase(); }
	};
	struct AtlasInfo {
		$ImageUtil::ImageSize _sz;
		MapOfInfo<ImageRes> _img;

		auto & cpy(AtlasInfo const & src) {
			_sz = src._sz;
			_img = src._img;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_sz.read(src["Size"].getArr());
			_img.read(src["Image"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_sz.write(dst.addArr("Size").last()._val.getArr());
			_img.write(dst.addObj("Image").last()._val.getObj());
			return dst;
		}
	};
	struct Res : ResBase {
		ResType _resType;
		Void * _append;

		auto & getThis() { return *this; }

		auto isType(ResType const & type) const { return _resType == type; }
		auto isNormalType() const { return _resType == ResType::kNormal; }
		auto isAtlasType() const { return _resType == ResType::kAtlas; }

		auto & setType(ResType const & type) {
			_resType = type;
			if (type == ResType::kNormal)
				_append = kNullPtr;
			elif(type == ResType::kAtlas)
				_append = new AtlasInfo();
			return *this;
		}
		auto & setNormalType() { return setType(ResType::kNormal); }
		auto & setAtlasType() { return setType(ResType::kAtlas); }

		auto & tfree() {
			getBase().tfree();
			if (_resType == ResType::kAtlas) safeDelVoidPtrSingle<AtlasInfo>(_append);
			return *this;
		}
		auto & getAtlasInfo() { return *static_cast<AtlasInfo *>(_append); }
		auto & getAtlasInfo() const { return *static_cast<AtlasInfo const *>(_append); }

		auto & cpy(Res const & src) {
			getBase().cpy(src);
			_resType = src._resType;
			if (_resType == ResType::kAtlas) {
				_append = new AtlasInfo();
				getAtlasInfo().cpy(src.getAtlasInfo());
			}
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			getBase().read(src);
			if (!src.exist("AtlasInfo"))
				setNormalType();
			else
				setAtlasType().getAtlasInfo().read(src["AtlasInfo"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			getBase().write(dst);
			if (_resType == ResType::kAtlas)
				getAtlasInfo().write(dst.addObj("AtlasInfo").last()._val.getObj());
			return dst;
		}

		~Res() {
			if (_resType == ResType::kAtlas) safeDelVoidPtrSingle<AtlasInfo>(_append);
			return;
		}
	};
	struct SubGroup {
		Category _category;
		MapOfInfo<Res> _res;

		auto & cpy(SubGroup const & src) {
			_category = src._category;
			_res = src._res;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_category.read(src["Category"].getArr());
			_res.read(src["Res"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_category.write(dst.addArr("Category").last()._val.getArr());
			_res.write(dst.addObj("Res").last()._val.getObj());
			return dst;
		}
	};
	struct Group {
		Bool _isComposite;
		MapOfInfo<SubGroup> _subGroup;

		auto & cpy(Group const & src) {
			_isComposite = src._isComposite;
			_subGroup = src._subGroup;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["IsComposite"].getX(_isComposite);
			_subGroup.read(src["SubGroup"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("IsComposite", _isComposite);
			_subGroup.write(dst.addObj("SubGroup").last()._val.getObj());
			return dst;
		}
	};
	struct ResPkgInfo {
		MapOfInfo<Group> _group;

		auto & getThis() { return *this; }

		auto & tfree() {
			_group.tfree();
			return *this;
		}
		auto & cpy(ResPkgInfo const & src) {
			_group = src._group;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_group.read(src["Group"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_group.write(dst.addObj("Group").last()._val.getObj());
			return dst;
		}
	};
}
