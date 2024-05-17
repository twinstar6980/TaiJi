# pragma once

# include "../../Type.h"
# include "../../MyType/ListOfOther.h"
# include "../../MyType/Map.h"
# include "../../ImageUtil/ImageType.h"
# include "../../UtilKit/JSONUtil.h"

# include "./Struct.h"
# include "../RSGPUtil/StructInfo.h"

namespace $TJ::$ResPkgUtil::$RSBUtil {
	constexpr SzI kCategoryResNull(0);
	Str const kCategoryLocNull;

	struct Category {
		SzI _res;
		Str _loc;

		auto & read($JSON::Arr const & src) {
			src[kThFirst].getX(_res, kCategoryResNull);
			src[kThSecond].getX(_loc, kCategoryLocNull);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.add(_res, _res == kCategoryResNull).add(_loc, _loc.isNull());
			return dst;
		}

		auto isNullRes() const { return _res == kCategoryResNull; }

		auto isValidRes() const { return _res != kCategoryResNull && _res != kSzNull; }

		auto & setNull() {
			_res = kCategoryResNull;
			_loc.setNull();
			return *this;
		}

		auto & operator=(Category const & t) {
			_res = t._res;
			_loc = t._loc;
			return *this;
		}
	};
	namespace $ResInfoList {
		struct Res_Append {
			Str _key;
			Str _val;
			UI32 _unKnown;

			auto & cpy(Res_Append const & src) {
				_key = src._key;
				_val = src._val;
				_unKnown = src._unKnown;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src[kThFirst]._key.getX(_key);
				src[kThFirst]._val.getX(_val);
				src["UnKnown"].getX(_unKnown);
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add(_key, _val);
				dst.add("UnKnown", _unKnown);
				return dst;
			}
		};
		struct Res_Expand {
			SzI _unKnown_A;
			SzI _unKnown_C;
			SzI _unKnown_D;
			Str _parent;
			SzI _flag;
			$ImageUtil::ImagePos _pos;
			$ImageUtil::ImageSize _sz;
			$ImageUtil::ImageSgdPos _padding;

			auto & cpy(Res_Expand const & src) {
				_unKnown_A = src._unKnown_A;
				_unKnown_C = src._unKnown_C;
				_unKnown_D = src._unKnown_D;
				_parent = src._parent;
				_flag = src._flag;
				_pos = src._pos;
				_sz = src._sz;
				_padding = src._padding;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["UnKnown_A"].getX(_unKnown_A);
				src["UnKnown_C"].getX(_unKnown_C);
				src["UnKnown_D"].getX(_unKnown_D);
				src["Parent"].getX(_parent, kNullStr);
				goif(_unKnown_A == 2)
					_sz.read(src["AtlasInfo"].getArr());
				elif(_unKnown_A == 4) {
					src["Flag"].getX(_flag);
					_pos.read(src["ImageInfo"].getArr(), kThFirst, kThSecond);
					_sz.read(src["ImageInfo"].getArr(), kThThird, kThFourth);
					_padding.read(src["ImageInfo"].getArr(), kThFifth, kThSixth);
				}
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("UnKnown_A", _unKnown_A);
				dst.add("UnKnown_C", _unKnown_C);
				dst.add("UnKnown_D", _unKnown_D);
				dst.add("Parent", _parent, _parent.isNull());
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
			Str _id;
			Path _path;
			Str _type;
			Bool _existExpand;
			Res_Expand _expand;
			ListOfInfo<Res_Append> _append;

			auto & cpy(Res const & src) {
				_id = src._id;
				_path = src._path;
				_type = src._type;
				_existExpand = src._existExpand;
				_expand.cpy(src._expand);
				_append = src._append;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX(_id);
				_path.read(src["Path"].getArr());
				src["Type"].getX(_type);
				if (_existExpand)
					_expand.read(src["Expand"].getObj());
				_append.read(src["Append"].getArr());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", _id);
				_path.write(dst.addArr("Path").last()._val.getArr());
				dst.add("Type", _type);
				if (_existExpand)
					_expand.write(dst.addObj("Expand").last()._val.getObj());
				_append.write(dst.addArr("Append").last()._val.getArr());
				return dst;
			}
		};
		struct SubGroup {
			Str _id;
			Category _category;
			ListOfInfo<Res> _res;

			auto & cpy(SubGroup const & src) {
				_id = src._id;
				_category = src._category;
				_res = src._res;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX(_id);
				_category.read(src["Category"].getArr());
				_res.read(src["Res"].getArr());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", _id);
				_category.write(dst.addArr("Category").last()._val.getArr());
				_res.write(dst.addArr("Res").last()._val.getArr());
				return dst;
			}
		};
		struct Group {
			Str _id;
			UI32 _unKnown;
			ListOfInfo<SubGroup> _subGroup;

			auto & cpy(Group const & src) {
				_id = src._id;
				_unKnown = src._unKnown;
				_subGroup = src._subGroup;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX(_id);
				src["UnKnown"].getX(_unKnown);
				_subGroup.read(src["SubGroup"].getArr());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", _id);
				dst.add("UnKnown", _unKnown);
				_subGroup.write(dst.addArr("SubGroup").last()._val.getArr());
				return dst;
			}
		};
		struct ResInfoList {
			ListOfInfo<Group> _group;

			auto & cpy(ResInfoList const & src) {
				_group = src._group;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				_group.read(src["Group"].getArr());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				_group.write(dst.addArr("Group").last()._val.getArr());
				return dst;
			}
		};
	}
	namespace $StructInfo {
		enum class ResType : UI32 { kNormal, kAtlas };
		struct AtlasInfo {
			SzI _idx;
			$ImageUtil::ImageSize _sz;
			SzI _texFmt;

			auto & cpy(AtlasInfo const & src) {
				_idx = src._idx;
				_sz = src._sz;
				_texFmt = src._texFmt;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["Idx"].getX(_idx);
				_sz.read(src["Size"].getArr());
				src["TexFmt"].getX(_texFmt);
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("Idx", _idx);
				_sz.write(dst.addArr("Size").last()._val.getArr());
				dst.add("TexFmt", _texFmt);
				return dst;
			}
		};
		struct Res {
			Path _path;
			ResType _type;
			Void * _append;

			auto isType(ResType const & type) const { return _type == type; }
			auto isNormalType() const { return _type == ResType::kNormal; }
			auto isAtlasType() const { return _type == ResType::kAtlas; }

			auto & setType(ResType const & type) {
				_type = type;
				if (type == ResType::kNormal)
					_append = kNullPtr;
				elif(type == ResType::kAtlas)
					_append = new AtlasInfo();
				return *this;
			}
			auto & setNormalType() { return setType(ResType::kNormal); }
			auto & setAtlasType() { return setType(ResType::kAtlas); }

			auto & FreeType() {
				if (_type == ResType::kNormal)
					_append = kNullPtr;
				elif(_type == ResType::kAtlas)
					safeDelVoidPtrSingle<AtlasInfo>(_append);
				_type = ResType::kNormal;
				return *this;
			}
			auto & tfree() {
				_path.tfree();
				FreeType();
				return *this;
			}
			auto & getAtlasInfo() { return *static_cast<AtlasInfo *>(_append); }
			auto & getAtlasInfo() const { return *static_cast<AtlasInfo const *>(_append); }

			auto & cpy(Res const & src) {
				_path = src._path;
				_type = src._type;
				if (_type == ResType::kAtlas) {
					_append = new AtlasInfo();
					getAtlasInfo().cpy(src.getAtlasInfo());
				}
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				_path.read(src["Path"].getArr());
				if (!src.exist("AtlasInfo"))
					setType(ResType::kNormal);
				else
					setType(ResType::kAtlas).getAtlasInfo().read(src["AtlasInfo"].getObj());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				_path.write(dst.addArr("Path").last()._val.getArr());
				if (_type == ResType::kAtlas)
					getAtlasInfo().write(dst.addObj("AtlasInfo").last()._val.getObj());
				return dst;
			}

			~Res() {
				if (_type == ResType::kAtlas) safeDelVoidPtrSingle<AtlasInfo>(_append);
				return;
			}
		};
		struct ResList : ListOfInfo<Res> {
			auto GetResSum(ResType const & type) const {
				auto sum = kSzZero;
				for_criter(res, *this)
					if (res._type == type)
						++sum;
				return sum;
			}
			auto GetNormalResSum() const { return GetResSum(ResType::kNormal); }
			auto GetAtlasResSum() const { return GetResSum(ResType::kAtlas); }

		};
		struct SubGroup {
			Category _category;
			SzI _headerType;
			Bool _compressNormalRes;
			Bool _compressAtlasRes;
			ResList _res;

			auto & cpy(SubGroup const & src) {
				_category = src._category;
				_headerType = src._headerType;
				_compressNormalRes = src._compressNormalRes;
				_compressAtlasRes = src._compressAtlasRes;
				_res = src._res;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				_category.read(src["Category"].getArr());
				src["HeaderType"].getX(_headerType);
				src["CompressMethod"][kThFirst].getX(_compressNormalRes);
				src["CompressMethod"][kThSecond].getX(_compressAtlasRes);
				_res.read(src["Res"].getArr());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				_category.write(dst.addArr("Category").last()._val.getArr());
				dst.add("HeaderType", _headerType);
				dst.addArr("CompressMethod").last()._val.getArr().add(_compressNormalRes).add(_compressAtlasRes);
				_res.write(dst.addArr("Res").last()._val.getArr());
				return dst;
			}
			auto & ToRSGPStructInfo($RSGPUtil::$StructInfo::StructInfo & rsgpStructInfo) const {
				rsgpStructInfo._headerType = _headerType;
				rsgpStructInfo._compressNormalRes = _compressNormalRes;
				rsgpStructInfo._compressAtlasRes = _compressAtlasRes;
				rsgpStructInfo._res.alloc(_res._sz);
				for_criter(res, _res) {
					auto & dstRes = rsgpStructInfo._res.add().last();
					dstRes._path.cpy(res._path);
					if (res._type == ResType::kNormal) {
						dstRes.setType($RSGPUtil::$StructInfo::ResType::kNormal);
					} elif(res._type == ResType::kAtlas) {
						dstRes.setType($RSGPUtil::$StructInfo::ResType::kAtlas);
						dstRes.getAtlasInfo()._idx = res.getAtlasInfo()._idx;
						dstRes.getAtlasInfo()._sz._w = res.getAtlasInfo()._sz._w;
						dstRes.getAtlasInfo()._sz._h = res.getAtlasInfo()._sz._h;
					}
				}
				return rsgpStructInfo;
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
		struct StructInfo {
			SzI _headerType;
			Bool _setResInfoListInHeader;
			Bool _useMoreAtlasInfo;
			MapOfInfo<Group> _group;

			auto & getThis() { return *this; }

			auto & tfree() {
				_group.tfree();
				return *this;
			}
			auto & cpy(StructInfo const & src) {
				_headerType = src._headerType;
				_setResInfoListInHeader = src._setResInfoListInHeader;
				_useMoreAtlasInfo = src._useMoreAtlasInfo;
				_group = src._group;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["HeaderType"].getX(_headerType);
				src["SetResListInHeader"].getX(_setResInfoListInHeader);
				src["UseMoreAtlasInfo"].getX(_useMoreAtlasInfo);
				_group.read(src["Group"].getObj());
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("HeaderType", _headerType);
				dst.add("SetResListInHeader", _setResInfoListInHeader);
				dst.add("UseMoreAtlasInfo", _useMoreAtlasInfo);
				_group.write(dst.addObj("Group").last()._val.getObj());
				return dst;
			}
			auto & CntSum(SzI & groupSum, SzI & subGroupSum, SzI & resSum, SzI & atlasResSum) const {
				groupSum = _group._sz;
				subGroupSum = kSzZero;
				resSum = kSzZero;
				atlasResSum = kSzZero;
				for_criter(group, _group) {
					subGroupSum += group._val._subGroup._sz;
					for_criter(subGroup, group._val._subGroup) {
						resSum += subGroup._val._res._sz;
						atlasResSum += subGroup._val._res.GetAtlasResSum();
					}
				}
				return *this;
			}
		};
	}
	namespace $PartInfo {
		struct PartInfo {
			Str _id;
			StrList _group;

			auto & cpy(PartInfo const & src) {
				_id = src._id;
				_group = src._group;
				return *this;
			}
			auto & read($JSON::Obj const & src) {
				src["ID"].getX(_id);
				$JSON::getArrStr(src["Group"].getArr(), _group);
				return *this;
			}
			auto & write($JSON::Obj & dst) const {
				dst.add("ID", _id);
				$JSON::setArrStr(_group, dst.addArr("Group").last()._val.getArr());
				return dst;
			}
		};
		using PartInfoList = ListOfInfo<PartInfo>;
	}
}
