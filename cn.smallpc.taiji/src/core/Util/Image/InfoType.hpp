#pragma once

#include "./Type.hpp"
#include "../../Type.hpp"
#include "../../Type/InfoType.hpp"
#include "../../Type/Map.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/JSON.hpp"

namespace $TJ::$Util::$Image {

	struct ImageInfo {

		Path $path;
		ImgArea $area;

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			$area.read(t["area"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			cnvPathToJSONArr($path, t("path"_r));
			$area.write(t("area"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct AtlasInfo {

		Path $path;
		ImgSz $sz;
		TexFmt $fmt;
		InfoList<ImageInfo> $img;

		auto & read($JSON::Obj const & t) {
			cnvJSONArrToPath(t["path"_r], $path);
			$sz.read(t["sz"_r]);
			$fmt = t["fmt"_r].isNull() ? TexFmt::kNull : getTexFmt(t["fmt"_r].getStr());
			$img.read(t["img"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			cnvPathToJSONArr($path, t("path"_r));
			$sz.write(t("sz"_r));
			if ($fmt == TexFmt::kNull)
				t("fmt"_r).setNull();
			else
				t("fmt"_r).set(getTexFmt($fmt));
			$img.write(t("img"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct TexFmtIdxMap : Map<Str, Sz> {

		auto & read($JSON::Arr const & t) {
			alloc(t.$sz);
			for (auto const & e : t) {
				appendX();
				e["fmt"_r].get(last().$k);
				e["idx"_r].get(last().$v);
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}

		auto & findTexFmt(Sz const & texIdx) const {
			Sz idx;
			return findVal(texIdx, idx) ? get(idx).$k : kTexFmtStr[kNo1];
		}
		auto & findTexFmt(Sz const & texIdx, Str & dst) const {
			Sz idx;
			return findVal(texIdx, idx) ? dst.cpy(get(idx).$k) : dst.setNull();
		}
		auto findTexFmt(Str const & texStr) const {
			Sz idx;
			return findKey(texStr, idx) ? get(idx).$v : Sz(-1);
		}

	};

}
