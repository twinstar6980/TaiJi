# pragma once

# include "../../MyType/ListOfOther.h"
# include "../../MyType/Path.h"

namespace $TJ::$ResPkgUtil::$PAKUtil::$StructInfo {
	struct Res {
		Path _path;
		UI64 _time;

		auto & cpy(Res const & src) {
			_path = src._path;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_path.read(src["Path"].getArr());
			src["Time"].getX(_time);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_path.write(dst.addArr("Path").last()._val.getArr());
			dst.add("Time", _time);
			return dst;
		}
	};
	struct StructInfo {
		UI32 _magic;
		UI32 _ver;
		ListOfInfo<Res> _res;

		auto & getThis() { return *this; }

		auto & tfree() {
			_res.tfree();
			return *this;
		}
		auto & cpy(StructInfo const & src) {
			_magic = src._magic;
			_ver = src._ver;
			_res = src._res;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["Magic"].getX(_magic);
			src["Ver"].getX(_ver);
			_res.read(src["Res"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Magic", _magic);
			dst.add("Ver", _ver);
			_res.write(dst.addArr("Res").last()._val.getArr());
			return dst;
		}
	};
}
