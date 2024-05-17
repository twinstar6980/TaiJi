#pragma once

#include "../../../../Type.hpp"
#include "../../../../Type/Path.hpp"

#include "../Struct.hpp"
#include "../Info.hpp"
#include "./CSSInfo.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter {

	struct ChgMatrix {

		F64 $a, $b, $c, $d, $x, $y;

		auto & init() {
			$a = $d = 1.0;
			$b = $c = 0.0;
			$x = $y = 0.0;
			return thiz;
		}
		auto & mix($Info::LayerChg const & t) {
			if (t.$pos.$state) {
				$x = t.$pos.$data.$pos.$x;
				$y = t.$pos.$data.$pos.$y;
			}
			if (t.$rotate.$state) {
				$a = cos(t.$rotate.$data.$angle);
				$b = sin(t.$rotate.$data.$angle);
				$c = -sin(t.$rotate.$data.$angle);
				$d = cos(t.$rotate.$data.$angle);
			}
			if (t.$matrix.$state) {
				$a = t.$matrix.$data.$a;
				$b = t.$matrix.$data.$b;
				$c = t.$matrix.$data.$c;
				$d = t.$matrix.$data.$d;
			}
			return thiz;
		}

		auto & operator*=(ChgMatrix const & t) {
			ChgMatrix tmp;
			tmp.$a = $a * t.$a + $c * t.$b;
			tmp.$b = $b * t.$a + $d * t.$b;
			tmp.$c = $a * t.$c + $c * t.$d;
			tmp.$d = $b * t.$c + $d * t.$d;
			tmp.$x = $a * t.$x + $c * t.$y + $x;
			tmp.$y = $b * t.$x + $d * t.$y + $y;
			thiz = tmp;
			return thiz;
		}

	};

	inline auto & mixChgMatrix(ChgMatrix & srcChg, ChgMatrix const & newChg) {
		auto tmp = newChg;
		tmp *= srcChg;
		srcChg = tmp;
		return srcChg;
	};

	struct ChgColor {

		F64 $r, $g, $b, $a;

		auto & init() {
			$r = $g = $b = $a = 1.0;
			return thiz;
		}

		auto & load($Info::LayerChg const & t) {
			if (t.$color.$state) {
				$r = t.$color.$data.$r;
				$g = t.$color.$data.$g;
				$b = t.$color.$data.$b;
				$a = t.$color.$data.$a;
			}
			return thiz;
		}

	};

}
