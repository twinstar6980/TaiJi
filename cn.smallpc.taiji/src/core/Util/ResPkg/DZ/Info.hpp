#pragma once

#include "../../../Type/InfoType.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$ResPkg::$DZ::$Info {

	struct Res {

		Path $path;
		Sz $flag;

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			t["flag"_r].get($flag);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			t("flag"_r).set($flag);
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

		InfoList<Res> $res;

		auto & init() {
			$res.setNull();
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			$res.read(t["res"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(1);
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

}
