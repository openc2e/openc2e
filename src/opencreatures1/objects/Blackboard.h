#pragma once

#include "CompoundObject.h"
#include "ImageGallery.h"
#include "common/render/RenderItemHandle.h"

#include <array>
#include <stdint.h>
#include <string>

struct Blackboard : CompoundObject {
	struct BlackboardWord {
		uint32_t value = 0;
		std::string text; // TODO: max 10 letters long
	};

	// SFC data
	uint8_t background_color = 0;
	uint8_t chalk_color = 0;
	uint8_t alias_color = 0;
	int8_t text_x_position = 0;
	int8_t text_y_position = 0;
	std::array<BlackboardWord, 16> words;

	// engine data
	ImageGallery charset_sprite;
	std::array<RenderItemHandle, 11> text_render_items;

	void blackboard_show_word(int32_t word_index);
	void blackboard_hide_word();
	void blackboard_enable_edit();
	void blackboard_disable_edit();
	void blackboard_emit_eyesight(int32_t word_index);
	void blackboard_emit_earshot(int32_t word_index);
};