# pragma once

# include "../Type.h"
# include "../MyType/StrList.h"
# include "./ImageType.h"
# include "../MyType/ListOfOther.h"
# include "../MyType/Map.h"
# include "../MyType/Path.h"
# include "../MyType/PairVal.h"
# include "../MyType/JSON.h"

namespace $TJ::$ImageUtil {
	struct ImageInfo {
		Path _path;
		ImagePos _pos;
		ImageSize _sz;

		auto & read($JSON::Obj const & src) {
			_path.read(src["Path"].getArr());
			_pos.read(src["Info"].getArr(), kThFirst, kThSecond);
			_sz.read(src["Info"].getArr(), kThThird, kThFourth);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_path.write(dst.addArr("Path").last()._val.getArr());
			auto & Info = dst.addArr("Info").last()._val.getArr();
			_pos.write(Info);
			_sz.write(Info);
			return dst;
		}
	};
	struct AtlasInfo {
		Path _path;
		TexFmt _texFmt;
		ImageSize _sz;
		ListOfInfo<ImageInfo> _img;

		auto & read($JSON::Obj const & src) {
			_path.read(src["Path"].getArr());
			_texFmt = GetTexFmt(src["TexFmt"].getStr());
			_sz.read(src["Size"].getArr());
			_img.read(src["Image"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_path.write(dst.addArr("Path").last()._val.getArr());
			dst.add("TexFmt", GetTexFmt(_texFmt));
			_sz.write(dst.addArr("Size").last()._val.getArr());
			_img.write(dst.addArr("Image").last()._val.getArr());
			return dst;
		}
	};
	struct TexFmtIdxList : Map<Str, SzI> {

		auto & tfree() { getBase().tfree(); return *this; }

		auto & getThis() { return *this; }

		auto & read($JSON::Arr const & src) {
			alloc(src._sz);
			for_criter(e, src) {
				add();
				e["TexFmt"].getX(last()._key);
				e["Idx"].getX(last()._val);
			}
			return *this;
		}

		auto & FindTexFmt(SzI const & texIdx) const {
			SzI idx;
			return findVal(texIdx, idx) ? get(idx)._key : kNullStr;
		}
		auto & FindTexFmt(SzI const & texIdx, Str & dst) const {
			SzI idx;
			return findVal(texIdx, idx) ? dst.cpy(get(idx)._key) : dst.setNull();
		}
		auto FindTexFmt(Str const & texStr) const {
			SzI idx;
			return findKey(texStr, idx) ? get(idx)._val : kThNull;
		}
	};
}
