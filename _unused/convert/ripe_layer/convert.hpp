#pragma once

// ------------------------------------------------

#include "core/tool/animation/popcap_animation/convert/common.hpp"
#include "core/tool/animation/popcap_animation/convert/ripe_layer/information.hpp"

#include "core/type/wrapper.hpp"
#include "core/string/string.hpp"
#include "core/file_system/path.hpp"
#include "core/json/json.hpp"

// ------------------------------------------------

namespace TwinKleS::Tool::Animation::PopcapAnimation::Convert::RipeLayer {

	// ------------------------------------------------

	namespace From {

		// ------------------------------------------------

		struct LayerSlot {
			Information::LayerID id;
			Information::LayerID parent;

			struct {
				Size         index;
				ChangeMatrix matrix;
				ChangeColor  color;
			}                sprite;
		};

		struct ReferenceSlot {
			Size         index;
			Boolean      animation;
			Size         reference;
			Size         begin_frame_index;
			ChangeMatrix change_matrix;
			ChangeColor  change_color;
		};

		// ------------------------------------------------

		inline auto make_layer_list_list (
			List<List<LayerSlot>> &                            dest,
			const PopcapAnimation::Information::Animation &    source_animation,
			const List<PopcapAnimation::Information::Sprite> & sprite_list,
			const List<List<List<LayerSlot>>> &                animation_list
		) -> None {
			auto layer_count = Range::accumulate(source_animation.frame, k_none_size,
				[] (
				auto & e
			) {
					return e.append ? e.append.get().size() : k_none_size;
				});
			auto reference_slot_list = List<ReferenceSlot>{layer_count};
			dest.clean();
			dest.append_many(source_animation.frame.size(), layer_count);
			auto current_frame_index = k_begin_index;
			for (auto & frame : source_animation.frame) {
				if (frame.append) {
					for (auto & e : frame.append.get()) {
						reference_slot_list.append();
						auto & new_reference_slot            = reference_slot_list.last();
						new_reference_slot.begin_frame_index = current_frame_index;
						new_reference_slot.index             = e.index;
						new_reference_slot.animation         = e.animation;
						new_reference_slot.reference         = e.reference;
						new_reference_slot.change_matrix.init();
						new_reference_slot.change_color.init();
					}
				}
				if (frame.change) {
					for (auto & e : frame.change.get()) {
						for (auto & i : SizeRange{reference_slot_list.size()}) {
							if (reference_slot_list[i].index == e.index) {
								reference_slot_list[i].change_matrix.mix(e);
								reference_slot_list[i].change_color.load(e);
								break;
							}
						}
					}
				}
				if (frame.remove) {
					for (auto & e : frame.remove.get()) {
						for (auto & i : SizeRange{reference_slot_list.size()}) {
							if (reference_slot_list[i].index == e.index) {
								reference_slot_list.remove_at(i);
								break;
							}
						}
					}
				}
				for (auto & reference_slot : reference_slot_list) {
					if (reference_slot.animation) {
						if (reference_slot.reference > animation_list.size()) {
							throw ExceptionMessage{"PopcapAnimation-Convert-CSSInformation : reference index too big : {} - {}"_sf(reference_slot.reference, animation_list.size())};
						}
						auto & reference_animation_sprite_list = animation_list[reference_slot.reference];
						auto   target_frame_index              = Size{};
						if (current_frame_index - reference_slot.begin_frame_index < reference_animation_sprite_list.size()) {
							target_frame_index = current_frame_index - reference_slot.begin_frame_index;
						} else {
							target_frame_index = reference_animation_sprite_list.last_index();
						}
						for (auto & current_reference_sprite_item : reference_animation_sprite_list[target_frame_index]) {
							dest[current_frame_index].append(current_reference_sprite_item);
							auto & layer = dest[current_frame_index].last();
							layer.id.data.prepend(reference_slot.index);
							layer.parent.data.append(reference_slot.reference);
							mix_change_martix(layer.sprite.matrix, reference_slot.change_matrix);
							layer.sprite.color = reference_slot.change_color;
						}
					} else {
						dest[current_frame_index].append();
						auto & new_layer = dest[current_frame_index].last();
						new_layer.id.data.alloc(2_size);
						new_layer.id.data.prepend(reference_slot.reference);
						new_layer.id.data.prepend(reference_slot.index);
						new_layer.parent.data.alloc();
						new_layer.sprite.index  = reference_slot.reference;
						new_layer.sprite.matrix = reference_slot.change_matrix;
						new_layer.sprite.color  = reference_slot.change_color;
					}
				}
				++current_frame_index;
			}
			return;
		}

		inline auto make_sub_label_list (
			const List<PopcapAnimation::Information::AnimationWithLabel> & source,
			List<String> &                                                 dest
		) -> None {
			dest.convert(source, [] (
				auto & dest_element,
				auto & source_element
			) {
					dest_element = source_element.label;
				});
			return;
		}

		inline auto make_label_range_map (
			const List<PopcapAnimation::Information::Frame> & source,
			Map<String, Information::RangeInformation> &      dest
		) -> None {
			dest.alloc(Range::accumulate(source, 1_size, [] (
				auto & e
			) {
					return e.label ? 1_size : k_none_size;
				}));
			for (auto & i : SizeRange{source.size()}) {
				if (source[i].label) {
					if (!dest.empty()) {
						dest.last().value.end = i - k_next_index;
					}
					dest.append(source[i].label.get(), Information::RangeInformation{i, k_default});
				} else if (i == k_begin_index) {
					dest.append(k_empty_string, Information::RangeInformation{i, k_default});
				}
			}
			if (!dest.empty()) {
				dest.last().value.end = source.last_index();
			}
			return;
		}

		inline auto make_sprite_list (
			const List<PopcapAnimation::Information::Sprite> & source,
			List<Information::Sprite> &                        dest
		) -> None {
			dest.convert(source, [] (
				auto & css_sprite,
				auto & sprite
			) {
					css_sprite.source      = sprite.source;
					css_sprite.size.width  = cast_wrapper<FloatingNumber64>(sprite.size.width) * sprite.scale.x;
					css_sprite.size.height = cast_wrapper<FloatingNumber64>(sprite.size.height) * sprite.scale.y;
					css_sprite.origin.x    = sprite.position.x;
					css_sprite.origin.y    = sprite.position.y;
				});
			return;
		}

		inline auto make_animation_list (
			const List<List<LayerSlot>> &                      source,
			const List<PopcapAnimation::Information::Sprite> & sprite,
			List<Information::Layer> &                         dest
		) -> None {
			auto layer_count = Range::accumulate(source, k_none_size,
				[] (
				auto & e
			) {
					return e.size();
				});
			auto id_list = List<Information::LayerID>{layer_count};
			dest.alloc(layer_count);
			for (auto & layer_list_index : SizeRange{source.size()}) {
				for (auto & layer_index : SizeRange{source[layer_list_index].size()}) {
					if (auto & id = source[layer_list_index][layer_index].id; !Range::has(id_list, id)) {
						id_list.append(id);
						auto parent    = ConstantReferenceOptional<Information::LayerID>{};
						auto range_end = Optional<Size>{};
						dest.append();
						auto & t      = dest.last();
						t.id          = id;
						t.range.begin = layer_list_index;
						t.frame.alloc(source.size() - layer_list_index);
						auto layer_list_index_2 = Size{};
						for (layer_list_index_2 = layer_list_index; layer_list_index_2 < source.size(); ++layer_list_index_2) {
							if (auto sprite_layer_index = Range::find_if_th(source[layer_list_index_2],
								[&id] (
								auto & e
							) {
									return e.id == id;
								})) {
								auto & layer = source[layer_list_index_2][sprite_layer_index.get()];
								t.frame.append();
								auto & tt        = t.frame.last();
								tt.transform     = layer.sprite.matrix;
								tt.discolor      = layer.sprite.color;
								auto last_change = ChangeMatrix{};
								last_change.init();
								last_change.x = sprite[id.data.last()].position.x;
								last_change.y = sprite[id.data.last()].position.y;
								mix_change_martix(tt.transform, last_change);
								if (!parent) {
									parent.save(layer.parent);
								}
							} else {
								range_end.create(layer_list_index_2 - k_next_index);
								break;
							}
						}
						t.range.end = range_end ? range_end.get() : layer_list_index_2;
						t.parent    = parent.get();
					}
				}
			}
			// TODO : legacy
			/*for (Size idx = kFirstItemIdx; idx < dst.$sz; ++idx)
				for (Size Idx_2 = (idx + kThSecond); Idx_2 < dst.$sz; ++Idx_2) {
					if (CmpLayerID(dst[idx].$id, dst[Idx_2].$id))
						dst.swpItem(idx, Idx_2);
				}*/
			return;
		}

		inline auto convert (
			const PopcapAnimation::Information::Package & popcap_animation,
			Information::Package &                        ripe_layer
		) -> None {
			// TODO : legacy
			/*List<$Info::ImageInfo> imageList;
			{
				for (auto & e : imageList)
					if (e.$src.range().existSub("add$"_r.range()))
						e.$src.eraseSub("add$"_r);
			}*/
			{
				ripe_layer.origin      = popcap_animation.origin;
				ripe_layer.frame_rate  = popcap_animation.main_animation.information.rate;
				ripe_layer.frame_count = popcap_animation.main_animation.information.count;
				make_sub_label_list(popcap_animation.animation, ripe_layer.sub);
				make_label_range_map(popcap_animation.main_animation.frame, ripe_layer.label);
				make_sprite_list(popcap_animation.sprite, ripe_layer.sprite);
				{
					auto cache_layer = List<List<List<LayerSlot>>>{popcap_animation.animation.size()};
					for (auto & e : popcap_animation.animation) {
						auto & t = *cache_layer.end();
						make_layer_list_list(t, e, popcap_animation.sprite, cache_layer);
						cache_layer.expand_size(1_size);
					}
					auto layer_list_list = List<List<LayerSlot>>{};
					make_layer_list_list(layer_list_list, popcap_animation.main_animation, popcap_animation.sprite, cache_layer);
					make_animation_list(layer_list_list, popcap_animation.sprite, ripe_layer.layer);
				}
			}
			return;
		}

		// ------------------------------------------------

	}

	// ------------------------------------------------

}

// ------------------------------------------------
