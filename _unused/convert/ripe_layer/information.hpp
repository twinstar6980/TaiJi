#pragma once

// ------------------------------------------------

#include "core/tool/animation/popcap_animation/convert/common.hpp"

#include "core/type/wrapper.hpp"
#include "core/string/string.hpp"
#include "core/file_system/path.hpp"
#include "core/json/json.hpp"

// ------------------------------------------------

namespace TwinKleS::Tool::Animation::PopcapAnimation::Convert::RipeLayer::Information {

	// ------------------------------------------------

	struct LayerID {

		// ------------------------------------------------

		List<Size> data;

		// ------------------------------------------------

		auto operator == (
			const LayerID & t
		) const -> Boolean {
			return thiz.data == t.data;
		}

		// ------------------------------------------------

		static auto compare (
			const LayerID & lt,
			const LayerID & rt
		) -> Boolean {
			for (auto & i : SizeRange{min(lt.data.size(), rt.data.size())}) {
				if (lt.data[i] > rt.data[i]) {
					return k_true;
				}
				if (lt.data[i] < rt.data[i]) {
					break;
				}
			}
			return k_false;
		}

		// ------------------------------------------------

		friend M_json_from(LayerID) {
			json.from(t.data);
		}

		friend M_json_to(LayerID) {
			json.to(t.data);
		}

		// ------------------------------------------------

	};

	struct RangeInformation {

		// ------------------------------------------------

		Size begin;
		Size end;

		// ------------------------------------------------

		friend M_json_from(RangeInformation) {
			M_from_json_as_array_to_a_n_f(2);
			M_from_json_for_member_of_a(1, begin);
			M_from_json_for_member_of_a(2, end);
		}

		friend M_json_to(RangeInformation) {
			M_to_json_as_array_to_a;
			M_to_json_for_member_of_a(1, begin);
			M_to_json_for_member_of_a(2, end);
		}

		// ------------------------------------------------

	};

	struct Sprite {

		// ------------------------------------------------

		String                   source;
		WHPair<FloatingNumber64> size;
		XYPair<FloatingNumber64> origin;

		// ------------------------------------------------

		friend M_json_from(Sprite) {
			M_from_json_as_object_to_o_n(3);
			M_from_json_for_member_of_o(source);
			M_from_json_for_member_of_o(size);
			M_from_json_for_member_of_o(origin);
		}

		friend M_json_to(Sprite) {
			M_to_json_as_object_to_o;
			M_to_json_for_member_of_o(source);
			M_to_json_for_member_of_o(size);
			M_to_json_for_member_of_o(origin);
		}

		// ------------------------------------------------

	};

	struct Frame {

		// ------------------------------------------------
		
		ChangeMatrix transform;
		ChangeColor  discolor;

		// ------------------------------------------------

		friend M_json_from(Frame) {
			M_from_json_as_object_to_o_n(2);
			M_from_json_for_member_of_o(transform);
			M_from_json_for_member_of_o(discolor);
		}

		friend M_json_to(Frame) {
			M_to_json_as_object_to_o;
			M_to_json_for_member_of_o(transform);
			M_to_json_for_member_of_o(discolor);
		}

		// ------------------------------------------------

	};

	struct Layer {

		// ------------------------------------------------

		LayerID          id;
		LayerID          parent;
		RangeInformation range;
		List<Frame>      frame;

		// ------------------------------------------------

		friend M_json_from(Layer) {
			M_from_json_as_object_to_o_n(4);
			M_from_json_for_member_of_o(id);
			M_from_json_for_member_of_o(parent);
			M_from_json_for_member_of_o(range);
			M_from_json_for_member_of_o(frame);
		}

		friend M_json_to(Layer) {
			M_to_json_as_object_to_o;
			M_to_json_for_member_of_o(id);
			M_to_json_for_member_of_o(parent);
			M_to_json_for_member_of_o(range);
			M_to_json_for_member_of_o(frame);
		}

		// ------------------------------------------------

	};

	struct Package {

		// ------------------------------------------------

		XYPair<FloatingNumber64>      origin;
		Size                          frame_rate;
		Size                          frame_count;
		List<String>                  sub;
		Map<String, RangeInformation> label;
		List<Sprite>                  sprite;
		List<Layer>                   layer;

		// ------------------------------------------------

		friend M_json_from(Package) {
			M_from_json_as_object_to_o_n(7);
			M_from_json_for_member_of_o(origin);
			M_from_json_for_member_of_o(frame_rate);
			M_from_json_for_member_of_o(frame_count);
			M_from_json_for_member_of_o(sub);
			M_from_json_for_member_of_o(label);
			M_from_json_for_member_of_o(sprite);
			M_from_json_for_member_of_o(layer);
		}

		friend M_json_to(Package) {
			M_to_json_as_object_to_o;
			M_to_json_for_member_of_o(origin);
			M_to_json_for_member_of_o(frame_rate);
			M_to_json_for_member_of_o(frame_count);
			M_to_json_for_member_of_o(sub);
			M_to_json_for_member_of_o(label);
			M_to_json_for_member_of_o(sprite);
			M_to_json_for_member_of_o(layer);
		}

		// ------------------------------------------------

	};

	// ------------------------------------------------

}

// ------------------------------------------------
