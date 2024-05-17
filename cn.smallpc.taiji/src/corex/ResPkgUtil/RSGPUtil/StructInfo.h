# pragma once

# include "../../Type.h"
# include "../../MyType/ListOfOther.h"
# include "../../ImageUtil/ImageType.h"

# include "../../MyType/JSON.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$StructInfo {
	enum class ResType : UI32 { kNormal, kAtlas };
	struct AtlasInfo {
		SzI _idx;
		$ImageUtil::ImageSize _sz;

		auto & cpy(AtlasInfo const & src) {
			_idx = src._idx;
			_sz = src._sz;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["Idx"].getX(_idx);
			_sz.read(src["Size"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Idx", _idx);
			_sz.write(dst.addArr("Size").last()._val.getArr());
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
				setNormalType();
			else
				setAtlasType().getAtlasInfo().read(src["AtlasInfo"].getObj());
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
	struct StructInfo {
		SzI _headerType;
		Bool _compressNormalRes;
		Bool _compressAtlasRes;
		ResList _res;

		auto & getThis() { return *this; }

		auto & tfree() {
			_res.tfree();
			return *this;
		}
		auto & cpy(StructInfo const & src) {
			_headerType = src._headerType;
			_compressNormalRes = src._compressNormalRes;
			_compressAtlasRes = src._compressAtlasRes;
			_res = src._res;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["HeaderType"].getX(_headerType);
			src["CompressMethod"][kThFirst].getX(_compressNormalRes);
			src["CompressMethod"][kThSecond].getX(_compressAtlasRes);
			_res.read(src["Res"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("HeaderType", _headerType);
			dst.addArr("CompressMethod").last()._val.getArr().add(_compressNormalRes).add(_compressAtlasRes);
			_res.write(dst.addArr("Res").last()._val.getArr());
			return dst;
		}
	};
}
