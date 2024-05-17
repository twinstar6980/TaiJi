#pragma once

// ------------------------------------------------

#include "core/type/wrapper.hpp"
#include "core/file_system/path.hpp"

#include "core/tool/animation/popcap_animation/information.hpp"

// ------------------------------------------------

namespace TwinKleS::Tool::Animation::PopcapAnimation::Convert {

	// ------------------------------------------------

	struct ChangeMatrix {

		// ------------------------------------------------

		FloatingNumber64 a;
		FloatingNumber64 b;
		FloatingNumber64 c;
		FloatingNumber64 d;
		FloatingNumber64 x;
		FloatingNumber64 y;

		// ------------------------------------------------

		auto init (
		) -> None {
			thiz.a = 1.0_floating_number_64;
			thiz.d = 1.0_floating_number_64;
			thiz.b = 0.0_floating_number_64;
			thiz.c = 0.0_floating_number_64;
			thiz.x = 0.0_floating_number_64;
			thiz.y = 0.0_floating_number_64;
			return;
		}

		auto mix (
			const Information::LayerChange & t
		) -> None {
			if (t.position) {
				thiz.x = t.position.get().x;
				thiz.y = t.position.get().y;
			}
			if (t.rotate) {
				thiz.a = make_wrapper<FloatingNumber64>(::cos(t.rotate.get().angle.get()));
				thiz.b = make_wrapper<FloatingNumber64>(::sin(t.rotate.get().angle.get()));
				thiz.c = make_wrapper<FloatingNumber64>(-::sin(t.rotate.get().angle.get()));
				thiz.d = make_wrapper<FloatingNumber64>(::cos(t.rotate.get().angle.get()));
			}
			if (t.transform) {
				thiz.a = t.transform.get().a;
				thiz.b = t.transform.get().b;
				thiz.c = t.transform.get().c;
				thiz.d = t.transform.get().d;
			}
			return;
		}

		// ------------------------------------------------

		auto operator *= (
			const ChangeMatrix & t
		) -> ChangeMatrix& {
			// TODO : replace to single {...} expression
			auto temp = ChangeMatrix{};
			temp.a    = a * t.a + c * t.b;
			temp.b    = b * t.a + d * t.b;
			temp.c    = a * t.c + c * t.d;
			temp.d    = b * t.c + d * t.d;
			temp.x    = a * t.x + c * t.y + x;
			temp.y    = b * t.x + d * t.y + y;
			thiz      = temp;
			return thiz;
		}

		// ------------------------------------------------

		friend M_json_from(ChangeMatrix) {
			M_from_json_as_array_to_a_n_f(6);
			M_from_json_for_member_of_a(1, a);
			M_from_json_for_member_of_a(2, b);
			M_from_json_for_member_of_a(3, c);
			M_from_json_for_member_of_a(4, d);
			M_from_json_for_member_of_a(5, x);
			M_from_json_for_member_of_a(6, y);
		}

		friend M_json_to(ChangeMatrix) {
			M_to_json_as_array_to_a;
			M_to_json_for_member_of_a(1, a);
			M_to_json_for_member_of_a(2, b);
			M_to_json_for_member_of_a(3, c);
			M_to_json_for_member_of_a(4, d);
			M_to_json_for_member_of_a(5, x);
			M_to_json_for_member_of_a(6, y);
		}

		// ------------------------------------------------

	};

	// ------------------------------------------------

	inline auto mix_change_martix (
		ChangeMatrix &       matrix,
		const ChangeMatrix & change
	) -> None {
		auto temp = change;
		temp *= matrix;
		matrix = temp;
		return;
	}

	// ------------------------------------------------

	struct ChangeColor {

		// ------------------------------------------------

		FloatingNumber64 red;
		FloatingNumber64 green;
		FloatingNumber64 blue;
		FloatingNumber64 alpha;

		// ------------------------------------------------

		auto init (
		) -> None {
			thiz.red   = 1.0_floating_number_64;
			thiz.green = 1.0_floating_number_64;
			thiz.blue  = 1.0_floating_number_64;
			thiz.alpha = 1.0_floating_number_64;
			return;
		}

		auto load (
			const Information::LayerChange & t
		) -> None {
			if (t.discolor) {
				thiz.red   = t.discolor.get().red;
				thiz.green = t.discolor.get().green;
				thiz.blue  = t.discolor.get().blue;
				thiz.alpha = t.discolor.get().alpha;
			}
			return;
		}

		// ------------------------------------------------

		friend M_json_from(ChangeColor) {
			M_from_json_as_array_to_a_n_f(4);
			M_from_json_for_member_of_a(1, red);
			M_from_json_for_member_of_a(2, green);
			M_from_json_for_member_of_a(3, blue);
			M_from_json_for_member_of_a(4, alpha);
		}

		friend M_json_to(ChangeColor) {
			M_to_json_as_array_to_a;
			M_to_json_for_member_of_a(1, red);
			M_to_json_for_member_of_a(2, green);
			M_to_json_for_member_of_a(3, blue);
			M_to_json_for_member_of_a(4, alpha);
		}

		// ------------------------------------------------

	};

	// ------------------------------------------------

}

// ------------------------------------------------
