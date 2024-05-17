#pragma once

#include "../Struct.hpp"
#include "./Common.hpp"

#include "../../../../Type.hpp"
#include "../../../../Type/Str.hpp"
#include "../../../../Type/Path.hpp"
#include "../../../../Type/JSON.hpp"
#include "../../../../Type/BlockFunc.hpp"
#include "../../../../Type/InfoType.hpp"
#include "../../../../Type/ImgType.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter::$CSSInfo {

	struct LayerID {

		List<Sz> $data;

		auto operator==(LayerID const & t) const {
			return $data == t.$data;
		}

		static auto cmp(LayerID const & a, LayerID const & b) {
			for (auto i = kNo1; i < a.$data.lastIdx() && i < b.$data.lastIdx(); ++i) {
				if (a.$data[i] > b.$data[i])
					return kBoT;
				if (a.$data[i] < b.$data[i])
					break;
			}
			return kBoF;
		}

	};

	struct RangeInfo {

		Sz $beg, $end;

		auto & read($JSON::Arr const & t) {
			t[kNo1].get($beg);
			t[kNo2].get($end);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			t.add($beg).add($end);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

	};

	struct Image {

		Str $src;
		F64 $szW, $szH;
		XYPair<F64> $origin;

		auto & read($JSON::Obj const & t) {
			t["src"_r].get($src);
			t["sz"_r][kNo1].get($szW);
			t["sz"_r][kNo2].get($szH);
			$origin.read(t["origin"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(3);
			t("src"_r).set($src);
			t("sz"_r).setArr().getArr().alloc(2).add($szW).add($szH);
			$origin.write(t("origin"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct Frame {

		F64 $opacity;
		ChgMatrix $matrix;

		auto & read($JSON::Obj const & t) {
			t["opacity"_r].get($opacity);
			t["matrix"_r][kNo1].get($matrix.$a);
			t["matrix"_r][kNo2].get($matrix.$b);
			t["matrix"_r][kNo3].get($matrix.$c);
			t["matrix"_r][kNo4].get($matrix.$d);
			t["matrix"_r][kNo5].get($matrix.$x);
			t["matrix"_r][kNo6].get($matrix.$y);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			t("opacity"_r).set($opacity);
			t("matrix"_r).setArr().getArr().alloc(6).add($matrix.$a).add($matrix.$b).add($matrix.$c).add($matrix.$d).add($matrix.$x).add($matrix.$y);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

	struct Layer {

		LayerID $id;
		LayerID $parent;
		RangeInfo $range;
		InfoList<Frame> $frame;

		auto & read($JSON::Obj const & t) {
			$JSON::getListFromArr(t["id"_r], $id.$data);
			$JSON::getListFromArr(t["parent"_r], $parent.$data);
			$range.read(t["range"_r]);
			$frame.read(t["frame"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			$JSON::setListToArr($id.$data, t("id"_r));
			$JSON::setListToArr($parent.$data, t("parent"_r));
			$range.write(t("range"_r));
			$frame.write(t("frame"_r));
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

		XYPair<F64> $origin;
		Sz $frameRate;
		Sz $frameSum;
		List<Str> $sub;
		InfoMap<RangeInfo> $label;
		InfoList<Image> $img;
		InfoList<Layer> $layer;

		auto & read($JSON::Obj const & t) {
			$origin.read(t["origin"_r]);
			t["frameRate"_r].get($frameRate);
			t["frameSum"_r].get($frameSum);
			$JSON::getListFromArr(t["sub"_r], $sub);
			$label.read(t["label"_r]);
			$img.read(t["img"_r]);
			$layer.read(t["layer"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(7);
			$origin.write(t("origin"_r));
			t("frameRate"_r).set($frameRate);
			t("frameSum"_r).set($frameSum);
			$JSON::setListToArr($sub, t("sub"_r));
			$label.write(t("label"_r));
			$img.write(t("img"_r));
			$layer.write(t("layer"_r));
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
