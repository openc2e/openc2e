#include "Blackboard.h"

#include "ImageManager.h"
#include "ObjectManager.h"
#include "SFCSerialization.h"
#include "common/Ascii.h"
#include "common/render/RenderSystem.h"
#include "fileformats/sfc/Blackboard.h"

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

	float x = get_bbox().x + text_x_position;
	const float y = get_bbox().y + text_y_position;
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

void Blackboard::serialize(SFCContext& ctx, sfc::BlackboardV1* bbd) {
	if (ctx.is_storing()) {
		bbd->background_color = background_color;
		bbd->chalk_color = chalk_color;
		bbd->alias_color = alias_color;
		bbd->text_x_position = text_x_position;
		bbd->text_y_position = text_y_position;
		for (size_t i = 0; i < bbd->words.size(); ++i) {
			bbd->words[i].value = words[i].value;
			bbd->words[i].text = words[i].text;
		}
	} else {
		background_color = bbd->background_color;
		chalk_color = bbd->chalk_color;
		alias_color = bbd->alias_color;
		text_x_position = bbd->text_x_position;
		text_y_position = bbd->text_y_position;
		for (size_t i = 0; i < bbd->words.size(); ++i) {
			auto& word = bbd->words[i];
			words[i].value = word.value;
			words[i].text = word.text;
		}
		charset_sprite = g_engine_context.images->get_charset_dta(bbd->background_color, bbd->chalk_color, bbd->alias_color);
	}
	CompoundObject::serialize(ctx, bbd);
}