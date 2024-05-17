#pragma once

#include "./Struct.hpp"

#include "../../../Type.hpp"
#include "../../../Type/Str.hpp"
#include "../../../Type/Path.hpp"
#include "../../../Type/JSON.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "../../../Type/InfoType.hpp"
#include "../../../Type/ImgType.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Info {

	namespace $OriDataRate {

		inline constexpr F64 kAnim(100.0);
		inline constexpr F64 kPos(20.0);
		inline constexpr F64 kScale(20.0 * 65536.0);
		inline constexpr F64 kColor(255.0);
		inline constexpr F64 kMatrix(65536.0);
		inline constexpr F64 kRotate(1000.0);

	}

	struct Image {

		Str $id, $src;
		ImgSz $sz;
		XYPair<F64> $pos;
		XYPair<F64> $scale;

		auto & read($JSON::Obj const & t) {
			t["id"_r].get($id);
			t["src"_r].get($src);
			$sz.read(t["sz"_r]);
			$pos.read(t["pos"_r]);
			$scale.read(t["scale"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(5);
			t("id"_r).set($id);
			t("src"_r).set($src);
			$sz.write(t("sz"_r));
			$pos.write(t("pos"_r));
			$scale.write(t("scale"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::Image const & t) {
			auto i = t.$str.$str.range().find('|');
			$id.cpy(Str().ref(t.$str.$str, i + kNo2, t.$str.$str.$sz - i - kNo2));
			$src.cpy(Str().ref(t.$str.$str, kNo1, i));
			$sz.set(t.$info.$szW, t.$info.$szH);
			$pos.set(F64(t.$info.$posX) / $OriDataRate::kPos, F64(t.$info.$posY) / $OriDataRate::kPos);
			$scale.set(F64(t.$info.$scaleX) / $OriDataRate::kScale, F64(t.$info.$scaleY) / $OriDataRate::kScale);
			return thiz;
		}
		auto & parseTo($Struct::Image & t) const {
			t.$str.$str = $src + '|' + $id;
			t.$info.$szW = $sz.$w;
			t.$info.$szH = $sz.$h;
			t.$info.$posX = $pos.$x * $OriDataRate::kPos;
			t.$info.$posY = $pos.$y * $OriDataRate::kPos;
			t.$info.$scaleX = $scale.$x * $OriDataRate::kScale;
			t.$info.$scaleY = $scale.$y * $OriDataRate::kScale;
			return thiz;
		}

	};

	namespace $LayerChgType {

		struct Pos {

			XYPair<F64> $pos;

			auto & read($JSON::Arr const & t) {
				$pos.read(t);
				return thiz;
			}
			auto & write($JSON::Arr & t) const {
				$pos.write(t);
				return t;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getArr());
			}
			auto & write($JSON::Val & t) const {
				return write(t.setArr().getArr());
			}

			auto & parse($Struct::$LayerChgType::Pos const & t) {
				$pos.$x = static_cast<F64>(t.$x) / $OriDataRate::kPos;
				$pos.$y = static_cast<F64>(t.$y) / $OriDataRate::kPos;
				return thiz;
			}
			auto & parseTo($Struct::$LayerChgType::Pos & t) const {
				t.$x = $pos.$x * $OriDataRate::kPos;
				t.$y = $pos.$y * $OriDataRate::kPos;
				return thiz;
			}

		};

		struct ColorRate {

			F64 $r, $g, $b, $a;

			auto & read($JSON::Arr const & t) {
				t[kNo1].get($r);
				t[kNo2].get($g);
				t[kNo3].get($b);
				t[kNo4].get($a);
				return thiz;
			}
			auto & write($JSON::Arr & t) const {
				t.alloc(4);
				t.add($r).add($g).add($b).add($a);
				return t;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getArr());
			}
			auto & write($JSON::Val & t) const {
				return write(t.setArr().getArr());
			}

			auto & parse($Struct::$LayerChgType::ColorRate const & t) {
				$r = static_cast<F64>(t.$r) / $OriDataRate::kColor;
				$g = static_cast<F64>(t.$g) / $OriDataRate::kColor;
				$b = static_cast<F64>(t.$b) / $OriDataRate::kColor;
				$a = static_cast<F64>(t.$a) / $OriDataRate::kColor;
				return thiz;
			}
			auto & parseTo($Struct::$LayerChgType::ColorRate & t) const {
				t.$r = $r * $OriDataRate::kColor;
				t.$g = $g * $OriDataRate::kColor;
				t.$b = $b * $OriDataRate::kColor;
				t.$a = $a * $OriDataRate::kColor;
				return thiz;
			}

		};

		struct Matrix {

			F64 $a, $b, $c, $d;

			auto & read($JSON::Arr const & t) {
				t[kNo1].get($a);
				t[kNo2].get($b);
				t[kNo3].get($c);
				t[kNo4].get($d);
				return thiz;
			}
			auto & write($JSON::Arr & t) const {
				t.alloc(4);
				t.add($a).add($b).add($c).add($d);
				return t;
			}

			auto & read($JSON::Val const & t) {
				return read(t.getArr());
			}
			auto & write($JSON::Val & t) const {
				return write(t.setArr().getArr());
			}

			auto & parse($Struct::$LayerChgType::Matrix const & t) {
				$a = t.$a / $OriDataRate::kMatrix;
				$b = t.$b / $OriDataRate::kMatrix;
				$c = t.$c / $OriDataRate::kMatrix;
				$d = t.$d / $OriDataRate::kMatrix;
				return thiz;
			}
			auto & parseTo($Struct::$LayerChgType::Matrix & t) const {
				t.$a = $a * $OriDataRate::kMatrix;
				t.$b = $b * $OriDataRate::kMatrix;
				t.$c = $c * $OriDataRate::kMatrix;
				t.$d = $d * $OriDataRate::kMatrix;
				return thiz;
			}

		};

		struct Rotate {

			F64 $angle;

			auto & read($JSON::Val const & t) {
				t.get($angle);
				return thiz;
			}
			auto & write($JSON::Val & t) const {
				t.set($angle);
				return t;
			}

			auto & parse($Struct::$LayerChgType::Rotate const & t) {
				$angle = t.$angle / $OriDataRate::kRotate;
				return thiz;
			}
			auto & parseTo($Struct::$LayerChgType::Rotate & t) const {
				t.$angle = $angle * $OriDataRate::kRotate;
				return thiz;
			}

		};

	}

	struct LayerChg {

		Sz $idx;
		ExistShell<$LayerChgType::Rotate> $rotate;
		ExistShell<$LayerChgType::ColorRate> $color;
		ExistShell<$LayerChgType::Matrix> $matrix;
		ExistShell<$LayerChgType::Pos> $pos;

		auto & read($JSON::Obj const & t) {
			t["idx"_r].get($idx);
			if ($pos.$state = t.exist("pos"_r))
				$pos.$data.read(t["pos"_r]);
			if ($color.$state = t.exist("color"_r))
				$color.$data.read(t["color"_r]);
			if ($rotate.$state = t.exist("rotate"_r))
				$rotate.$data.read(t["rotate"_r]);
			if ($matrix.$state = t.exist("matrix"_r))
				$matrix.$data.read(t["matrix"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(5);
			t("idx"_r).set($idx);
			if ($pos.$state)
				$pos.$data.write(t("pos"_r));
			if ($color.$state)
				$color.$data.write(t("color"_r));
			if ($rotate.$state)
				$rotate.$data.write(t("rotate"_r));
			if ($matrix.$state)
				$matrix.$data.write(t("matrix"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::LayerChg const & t) {
			$idx = t.$idx;
			if ($pos.$state = t.$pos.$state)
				$pos.$data.parse(t.$pos.$data);
			if ($color.$state = t.$color.$state)
				$color.$data.parse(t.$color.$data);
			if ($rotate.$state = t.$rotate.$state)
				$rotate.$data.parse(t.$rotate.$data);
			if ($matrix.$state = t.$matrix.$state)
				$matrix.$data.parse(t.$matrix.$data);
			return thiz;
		}
		auto & parseTo($Struct::LayerChg & t) const {
			t.$idx = $idx;
			if (t.$pos.$state = $pos.$state)
				$pos.$data.parseTo(t.$pos.$data);
			if (t.$color.$state = $color.$state)
				$color.$data.parseTo(t.$color.$data);
			if (t.$rotate.$state = $rotate.$state)
				$rotate.$data.parseTo(t.$rotate.$data);
			if (t.$matrix.$state = $matrix.$state)
				$matrix.$data.parseTo(t.$matrix.$data);
			t.$existU8 = kBoF;
			t.$existU3 = kBoF;
			return thiz;
		}

	};

	struct LayerErase {

		Sz $idx;
		Sz $unk;

		auto & read($JSON::Obj const & t) {
			t["idx"_r].get($idx);
			t["unk"_r].get($unk);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			t("idx"_r).set($idx);
			t("unk"_r).set($unk);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::LayerErase const & t) {
			$idx = t.$idx;
			$unk = t.$unk;
			return thiz;
		}
		auto & parseTo($Struct::LayerErase & t) const {
			t.$idx = $idx;
			t.$unk = $unk;
			return thiz;
		}

		LayerErase() {
			$idx = -1;
			$unk = 0;
			return;
		}

	};

	struct LayerAppend {

		Sz $idx;
		Sz $refIdx;
		Bool $isAnim;
		ExistShell<Str> $label;

		auto & read($JSON::Obj const & t) {
			t["idx"_r].get($idx);
			t["refIdx"_r].get($refIdx);
			t["isAnim"_r].get($isAnim);
			if ($label.$state = t.exist("label"_r))
				t["label"_r].get($label.$data);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(4);
			t("idx"_r).set($idx);
			t("refIdx"_r).set($refIdx);
			t("isAnim"_r).set($isAnim);
			if ($label.$state)
				t("label"_r).set($label.$data);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::LayerAppend const & t) {
			$idx = t.$idx;
			$refIdx = t.$refIdx;
			$isAnim = t.$isAnim;
			if ($label.$state = t.$label.$state)
				$label.$data = t.$label.$data.$str;
			else
				$label.$data.setNull();
			return thiz;
		}
		auto & parseTo($Struct::LayerAppend & t) const {
			t.$idx = $idx;
			t.$refIdx = $refIdx;
			t.$isAnim = $isAnim;
			if (t.$label.$state = $label.$state)
				t.$label.$data.$str = $label.$data;
			else
				t.$label.$data.$str.setNull();
			t.$u7 = kBoF;
			t.$u6 = kBoF;
			t.$u4 = kBoF;
			return thiz;
		}

	};

	struct FSCmd {

		Str $cmd, $arg;

		auto & read($JSON::Arr const & t) {
			t[kNo1].get($cmd);
			t[kNo2].get($arg, kNullStr);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			t.add($cmd).add($arg, $arg.isNull());
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto & parse($Struct::FSCmd const & t) {
			$cmd = t.$cmd.$str;
			if (!t.$arg.$str.isEmpty())
				$arg = t.$arg.$str;
			else
				$arg.setNull();
			return thiz;
		}
		auto & parseTo($Struct::FSCmd & t) const {
			t.$cmd.$str = $cmd;
			if ($arg.isNull())
				t.$arg.$str.alloc().setEmpty();
			else
				t.$arg.$str = $arg;
			return thiz;
		}

	};

	struct Frame {

		ExistShell<InfoList<FSCmd>> $cmd;
		ExistShell<Bool> $stop;
		ExistShell<Str> $label;
		ExistShell<InfoList<LayerChg>> $chg;
		ExistShell<InfoList<LayerAppend>> $append;
		ExistShell<InfoList<LayerErase>> $erase;

		auto & read($JSON::Obj const & t) {
			if ($label.$state = t.exist("label"_r))
				$label.$data = t["label"_r].getStr();
			if ($stop.$state = t.exist("stop"_r))
				$stop.$data = t["stop"_r].getBool();
			if ($cmd.$state = t.exist("cmd"_r))
				$cmd.$data.read(t["cmd"_r]);
			if ($erase.$state = t.exist("erase"_r))
				$erase.$data.read(t["erase"_r]);
			if ($append.$state = t.exist("append"_r))
				$append.$data.read(t["append"_r]);
			if ($chg.$state = t.exist("chg"_r))
				$chg.$data.read(t["chg"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(6);
			if ($label.$state)
				t("label"_r).set($label.$data);
			if ($stop.$state)
				t("stop"_r).set($stop.$data);
			if ($cmd.$state)
				$cmd.$data.write(t("cmd"_r));
			if ($erase.$state)
				$erase.$data.write(t("erase"_r));
			if ($append.$state)
				$append.$data.write(t("append"_r));
			if ($chg.$state)
				$chg.$data.write(t("chg"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::Frame const & t) {
			if ($label.$state = t.$label.$state)
				$label.$data = t.$label.$data.$str;
			if ($stop.$state = t.$stop.$state)
				$stop.$data = t.$stop.$state;
			if ($cmd.$state = t.$cmd.$state)
				$cmd.$data.parse(t.$cmd.$data);
			if ($erase.$state = t.$erase.$state)
				$erase.$data.parse(t.$erase.$data);
			if ($append.$state = t.$append.$state)
				$append.$data.parse(t.$append.$data);
			if ($chg.$state = t.$chg.$state)
				$chg.$data.parse(t.$chg.$data);
			return thiz;
		}
		auto & parseTo($Struct::Frame & t) const {
			if (t.$label.$state = $label.$state)
				t.$label.$data.$str = $label.$data;
			if (t.$stop.$state = $stop.$state)
				t.$stop.$data = $stop.$state;
			if (t.$cmd.$state = $cmd.$state)
				$cmd.$data.parseTo(t.$cmd.$data);
			if (t.$erase.$state = $erase.$state)
				$erase.$data.parseTo(t.$erase.$data);
			if (t.$append.$state = $append.$state)
				$append.$data.parseTo(t.$append.$data);
			if (t.$chg.$state = $chg.$state)
				$chg.$data.parseTo(t.$chg.$data);
			return thiz;
		}

	};

	struct Type1AtAnimBegin { // 不知其内包含的数据为何种类型，暂定为BW2

		InfoList<LayerErase> $v1, $v2;

		auto & read($JSON::Obj const & t) {
			$v1.read(t["v1"_r]);
			$v2.read(t["v2"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$v1.write(t("v1"_r));
			$v2.write(t("v2"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::Type1AtAnimBegin const & t) {
			$v1.parse(t.$v1);
			$v2.parse(t.$v2);
			return thiz;
		}
		auto & parseTo($Struct::Type1AtAnimBegin & t) const {
			$v1.parseTo(t.$v1);
			$v2.parseTo(t.$v2);
			return thiz;
		}

	};

	struct AnimFrameInfo {

		Sz $rate;
		Sz $sum;
		Sz $beg, $end;

		auto & read($JSON::Obj const & t) {
			t["rate"_r].get($rate);
			t["sum"_r].get($sum);
			t["range"_r][kNo1].get($beg);
			t["range"_r][kNo2].get($end);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(3);
			t("rate"_r).set($rate);
			t("sum"_r).set($sum);
			t("range"_r).setArr().getArr().alloc(2).add($beg).add($end);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::AnimFrameInfo const & t) {
			$rate = t.$rate;
			$sum = t.$sum;
			$beg = t.$beg;
			$end = t.$end;
			return thiz;
		}
		auto & parseTo($Struct::AnimFrameInfo & t) const {
			t.$rate = $rate;
			t.$sum = $sum;
			t.$beg = $beg;
			t.$end = $end;
			return thiz;
		}

	};

	struct Anim {

		AnimFrameInfo $info;
		InfoList<Frame> $frame;

		auto & read($JSON::Obj const & t) {
			$info.read(t["info"_r]);
			$frame.read(t["frame"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(2);
			$info.write(t("info"_r));
			$frame.write(t("frame"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::Anim const & t) {
			$info.parse(t.$info);
			$frame.parse(t.$frame);
			return thiz;
		}
		auto & parseTo($Struct::Anim & t) const {
			$info.parseTo(t.$info);
			$frame.parseTo(t.$frame);
			return thiz;
		}

	};

	struct AnimWithLabel : Anim {

		Str $label;

		auto & read($JSON::Obj const & t) {
			t["label"_r].get($label);
			Anim::read(t);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			Anim::write(t);
			t.prependX(kSz1).first().set("label"_r, $label);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::AnimWithLabel const & t) {
			$label = t.$label.$str;
			Anim::parse(t);
			return thiz;
		}
		auto & parseTo($Struct::AnimWithLabel & t) const {
			t.$label.$str = $label;
			Anim::parseTo(t);
			return thiz;
		}

	};

	struct AnimWithUnk : Anim {

		Type1AtAnimBegin $block1;

		auto & read($JSON::Obj const & t) {
			$block1.read(t["block1"_r]);
			Anim::read(t);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			Anim::write(t);
			$block1.write(t.prependX(kSz1).first().set("block1"_r).$v);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::AnimWithUnk const & t) {
			$block1.parse(t.$block1);
			Anim::parse(t);
			return thiz;
		}
		auto & parseTo($Struct::AnimWithUnk & t) const {
			$block1.parseTo(t.$block1);
			Anim::parseTo(t);
			return thiz;
		}

	};

	using $Struct::kUNKDataSz;

	struct Info {

		ConstSzArray<Byte, kUNKDataSz> $unk;
		XYPair<F64> $origin;
		InfoList<Image> $img;
		InfoList<AnimWithLabel> $anim;
		AnimWithUnk $main;

		auto & read($JSON::Obj const & t) {
			ByteSection unk;
			$Str::readHexStr(t["unk"_r].getStr(), unk);
			if (unk.$sz != kUNKDataSz)
				throw VAMGException("PopcapAnimation-Info", "unk.$sz != kUNKDataSz");
			Range(&*$unk.$data, kUNKDataSz).cpy(unk.begin());
			$origin.read(t["origin"_r]);
			$img.read(t["img"_r]);
			$anim.read(t["anim"_r]);
			$main.read(t["main"_r]);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(5);
			$Str::writeHexStr(ByteSection().ref(&*$unk.$data, kUNKDataSz), t("unk"_r).setStr().getStr());
			$origin.write(t("origin"_r));
			$img.write(t("img"_r));
			$anim.write(t("anim"_r));
			$main.write(t("main"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

		auto & parse($Struct::Struct const & t) {
			$unk = t.$unk;
			$origin.$x = t.$originX / $OriDataRate::kAnim;
			$origin.$y = t.$originY / $OriDataRate::kAnim;
			$img.parse(t.$img);
			$anim.parse(t.$anim);
			$main.parse(t.$main);
			return thiz;
		}
		auto & parseTo($Struct::Struct & t) const {
			t.$unk = $unk;
			t.$originX = $origin.$x * $OriDataRate::kAnim;
			t.$originY = $origin.$y * $OriDataRate::kAnim;
			$img.parseTo(t.$img);
			$anim.parseTo(t.$anim);
			$main.parseTo(t.$main);
			return thiz;
		}

	};

}
