#pragma once

#include "../../../Type/InfoType.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$ResPkg::$PAK::$Info {

	struct Res {

		Path $path;
		IU64 $time;

		auto & init() {
			$path.setNull();
			$time = 0;
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			t["time"_r].get($time);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			t("time"_r).set($time);
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

		IU32 $magic;
		IU32 $ver;
		InfoList<Res> $res;

		auto & init() {
			$magic = 0;
			$ver = 0;
			$res.setNull();
			return thiz;
		}
		auto & free() {
			return init();
		}

		auto & read($JSON::Obj const & t) {
			t["magic"_r].get($magic);
			t["ver"_r].get($ver);
			$res.read(t["res"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(3);
			t("magic"_r).set($magic);
			t("ver"_r).set($ver);
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
