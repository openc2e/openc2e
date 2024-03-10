#include "Blackboard.h"

#include "ObjectManager.h"
#include "common/Ascii.h"

static bool world_has_at_least_one_creature() {
	for (auto* obj : *g_engine_context.objects) {
		if (obj->as_creature()) {
			return true;
		}
	}
	return false;
}

void Blackboard::blackboard_show_word(int32_t word_index) {
	if (word_index < 0 || numeric_cast<size_t>(word_index) >= words.size()) {
		throw Exception(fmt::format("Blackboard word index {} out of bounds", word_index));
	}
	const auto& word = words[numeric_cast<size_t>(word_index)];

	int32_t x = get_bbox().x + text_x_position;
	const int32_t y = get_bbox().y + text_y_position;
	// TODO: What should the actual z-order be? Should this be in a SortingGroup with the
	// main blackboard part?
	const int32_t z = get_z_order() + 1;

	blackboard_hide_word();
	for (size_t i = 0; i < word.text.size(); ++i) {
		const unsigned int frame = static_cast<unsigned char>(to_ascii_uppercase(word.text[i]));

		auto renderitem = get_rendersystem()->render_item_create(LAYER_OBJECTS);
		get_rendersystem()->render_item_set_texture(renderitem,
			charset_sprite.texture,
			charset_sprite.texture_locations[frame]);
		get_rendersystem()->render_item_set_position(renderitem, x, y, z);
		text_render_items[i] = std::move(renderitem);

		x += charset_sprite.width(numeric_cast<int32_t>(frame)) + 1;
	}
}

void Blackboard::blackboard_hide_word() {
	text_render_items = {};
}

void Blackboard::blackboard_enable_edit() {
	printf("WARNING: blackboard_enable_edit not implemented\n");
}

void Blackboard::blackboard_disable_edit() {
	// TODO: no-op until we implement blackboard_enable_edit
}

void Blackboard::blackboard_emit_eyesight(int32_t word_index) {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: blackboard_emit_eyesight %i not implemented\n", word_index);
	}
}

void Blackboard::blackboard_emit_earshot(int32_t word_index) {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: blackboard_emit_earshot %i not implemented\n", word_index);
	}
}