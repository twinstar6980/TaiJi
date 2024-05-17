#pragma once

// ------------------------------------------------

#include "core/tool/animation/popcap_animation/information.hpp"
#include "core/tool/animation/popcap_animation/convert/common.hpp"

#include "core/file_system/file_system.hpp"
#include "core/string/regular_expression.hpp"
#include "core/json/parser.hpp"

#include <cmath>
#include <iostream>

#include "third_party/tinyxml2/tinyxml2.h"

// ------------------------------------------------

namespace TwinKleS::Tool::Animation::PopcapAnimation::Convert::Flash {

	// ------------------------------------------------

	struct LayerResult {
		Boolean animation;
		Size    reference;
		Size    frame_begin_index;
		Size    frame_sum;
	};

	struct LayerState {
		Boolean                animation;
		Size                   reference;
		Size                   duration;
		ChangeMatrix           matrix;
		ChangeColor            color;
		Boolean                is_new_frame_now;
		Boolean                next_erase;
		tinyxml2::XMLElement * pointer = nullptr;
	};

	// ------------------------------------------------

	inline const auto k_xfl_content = ConstantStringView{"PROXY-CS5"_sv};

	// ------------------------------------------------

	inline constexpr auto k_stdandard_image_size = Size{1200_size};

	inline auto g_resource_scale_rate = FloatingNumber64{1.0_floating_number_64};

	// ------------------------------------------------

	namespace TinyXML {

		// ------------------------------------------------

		inline auto create_element (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & name
		) -> tinyxml2::XMLElement* {
			return document.NewElement(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(name).begin()).get());
		}

		inline auto create_cdata (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & text
		) -> tinyxml2::XMLText* {
			auto x = document.NewText(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(text).begin()).get());
			x->SetCData(true);
			return x;
		}

		// ------------------------------------------------

		inline auto get_tag (
			const tinyxml2::XMLElement & node
		) -> String {
			return make_string(node.Value());
		}

		// ------------------------------------------------

		inline auto destory (
			tinyxml2::XMLNode & node
		) -> None {
			//node.Clear();
			return;
		}

		// ------------------------------------------------

		inline auto append_child (
			tinyxml2::XMLNode & parent,
			tinyxml2::XMLNode * child
		) -> tinyxml2::XMLNode& {
			parent.LinkEndChild(child);
			return parent;
		}

		inline auto wrapper_shell (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & shell_name,
			tinyxml2::XMLElement *     element
		) -> tinyxml2::XMLElement* {
			auto x_shell = create_element(document, shell_name);
			append_child(*x_shell, element);
			return x_shell;
		}

		// ------------------------------------------------

		inline auto set_attribute (
			tinyxml2::XMLElement &     node,
			const ConstantStringView & name,
			const ConstantStringView & value
		) -> None {
			node.SetAttribute(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(name).begin()).get(), cast_pointer<CharacterZ>(to_c_style_end_identifier_string(value).begin()).get());
			return;
		}

		inline auto set_attribute (
			tinyxml2::XMLElement &     node,
			const ConstantStringView & name,
			const FloatingNumber64 &   value
		) -> None {
			return set_attribute(node, name, "{:.6f}"_sf(value));
		}

		inline auto set_attribute (
			tinyxml2::XMLElement &     node,
			const ConstantStringView & name,
			const Size &               value
		) -> None {
			return set_attribute(node, name, "{}"_sf(value));
		}

		// ------------------------------------------------

		inline auto get_attribute (
			const tinyxml2::XMLElement & node,
			const ConstantStringView &   name,
			String &                     value
		) -> None {
			value = make_string_view(node.Attribute(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(name).begin()).get()));
			return;
		}

		inline auto get_attribute (
			const tinyxml2::XMLElement & node,
			const ConstantStringView &   name,
			FloatingNumber64 &           value
		) -> None {
			node.QueryDoubleAttribute(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(name).begin()).get(), &value.get());
			return;
		}

		inline auto get_attribute (
			const tinyxml2::XMLElement & node,
			const ConstantStringView &   name,
			Size &                       value
		) -> None {
			auto raw_value = unsigned{};
			node.QueryUnsignedAttribute(cast_pointer<CharacterZ>(to_c_style_end_identifier_string(name).begin()).get(), &raw_value);
			value.set(raw_value);
			return;
		}

		inline auto get_attribute (
			const tinyxml2::XMLElement & node,
			const ConstantStringView &   name
		) -> String {
			auto result = String{};
			get_attribute(node, name, result);
			return result;
		}

		// ------------------------------------------------

		inline auto read (
			const Path &            path,
			tinyxml2::XMLDocument & document
		) -> None {
			document.LoadFile(cast_pointer<CharacterZ>(as_lvalue(path.to_string().begin())).get());
			return;
		}
		
		class MyTinyXML2Printer : public tinyxml2::XMLPrinter {

		public: //

			auto PrintSpace (
				int depth
			) -> void override {
				for (int i = 0; i < depth; ++i) {
					Write("\t");
				}
			}

		};

		inline auto write (
			const Path &                  path,
			const tinyxml2::XMLDocument & document
		) -> None {
			auto printer = MyTinyXML2Printer{};
			document.Print(&printer);
			FileSystem::write_character(path, make_string_view(printer.CStr(), printer.CStrSize() - 1));
			return;
		}

		// ------------------------------------------------

	}

	// ------------------------------------------------

	struct ExtraInformation {

		// ------------------------------------------------

		StaticArray<Byte, Structure::k_unknown_data_size.get()> unknown;
		XYPair<FloatingNumber64>                                origin;
		Map<String, WHPair<Size>>                               sprite_size;
		Map<String, String>                                     sprite_mapper;
		Map<String, String>                                     animation_mapper;

		// ------------------------------------------------

		friend M_json_from(ExtraInformation) {
			M_from_json_as_object_to_o;
			M_from_json_for_member_of_o_by(unknown, StringParser::write_byte_list(t.unknown.view()));
			M_from_json_for_member_of_o(origin);
			M_from_json_for_member_of_o(sprite_size);
			M_from_json_for_member_of_o(sprite_mapper);
			M_from_json_for_member_of_o(animation_mapper);
		}

		// ------------------------------------------------

	};

	// ------------------------------------------------

	namespace From {

		// ------------------------------------------------

		inline auto make_x_transformation_point_with_shell (
			tinyxml2::XMLDocument &          document,
			const XYPair<FloatingNumber64> & origin
		) -> tinyxml2::XMLElement* {
			return TinyXML::wrapper_shell(document, "transformationPoint"_sv, [&] () {
				auto x = TinyXML::create_element(document, "Point"_sv);
				TinyXML::set_attribute(*x, "x"_sv, origin.x);
				TinyXML::set_attribute(*x, "y"_sv, origin.y);
				return x;
			}());
		}

		inline auto make_x_matrix_with_shell (
			tinyxml2::XMLDocument & document,
			const ChangeMatrix &    matrix
		) -> tinyxml2::XMLElement* {
			return TinyXML::wrapper_shell(document, "matrix"_sv, [&] () {
				auto x = TinyXML::create_element(document, "Matrix"_sv);
				TinyXML::set_attribute(*x, "a"_sv, matrix.a);
				TinyXML::set_attribute(*x, "b"_sv, matrix.b);
				TinyXML::set_attribute(*x, "c"_sv, matrix.c);
				TinyXML::set_attribute(*x, "d"_sv, matrix.d);
				TinyXML::set_attribute(*x, "tx"_sv, matrix.x);
				TinyXML::set_attribute(*x, "ty"_sv, matrix.y);
				return x;
			}());
		}

		inline auto make_x_color_with_shell (
			tinyxml2::XMLDocument & document,
			const ChangeColor &     color
		) -> tinyxml2::XMLElement* {
			return TinyXML::wrapper_shell(document, "color"_sv, [&] () {
				auto x = TinyXML::create_element(document, "Color"_sv);
				TinyXML::set_attribute(*x, "redMultiplier"_sv, color.red);
				TinyXML::set_attribute(*x, "greenMultiplier"_sv, color.green);
				TinyXML::set_attribute(*x, "blueMultiplier"_sv, color.blue);
				TinyXML::set_attribute(*x, "alphaMultiplier"_sv, color.alpha);
				return x;
			}());
		}

		inline auto make_x_matrix_with_shell_for_bitmap_instance (
			tinyxml2::XMLDocument &          document,
			const XYPair<FloatingNumber64> & scale,
			const XYPair<FloatingNumber64> & position
		) -> tinyxml2::XMLElement* {
			return TinyXML::wrapper_shell(document, "matrix"_sv, [&] () {
				auto x = TinyXML::create_element(document, "Matrix"_sv);
				TinyXML::set_attribute(*x, "a"_sv, scale.x * g_resource_scale_rate);
				TinyXML::set_attribute(*x, "d"_sv, scale.y * g_resource_scale_rate);
				TinyXML::set_attribute(*x, "tx"_sv, position.x);
				TinyXML::set_attribute(*x, "ty"_sv, position.y);
				return x;
			}());
		}

		inline auto make_x_frame_only_symbol_instance (
			tinyxml2::XMLDocument &    document,
			const Size &               index,
			const Size &               duration,
			const ConstantStringView & library,
			const ChangeMatrix &       matrix,
			const ChangeColor &        color
		) -> tinyxml2::XMLElement* {
			auto x_frame = TinyXML::create_element(document, "DOMFrame"_sv);
			TinyXML::set_attribute(*x_frame, "index"_sv, index);
			TinyXML::set_attribute(*x_frame, "duration"_sv, duration);
			TinyXML::append_child(*x_frame, TinyXML::wrapper_shell(document, "elements"_sv, [&] () {
				auto x_symbol_instance = TinyXML::create_element(document, "DOMSymbolInstance"_sv);
				TinyXML::set_attribute(*x_symbol_instance, "libraryItemName"_sv, library);
				TinyXML::append_child(*x_symbol_instance, make_x_matrix_with_shell(document, matrix));
				TinyXML::append_child(*x_symbol_instance, make_x_color_with_shell(document, color));
				return x_symbol_instance;
			}()));
			return x_frame;
		}

		inline auto make_x_empty_frame (
			tinyxml2::XMLDocument & document,
			const Size &            index,
			const Size &            duration
		) -> tinyxml2::XMLElement* {
			auto x_frame = TinyXML::create_element(document, "DOMFrame"_sv);
			TinyXML::set_attribute(*x_frame, "index"_sv, index);
			TinyXML::set_attribute(*x_frame, "duration"_sv, duration);
			return x_frame;
		}

		inline auto make_x_label_frame (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & label,
			const Size &               index,
			const Size &               duration
		) -> tinyxml2::XMLElement* {
			auto x_frame = make_x_empty_frame(document, index, duration);
			TinyXML::set_attribute(*x_frame, "name"_sv, label);
			return x_frame;
		}

		inline auto make_x_command_frame (
			tinyxml2::XMLDocument &    document,
			const Size &               index,
			const ConstantStringView & command
		) -> tinyxml2::XMLElement* {
			auto x_frame = TinyXML::create_element(document, "DOMFrame"_sv);
			TinyXML::set_attribute(*x_frame, "index"_sv, index);
			TinyXML::append_child(*x_frame, TinyXML::wrapper_shell(document, "Actionscript"_sv, [&] () {
				auto x_script = TinyXML::create_element(document, "script"_sv);
				TinyXML::append_child(*x_script, TinyXML::create_cdata(document, command));
				return x_script;
			}()));
			return x_frame;
		}

		inline auto make_sprite_x_timeline_with_shell (
			tinyxml2::XMLDocument &     document,
			const ConstantStringView &  name,
			const Information::Sprite & sprite
		) -> tinyxml2::XMLElement* {
			auto x_timeline = TinyXML::create_element(document, "DOMTimeline"_sv);
			TinyXML::set_attribute(*x_timeline, "name"_sv, name);
			TinyXML::append_child(*x_timeline, TinyXML::wrapper_shell(document, "layers"_sv, TinyXML::wrapper_shell(document, "DOMLayer"_sv, TinyXML::wrapper_shell(document, "frames"_sv, [&] () {
				auto x_frame = TinyXML::create_element(document, "DOMFrame"_sv);
				TinyXML::set_attribute(*x_frame, "index"_sv, k_begin_index);
				TinyXML::append_child(*x_frame, TinyXML::wrapper_shell(document, "elements"_sv, [&] () {
					auto x_bitmap_instance = TinyXML::create_element(document, "DOMBitmapInstance"_sv);
					TinyXML::set_attribute(*x_bitmap_instance, "libraryItemName"_sv, sprite.source);
					TinyXML::append_child(*x_bitmap_instance, make_x_matrix_with_shell_for_bitmap_instance(document, sprite.scale, sprite.position));
					TinyXML::append_child(*x_bitmap_instance, make_x_transformation_point_with_shell(document, XYPair<FloatingNumber64>(-sprite.position.x, -sprite.position.y)));
					return x_bitmap_instance;
				}()));
				return x_frame;
			}()))));
			return TinyXML::wrapper_shell(document, "timeline"_sv, x_timeline);
		}

		inline auto make_animation_x_timeline_with_shell (
			tinyxml2::XMLDocument &          document,
			const ConstantStringView &       name,
			const List<Information::Frame> & frame_list,
			const List<String> &             sprite_id,
			const List<String> &             animation_label
		) -> tinyxml2::XMLElement* {
			auto       layer_sum = Range::accumulate(frame_list, k_none_size, [] (
				auto & e
			) {
					return e.append ? e.append.get().size() : k_none_size;
				});
			auto layer_is_enable          = List<Boolean>{};
			auto layer_state_list         = List<LayerState>{};
			auto layer_result_list        = List<LayerResult>{};
			auto x_layer_frame_shell_list = List<List<tinyxml2::XMLElement *>>{};
			layer_is_enable.alloc_full(layer_sum);
			layer_state_list.alloc_full(layer_sum);
			layer_result_list.alloc_full(layer_sum);
			x_layer_frame_shell_list.alloc_full(layer_sum);
			for (auto & frame_index : SizeRange{frame_list.size()}) {
				auto & frame = frame_list[frame_index];
				if (frame.remove) {
					for (auto & e : frame.remove.get()) {
						layer_state_list[e.index].next_erase = k_true;
					}
				}
				if (frame.append) {
					for (auto & e : frame.append.get()) {
						layer_is_enable[e.index] = k_true;
						auto & t                 = layer_state_list[e.index];
						t.reference              = e.reference;
						t.animation              = e.animation;
						t.duration               = frame_index;
						t.matrix.init();
						t.color.init();
						t.next_erase                                 = k_false;
						layer_result_list[e.index].animation         = e.animation;
						layer_result_list[e.index].reference         = e.reference;
						layer_result_list[e.index].frame_begin_index = frame_index;
						x_layer_frame_shell_list[e.index].alloc(frame_list.size() - frame_index);
					}
				}
				for (auto & i : SizeRange{layer_is_enable.size()}) {
					if (layer_is_enable[i]) {
						layer_state_list[i].is_new_frame_now = k_false;
					}
				}
				if (frame.change) {
					for (auto & e : frame.change.get()) {
						auto & t = layer_state_list[e.index];
						t.matrix.mix(e);
						t.color.load(e);
						t.is_new_frame_now = k_true;
					}
				}
				for (auto & i : SizeRange{layer_is_enable.size()}) {
					if (layer_is_enable[i]) {
						auto & t = layer_state_list[i];
						if ((t.is_new_frame_now || t.next_erase) && t.pointer != nullptr) {
							TinyXML::set_attribute(*t.pointer, "duration"_sv, t.duration);
						}
						if (t.is_new_frame_now) {
							t.duration = 1_size;
							t.pointer  = make_x_frame_only_symbol_instance(
								document,
								frame_index, t.duration,
								(t.animation ? animation_label : sprite_id)[t.reference],
								t.matrix, t.color
							);
							x_layer_frame_shell_list[i].append(t.pointer);
						} else {
							++t.duration;
						}
						if (t.next_erase) {
							layer_is_enable[i]             = k_false;
							layer_result_list[i].frame_sum = frame_index - layer_result_list[i].frame_begin_index;
						}
					}
				}
			}
			for (auto & i : SizeRange{layer_is_enable.size()}) {
				if (layer_is_enable[i]) {
					layer_result_list[i].frame_sum = frame_list.size() - layer_result_list[i].frame_begin_index;
					auto & state                   = layer_state_list[i];
					if (state.pointer != nullptr)
						TinyXML::set_attribute(*state.pointer, "duration"_sv, state.duration);
				}
			}
			auto x_timeline    = TinyXML::create_element(document, "DOMTimeline"_sv);
			auto x_layer_shell = TinyXML::create_element(document, "layers"_sv);
			TinyXML::set_attribute(*x_timeline, "name"_sv, name);
			for (auto j = x_layer_frame_shell_list.size(); j > k_begin_index; --j) {
				auto i       = j - k_next_index;
				auto x_layer = TinyXML::create_element(document, "DOMLayer"_sv);
				TinyXML::set_attribute(*x_layer, "name"_sv, "l_{}"_sf(i));
				auto x_frame_shell = TinyXML::create_element(document, "frames"_sv);
				if (layer_result_list[i].frame_begin_index > k_begin_index) {
					TinyXML::append_child(*x_frame_shell, make_x_empty_frame(document, k_begin_index, layer_result_list[i].frame_begin_index));
				}
				for (auto & e : x_layer_frame_shell_list[i]) {
					TinyXML::append_child(*x_frame_shell, e);
				}
				TinyXML::append_child(*x_layer, x_frame_shell);
				TinyXML::append_child(*x_layer_shell, x_layer);
			}
			TinyXML::append_child(*x_timeline, x_layer_shell);
			return TinyXML::wrapper_shell(document, "timeline"_sv, x_timeline);
		}

		// labels/actions/audios
		inline auto make_other_x_layer (
			tinyxml2::XMLDocument &          document,
			const List<Information::Frame> & frame_list,
			tinyxml2::XMLElement *&          x_label,
			tinyxml2::XMLElement *&          x_action,
			tinyxml2::XMLElement *&          x_audio
		) -> None {
			auto x_label_frame_shell       = TinyXML::create_element(document, "frames"_sv);
			auto x_action_frame_shell      = TinyXML::create_element(document, "frames"_sv);
			auto x_audio_frame_shell       = TinyXML::create_element(document, "frames"_sv);
			auto current_label             = String{};
			auto current_label_begin_index = Size{};
			if (!frame_list.first().label) {
				current_label             = ""_s;
				current_label_begin_index = k_begin_index;
			}
			auto last_action_index = k_begin_index;
			auto last_audio_index  = k_begin_index;
			for (auto & frame_index : SizeRange{frame_list.size()}) {
				auto & frame = frame_list[frame_index];
				if (frame.label) {
					if (frame_index > k_begin_index) {
						TinyXML::append_child(*x_label_frame_shell, make_x_label_frame(document, current_label, current_label_begin_index, frame_index - current_label_begin_index));
					}
					current_label             = frame.label.get();
					current_label_begin_index = frame_index;
				}
				if (frame.stop || frame.command) {
					auto action_command = List<String>{};
					auto audio_command  = List<String>{};
					if (frame.stop) {
						action_command.append("stop();"_sv);
					}
					if (frame.command) {
						for (auto & fs_command : frame.command.get()) {
							if (fs_command.command.equal_icase("PlaySample"_sv)) {
								audio_command.append(R"(fscommand("{}", "{}");)"_sf(fs_command.command, fs_command.argument.get()));
							} else {
								if (!fs_command.argument) {
									action_command.append(R"(fscommand("{}");)"_sf(fs_command.command));
								} else {
									action_command.append(R"(fscommand("{}", "{}");)"_sf(fs_command.command, fs_command.argument.get()));
								}
							}
						}
					}
					if (!action_command.empty()) {
						if (last_action_index != frame_index - last_action_index) {
							TinyXML::append_child(*x_action_frame_shell, make_x_empty_frame(document, last_action_index, frame_index - last_action_index));
						}
						TinyXML::append_child(*x_action_frame_shell, make_x_command_frame(document, frame_index, catenate_list<String, ListCatenateDelimitStyle::inside>(action_command, '\n'_character)));
						last_action_index = frame_index + k_next_index;
					}
					if (!audio_command.empty()) {
						if (last_audio_index != frame_index - last_audio_index) {
							TinyXML::append_child(*x_audio_frame_shell, make_x_empty_frame(document, last_audio_index, frame_index - last_audio_index));
						}
						TinyXML::append_child(*x_audio_frame_shell, make_x_command_frame(document, frame_index, catenate_list<String, ListCatenateDelimitStyle::inside>(audio_command, '\n'_character)));
						last_audio_index = frame_index + k_next_index;
					}
				}
			}
			if (frame_list.size() > k_none_size) {
				TinyXML::append_child(*x_label_frame_shell, make_x_label_frame(document, current_label, current_label_begin_index, frame_list.size() - current_label_begin_index));
			}
			x_label = TinyXML::create_element(document, "DOMLayer"_sv);
			TinyXML::set_attribute(*x_label, "name"_sv, "label"_sv);
			TinyXML::append_child(*x_label, x_label_frame_shell);
			x_action = TinyXML::create_element(document, "DOMLayer"_sv);
			TinyXML::set_attribute(*x_action, "name"_sv, "action"_sv);
			TinyXML::append_child(*x_action, x_action_frame_shell);
			x_audio = TinyXML::create_element(document, "DOMLayer"_sv);
			TinyXML::set_attribute(*x_audio, "name"_sv, "audio"_sv);
			TinyXML::append_child(*x_audio, x_audio_frame_shell);
			return;
		}

		inline auto make_x_symbol_item (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & name,
			tinyxml2::XMLElement *     x_timeline
		) -> tinyxml2::XMLElement* {
			auto x = TinyXML::create_element(document, "DOMSymbolItem"_sv);
			TinyXML::set_attribute(*x, "name"_sv, name);
			TinyXML::append_child(*x, x_timeline);
			return x;
		}

		inline auto make_x_bitmap_item (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & path
		) -> tinyxml2::XMLElement* {
			auto x = TinyXML::create_element(document, "DOMBitmapItem"_sv);
			TinyXML::set_attribute(*x, "name"_sv, path);
			TinyXML::set_attribute(*x, "href"_sv, path + ".png"_sv);
			return x;
		}

		inline auto make_x_include (
			tinyxml2::XMLDocument &    document,
			const ConstantStringView & href
		) -> tinyxml2::XMLElement* {
			auto x = TinyXML::create_element(document, "Include"_sv);
			TinyXML::set_attribute(*x, "href"_sv, href);
			return x;
		}

		inline auto insert_flash_document_information (
			tinyxml2::XMLElement & x
		) -> None {
			TinyXML::set_attribute(x, "xmlns"_sv, "http://ns.adobe.com/xfl/2008/"_sv);
			TinyXML::set_attribute(x, "xmlns:xsi"_sv, "http://www.w3.org/2001/XMLSchema-instance"_sv);
			return;
		}

		inline auto convert (
			const Information::Package & popcap_animation,
			const Path &                 xfl_directory,
			const Size &                 image_size
		) -> None {
			auto document                = tinyxml2::XMLDocument{};
			g_resource_scale_rate        = cast_wrapper<FloatingNumber64>(k_stdandard_image_size) / cast_wrapper<FloatingNumber64>(image_size);
			auto extra_information       = ExtraInformation{};
			extra_information.unknown    = popcap_animation.unknown;
			extra_information.origin     = popcap_animation.origin;
			auto directory_before_change = FileSystem::current_directory();
			FileSystem::change_directory_force(xfl_directory);
			FileSystem::create_directory("LIBRARY"_path);
			auto x_document = TinyXML::create_element(document, "DOMDocument"_sv);
			auto x_symbols  = TinyXML::create_element(document, "symbols"_sv);
			auto x_media    = TinyXML::create_element(document, "media"_sv);
			extra_information.sprite_size.alloc(popcap_animation.sprite.size());
			extra_information.sprite_mapper.alloc(popcap_animation.sprite.size());
			extra_information.animation_mapper.alloc(popcap_animation.animation.size());
			auto sprite_id_list       = List<String>{popcap_animation.sprite.size()};
			auto animation_label_list = List<String>{popcap_animation.animation.size()};
			for (auto & i : SizeRange{popcap_animation.sprite.size()}) {
				auto & e  = popcap_animation.sprite[i];
				auto   id = "sprite_{}"_sf(i);
				extra_information.sprite_size.append(e.id, e.size);
				extra_information.sprite_mapper.append(id, e.id);
				TinyXML::append_child(*x_symbols, make_x_include(document, "{}.xml"_sf(id)));
				TinyXML::append_child(*x_media, make_x_bitmap_item(document, e.source));
				auto x_sprite_document = make_x_symbol_item(document, id, make_sprite_x_timeline_with_shell(document, id, e));
				insert_flash_document_information(*x_sprite_document);
				document.InsertFirstChild(x_sprite_document);
				TinyXML::write("LIBRARY"_path + "{}.xml"_sf(id), document);
				sprite_id_list.append(id);
				document.Clear();
			}
			for (auto & i : SizeRange{popcap_animation.animation.size()}) {
				auto & e  = popcap_animation.animation[i];
				auto   id = "animation_{}"_sf(i);
				extra_information.animation_mapper.append(id, e.label);
				TinyXML::append_child(*x_symbols, make_x_include(document, "{}.xml"_sf(id)));
				auto x_animation_document = make_x_symbol_item(document, id, make_animation_x_timeline_with_shell(document, id, e.frame, sprite_id_list, animation_label_list));
				insert_flash_document_information(*x_animation_document);
				document.InsertFirstChild(x_animation_document);
				TinyXML::write("LIBRARY"_path + "{}.xml"_sf(id), document);
				animation_label_list.append(id);
				document.Clear();
			}
			TinyXML::append_child(*x_symbols, make_x_include(document, "main_animation.xml"_sv));
			{
				auto x_main_animation_document = make_x_symbol_item(document, "main_animation"_sv, make_animation_x_timeline_with_shell(document, "main_animation"_sv, popcap_animation.main_animation.frame, sprite_id_list, animation_label_list));
				insert_flash_document_information(*x_main_animation_document);
				document.InsertFirstChild(x_main_animation_document);
				TinyXML::write("LIBRARY"_path + "main_animation.xml"_sv, document);
				document.Clear();
			}
			return;
			//TinyXML::append_child(*x_document, x_media);
			//TinyXML::append_child(*x_document, x_symbols);
			{
				auto x_label_layer  = PointerZ<tinyxml2::XMLElement>{};
				auto x_action_layer = PointerZ<tinyxml2::XMLElement>{};
				auto x_audio_layer  = PointerZ<tinyxml2::XMLElement>{};
				make_other_x_layer(document, popcap_animation.main_animation.frame, x_label_layer, x_action_layer, x_audio_layer);
				auto x_timeline = TinyXML::create_element(document, "DOMTimeline"_sv);

				TinyXML::set_attribute(*x_timeline, "name"_sv, "animation"_sv);

				auto x_animation_layer = TinyXML::create_element(document, "DOMLayer"_sv);
				TinyXML::set_attribute(*x_animation_layer, "name"_sv, "animation"_sv);

				TinyXML::append_child(*x_animation_layer, TinyXML::wrapper_shell(document, "frames"_sv, [&] () {
					auto x_animation_frame = TinyXML::create_element(document, "DOMFrame"_sv);
					TinyXML::set_attribute(*x_animation_frame, "index"_sv, k_begin_index);
					TinyXML::set_attribute(*x_animation_frame, "duration"_sv, popcap_animation.main_animation.information.count);
					TinyXML::append_child(*x_animation_frame, TinyXML::wrapper_shell(document, "elements"_sv, [&] () {
						auto x_animation_symbol_instance = TinyXML::create_element(document, "DOMSymbolInstance"_sv);
						TinyXML::set_attribute(*x_animation_symbol_instance, "libraryItemName"_sv, "main_animation"_sv);
						return x_animation_symbol_instance;
					}()));
					return x_animation_frame;
				}()));
				TinyXML::append_child(*x_timeline, [&] () {
					auto x_layer_shell = TinyXML::create_element(document, "layers"_sv);
					TinyXML::append_child(*x_layer_shell, x_label_layer);
					TinyXML::append_child(*x_layer_shell, x_action_layer);
					TinyXML::append_child(*x_layer_shell, x_audio_layer);
					TinyXML::append_child(*x_layer_shell, x_animation_layer);
					return x_layer_shell;
				}());
				TinyXML::append_child(*x_document, TinyXML::wrapper_shell(document, "timelines"_sv, x_timeline));
			}
			TinyXML::set_attribute(*x_document, "frameRate"_sv, popcap_animation.main_animation.information.rate);
			TinyXML::set_attribute(*x_document, "width"_sv, 0_size);
			TinyXML::set_attribute(*x_document, "height"_sv, 0_size);
			TinyXML::set_attribute(*x_document, "xflVersion"_sv, "2.97"_sv);
			{
				insert_flash_document_information(*x_document);
				document.InsertFirstChild(x_document);
				TinyXML::write("DOMDocument.xml"_path, document);
				document.Clear();
			}
			FileSystem::write_character("main.xfl"_path, k_xfl_content);
			JSON::write("extra.json"_path, extra_information);
			FileSystem::change_directory(directory_before_change);
			return;
		}

		// ------------------------------------------------

	}

	// ------------------------------------------------

	namespace To {

		// ------------------------------------------------

		inline auto load_x_matrix_with_shell (
			const tinyxml2::XMLElement & x_element,
			ChangeMatrix &               matrix
		) -> None {
			auto & x = *x_element.FirstChildElement("Matrix");
			TinyXML::get_attribute(x, "a"_sv, matrix.a);
			TinyXML::get_attribute(x, "b"_sv, matrix.b);
			TinyXML::get_attribute(x, "c"_sv, matrix.c);
			TinyXML::get_attribute(x, "d"_sv, matrix.d);
			TinyXML::get_attribute(x, "tx"_sv, matrix.x);
			TinyXML::get_attribute(x, "ty"_sv, matrix.y);
			return;
		}

		inline auto load_x_color_with_shell (
			const tinyxml2::XMLElement & x_element,
			ChangeColor &                color
		) -> None {
			auto & x = *x_element.FirstChildElement("Color");
			TinyXML::get_attribute(x, "redMultiplier"_sv, color.red);
			TinyXML::get_attribute(x, "greenMultiplier"_sv, color.green);
			TinyXML::get_attribute(x, "blueMultiplier"_sv, color.blue);
			TinyXML::get_attribute(x, "alphaMultiplier"_sv, color.alpha);
			return;
		}

		inline auto load_sprite_x_timeline (
			const tinyxml2::XMLElement & x_timeline,
			String &                     name,
			Information::Sprite &        sprite
		) -> None {
			TinyXML::get_attribute(x_timeline, "name"_sv, name);
			auto & x_dom_bitmap_instance = *x_timeline
											.FirstChildElement()->FirstChildElement()
											->FirstChildElement()->FirstChildElement()
											->FirstChildElement()->FirstChildElement();
			TinyXML::get_attribute(x_dom_bitmap_instance, "libraryItemName"_sv, sprite.source);
			//auto xTransformationPoint = xDOMBitmapInstance->FirstChildElement("matrix")->FirstChildElement("Matrix");
			//Str(xTransformationPoint->Attribute("x")).scanFmt("%lf", image.$posX);
			//Str(xTransformationPoint->Attribute("y")).scanFmt("%lf", image.$posY);
			auto & x_matrix = *x_dom_bitmap_instance.FirstChildElement("matrix")->FirstChildElement("Matrix");
			TinyXML::get_attribute(x_matrix, "a"_sv, sprite.scale.x);
			TinyXML::get_attribute(x_matrix, "d"_sv, sprite.scale.y);
			TinyXML::get_attribute(x_matrix, "tx"_sv, sprite.position.x);
			TinyXML::get_attribute(x_matrix, "ty"_sv, sprite.position.y);
			sprite.scale.x /= g_resource_scale_rate;
			sprite.scale.y /= g_resource_scale_rate;
			return;
		}

		inline auto load_x_frame_only_symbol_instance (
			const tinyxml2::XMLElement & x_frame,
			Size &                       index,
			Size &                       duration,
			String &                     library,
			ChangeMatrix &               matrix,
			ChangeColor &                color
		) -> None {
			duration = 1_size;
			matrix.init();
			color.init();
			TinyXML::get_attribute(x_frame, "index"_sv, index);
			TinyXML::get_attribute(x_frame, "duration"_sv, duration);
			auto & x_dom_symbol_instance = *x_frame.FirstChildElement("elements")->FirstChildElement("DOMSymbolInstance");
			TinyXML::get_attribute(x_dom_symbol_instance, "libraryItemName"_sv, library);
			if (x_dom_symbol_instance.FirstChildElement("matrix") != nullptr) {
				load_x_matrix_with_shell(*x_dom_symbol_instance.FirstChildElement("matrix"), matrix);
			}
			if (x_dom_symbol_instance.FirstChildElement("color") != nullptr) {
				load_x_color_with_shell(*x_dom_symbol_instance.FirstChildElement("color"), color);
			}
			return;
		}

		/*
		inline None load_animation_x_timeline(const tinyxml2::XMLElement & x_timeline, String & name, List<Information::Frame> & frame_list, Size & frame_sum) {
			TinyXML::get_attribute(x_timeline, "name"_sv, name);
			auto & x_layer_shell = *x_timeline.FirstChildElement("layers");
			List<const tinyxml2::XMLElement *> x_layer_list;
			auto layer_sum = k_none_size;
			frame_sum      = k_none_size;
			for (auto e = x_layer_shell.FirstChildElement("DOMLayer"); e != nullptr; e = e->NextSiblingElement()) {
				++layer_sum;
				auto x_frame = *e->FirstChildElement("frames")->LastChild("DOMFrame")->ToElement();
				Size this_frame_index;
				Size this_frame_duration;
				TinyXML::get_attribute(x_frame, "index"_sv, this_frame_index);
				TinyXML::get_attribute(x_frame, "duration"_sv, this_frame_duration);
				frame_sum = max(frame_sum, this_frame_index + this_frame_duration);
			}
			x_layer_list.alloc(layer_sum);
			for (auto e = x_layer_shell.FirstChildElement("DOMLayer"); e != nullptr; e = e->NextSiblingElement()) {
				x_layer_list.append(e);
			}
			frame_list.alloc_full(frame_sum);
			for (auto layer_index : SizeRange { layer_sum }) {
				auto & x_layer = x_layer_list[x_layer_list.last_index() - layer_index];
				Size frame_index_of_be_remove;
				auto x_frame_shell      = x_layer->FirstChildElement("frames");
				auto x_first_real_frame = x_frame_shell->FirstChildElement("DOMFrame");
				while (x_first_real_frame != nullptr) {
					if (!(x_first_real_frame->NoChildren() || x_first_real_frame->FirstChildElement("elements")->NoChildren())) {
						break;
					}
					x_first_real_frame = x_first_real_frame->NextSiblingElement();
				}
				if (x_first_real_frame == nullptr) {
					//log.war("this layer can't found valid-frame.");
				}
				else {
					auto first_frame_index = k_begin_index;
					{
						Size frame_index;
						String library;
						load_x_frame_only_symbol_instance(*x_first_real_frame, frame_index, as_lvalue(Size {}), library, as_lvalue(ChangeMatrix {}), as_lvalue(ChangeColor {}));
						auto & frame = frame_list[frame_index];
						if (!frame.append) {
							frame.append.create();
						}
						frame.append.get().append();
						auto & layer_append = frame.append.get().last();
						layer_append.index  = layer_index;
						if (library.first() == 'a') {
							layer_append.animation = k_true;
						}
						else if (library.first() == 'i') {
							layer_append.animation = k_false;
						}
						else {
							throw SimpleException { "PopcapAnimation-Convert-XFL-From : unknown XFL-Library type"_sv };
						}
						$Str::scanFmt(Str().ref(library, kNo2), "%u", &layer_append.reference);
						first_frame_index = frame_index;
					}
					for (auto xFrame = x_first_real_frame; xFrame != nullptr; xFrame = xFrame->NextSiblingElement()) {
						Size frame_index;
						Size frame_duration;
						ChangeMatrix matrix;
						ChangeColor color;
						load_x_frame_only_symbol_instance(*xFrame, frame_index, frame_duration, as_lvalue(String {}), matrix, color);
						frame_index_of_be_remove = frame_index + frame_duration;
						auto & frame             = frame_list[frame_index];
						if (!frame.change) {
							frame.change.create();
						}
						frame.change.get().append();
						auto & layer_change = frame.change.get().last();
						layer_change.index  = layer_index;
						layer_change.matrix.create(Information::LayerChangeType::Matrix {
							matrix.a, matrix.b, matrix.c, matrix.d
						});
						layer_change.position.create(Information::LayerChangeType::Position {
							{
								matrix.x, matrix.y
							}
						});
						layer_change.color.create(Information::LayerChangeType::Discolor {
							color.red, color.green, color.blue, color.alpha
						});
					}
					if (frame_index_of_be_remove < frame_sum) {
						auto & frame = frame_list[frame_index_of_be_remove];
						if (!frame.remove) {
							frame.remove.create();
						}
						frame.remove.get().append();
						auto & layer_remove  = frame.remove.get().last();
						layer_remove.index   = layer_index;
						layer_remove.unknown = 0;
					}
				}
			}
			return;
		}

		inline None analysis_fscommand_string(const String & string, List<Information::FSCommand> & command) {
			List<String> command_string_list {};
			split_list(string, make_static_array({ ';'_character }), command_string_list);
			command.alloc(command_string_list.size());
			for (auto & command_string : command_string_list) {
				if (Range::has_sub(command_string, "fscommand"_sv)) {
					command.append();
					auto & current_command = command.last();
					ICharacterStreamView command_string_stream { command_string };
					StringParser::read_string(command_string_stream, '('_character);
					StringParser::read_string(command_string_stream, '\"'_character);
					current_command.command = StringParser::read_string(command_string_stream, '\"'_character);
					StringParser::read_string(command_string_stream, ','_character);
					if (!command_string_stream.full()) {
						StringParser::read_string(command_string_stream, '\"'_character);
						current_command.argument.create(StringParser::read_string(command_string_stream, '\"'_character));
					}
					else {
						current_command.argument.clear();
					}
				}
			}
			return;
		}

		inline None load_other_layer(const tinyxml2::XMLElement & x_labels, const tinyxml2::XMLElement & x_actions, const tinyxml2::XMLElement & x_audios, List<Information::Frame> & frame_list) {
			for (auto e = x_labels.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != nullptr; e = e->NextSiblingElement()) {
				Size index;
				TinyXML::get_attribute(*e, "index"_sv, index);
				frame_list[index].label.create();
				TinyXML::get_attribute(*e, "name"_sv, frame_list[index].label.get());
			}
			for (auto e = x_actions.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != nullptr; e = e->NextSiblingElement()) {
				if (!e->NoChildren() && TinyXML::get_tag(*e->FirstChildElement()) == "Actionscript"_sv) {
					Size index;
					TinyXML::get_attribute(*e, "index"_sv, index);
					ConstantStringView all_command { make_string_view(e->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText()) };
					if (Range::has_sub(all_command, "stop();"_sv)) {
						frame_list[index].stop = k_true;
					}
					if (Range::has_sub(all_command, "fscommand"_sv)) {
						frame_list[index].command.create();
						analysis_fscommand_string(all_command, frame_list[index].command.get());
					}
				}
			}
			for (auto e = x_audios.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != nullptr; e = e->NextSiblingElement()) {
				if (!e->NoChildren() && TinyXML::get_tag(*e->FirstChildElement()) == "Actionscript"_sv) {
					Size index;
					TinyXML::get_attribute(*e, "index"_sv, index);
					ConstantStringView all_command { make_string_view(e->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText()) };
					frame_list[index].command.create();
					analysis_fscommand_string(all_command, frame_list[index].command.get());
				}
			}
			return;
		}

		inline None convert(Information::Package & information, const Path & xfl_directory, const Size & image_size) {
			Path directory_before_change { FileSystem::current_directory() };
			FileSystem::change_directory(xfl_directory);
			g_resource_scale_rate = cast_wrapper<FloatingNumber64>(k_stdandard_image_size) / cast_wrapper<FloatingNumber64>(image_size);
			ExtraInformation extra_information {};
			JSON::read("extra.json"_path, extra_information);
			information.unknown = extra_information.unknown;
			information.origin  = extra_information.origin;
			tinyxml2::XMLDocument main_document {};
			TinyXML::read("DOMDocument.xml"_path, main_document);
			Size frame_rate {};
			TinyXML::get_attribute(*main_document.RootElement(), "frameRate"_sv, frame_rate);
			List<String> sprite_name_list {};
			List<String> symbol_href_list {};
			for (auto e = main_document.RootElement()->FirstChildElement("media")->FirstChildElement("DOMBitmapItem"); e != nullptr; e = e->NextSiblingElement()) {
				sprite_name_list.append(TinyXML::get_attribute(*e, "name"_sv));
			}
			for (auto e = main_document.RootElement()->FirstChildElement("symbols")->FirstChildElement("Include"); e != nullptr; e = e->NextSiblingElement()) {
				symbol_href_list.append(TinyXML::get_attribute(*e, "href"_sv));
			}
			information.sprite.alloc_full(symbol_href_list.size() - 1_size);
			information.animation.alloc_full(symbol_href_list.size() - 1_size);
			Size sprite_count { k_none_size };
			Size animation_count { k_none_size };
			for (auto & symbol_href : symbol_href_list) {
				tinyxml2::XMLDocument x_document;
				TinyXML::read("LIBRARY"_path + symbol_href, x_document);
				if (symbol_href == "main.xml"_sv) {
					String name;
					Size frame_sum;
					load_animation_x_timeline(*x_document.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, information.main_animation.frame, frame_sum);
					information.main_animation.information.rate  = frame_rate;
					information.main_animation.information.count   = frame_sum;
					information.main_animation.information.begin = k_begin_index;
					information.main_animation.information.end   = frame_sum - k_next_index;
				}
				else if (symbol_href.first() == 'i') {
					Size i;
					$Str::scanFmt(Str().ref(symbol_href, k_next_index), "%u", &i);
					auto & t = information.sprite[i];
					String name;
					load_sprite_x_timeline(*x_document.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, t);
					auto const & id = extra_information.sprite_mapper[name];
					t.id            = id;
					t.size          = extra_information.sprite_size[id];
					++sprite_count;
				}
				else if (symbol_href.first() == 'a') {
					Size i;
					$Str::scanFmt(Str().ref(symbol_href, k_next_index), "%u", &i);
					auto & t = information.animation[i];
					String name;
					Size frameSum;
					load_animation_x_timeline(*x_document.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, t.$frame, frameSum);
					t.information.rate  = frame_rate;
					t.information.count   = frameSum;
					t.information.begin = k_begin_index;
					t.information.end   = frameSum - k_next_index;
					t.label             = extra_information.animation_mapper[name];
					++animation_count;
				}
				else {
					throw SimpleException{"PopcapAnimation-Convert-XFL-Form unknown XFL-Library type : %c"_sv};
				}
				x_document.Clear();
			}
			information.sprite.remove_tail(information.sprite.size() - sprite_count);
			information.animation.remove_tail(information.animation.size() - animation_count);
			{
				auto xLabelsLayer  = main_document.RootElement()->FirstChildElement("timelines")->FirstChildElement("DOMTimeline")->FirstChildElement("layers")->FirstChildElement("DOMLayer");
				auto xActionsLayer = xLabelsLayer->NextSiblingElement();
				auto xAudioLayer   = xActionsLayer->NextSiblingElement();
				load_other_layer(*xLabelsLayer, *xActionsLayer, *xAudioLayer, information.main_animation.frame);
			}
			main_document.Clear();
			FileSystem::change_directory(directory_before_change);
			information.main_animation.block1.v1.clean();
			information.main_animation.block1.v2.clean();
			return;
		}*/

		// ------------------------------------------------

	}

	// ------------------------------------------------

	namespace Scale {

		// ------------------------------------------------
		/*
		inline auto convert (
			const Path & xfl_directory,
			const Size & source_size,
			const Size & dest_size
		) -> None {
			const auto source_scale = cast_wrapper<FloatingNumber64>(k_stdandard_image_size) / cast_wrapper<FloatingNumber64>(source_size);
			const auto dest_scale   = cast_wrapper<FloatingNumber64>(k_stdandard_image_size) / cast_wrapper<FloatingNumber64>(dest_size);
			List<Path> file_list{FileSystem::list_file(xfl_directory + "LIBRARY"_sv)};
			for (auto & library_file : file_list) {
				if (library_file.compare_extension("xml"_sv) && library_file.name().first() == 'i'_character) {
					tinyxml2::XMLDocument x_document{};
					TinyXML::read(library_file, x_document);
					auto matrix = *x_document.RootElement()
											->FirstChildElement("timeline")->FirstChildElement("DOMTimeline")
											->FirstChildElement("layers")->FirstChildElement("DOMLayer")
											->FirstChildElement("frames")->FirstChildElement("DOMFrame")
											->FirstChildElement("elements")->FirstChildElement("DOMBitmapInstance")
											->FirstChildElement("matrix")->FirstChildElement("Matrix");
					FloatingNumber64 origin_a;
					FloatingNumber64 origin_d;
					TinyXML::get_attribute(matrix, "a"_sv, origin_a);
					TinyXML::get_attribute(matrix, "d"_sv, origin_d);
					TinyXML::set_attribute(matrix, "a"_sv, origin_a / source_scale * dest_scale);
					TinyXML::set_attribute(matrix, "d"_sv, origin_d / source_scale * dest_scale);
					TinyXML::write(library_file, *x_document.RootElement()->Clone());
				}
			}
			return;
		}*/

		// ------------------------------------------------

	}

	// ------------------------------------------------

}

// ------------------------------------------------
