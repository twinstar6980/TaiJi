# pragma once

# include "../../MyType/ListOfOther.h"
# include "../../MyType/Path.h"

namespace $TJ::$ResPkgUtil::$DZUtil::$StructInfo {
	struct Res {
		Path _path;

		auto & cpy(Res const & src) {
			_path = src._path;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_path.read(src["Path"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_path.write(dst.addArr("Path").last()._val.getArr());
			return dst;
		}
	};
	struct StructInfo {
		ListOfInfo<Res> _res;

		auto & getThis() { return *this; }

		auto & tfree() {
			_res.tfree();
			return *this;
		}
		auto & cpy(StructInfo const & src) {
			_res = src._res;
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			_res.read(src["Res"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_res.write(dst.addArr("Res").last()._val.getArr());
			return dst;
		}
	};
}
