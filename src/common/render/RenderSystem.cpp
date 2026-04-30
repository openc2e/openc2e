#include "RenderSystem.h"

#include "common/NumericCast.h"
#include "common/backend/Backend.h"
#include "common/math/ModularArithmetic.h"

RenderSystem::RenderSystem() {
}

void RenderSystem::draw(const DrawConfig& config) {
	config.renderer->renderClear();
	config.renderer->setClip(config.screen_dest);

	std::vector<RenderItem*> render_list;
	for (auto& r : m_render_items) {
		// TODO: optimize with separate path for non-modular arithmetic?
		if (r.dest.intersects_modx(config.world_src, numeric_cast<float>(config.world_wrap_width))) {
			render_list.push_back(&r);
		}
	}
	std::sort(render_list.begin(), render_list.end(), [](auto* left, auto* right) {
		// TODO: speed up by caching composite sort key on renderitem, secondly
		// by including it in this vector to make sort faster
		if (left->layer != right->layer) {
			return left->layer < right->layer;
		}
		if (left->z != right->z) {
			return left->z < right->z;
		}
		if (left->tex != right->tex) {
			return left->tex < right->tex;
		}
		if (left->type != right->type) {
			return left->type < right->type;
		}
		return left < right;
	});

	// What's the transformation for this camera? First, let's transform source space to dest space

	// destx = srcx * (config.screen_dest.width / config.world_src.width) + config.screen_dest.x
	// desty = srcy * (config.screen_dest.height / config.world_src.height) + config.screen_dest.y

	// Now, let's transfom world space to source space
	// srcx = worldx + config.world_src.x
	// srcy = worldy + config.world_src.y

	// Putting it together, we have
	// destx = (worldx + config.world_src.x) * (config.screen_dest.width / config.world_src.width) + config.screen_dest.x
	//       = worldx * (config.screen_dest.width / config.world_src.width) + config.world_src.x * (config.screen_dest.width / config.world_src.width) + config.screen_dest.x
	//       = worldx * xscale + xadjust

	const float xscale = config.screen_dest.width / numeric_cast<float>(config.world_src.width);
	const float xadjust = numeric_cast<float>(mod_remainder(config.world_src.x, config.world_wrap_width)) * xscale - config.screen_dest.x;
	const float yscale = config.screen_dest.height / numeric_cast<float>(config.world_src.height);
	const float yadjust = numeric_cast<float>(config.world_src.y) * yscale - config.screen_dest.y;

	const float worldadjust = numeric_cast<float>(config.world_wrap_width) * xscale;

	for (auto* r : render_list) {
		float x = mod_remainder(r->dest.x, numeric_cast<float>(config.world_wrap_width)) * xscale - xadjust;
		float y = r->dest.y * yscale - yadjust;
		float w = r->dest.width * xscale;
		float h = r->dest.height * yscale;

		if (r->type == RenderItem::RENDER_TEXTURE) {
			Rect2f dest;
			dest.x = x;
			dest.y = y;
			dest.width = w;
			dest.height = h;

			// handle wraparound by just drawing multiple times, SDLBackend will cull from here
			config.renderer->renderTexture(r->tex, r->src, dest);
			config.renderer->renderTexture(r->tex, r->src, dest.move(-worldadjust, 0));
			config.renderer->renderTexture(r->tex, r->src, dest.move(worldadjust, 0));
		} else if (r->type == RenderItem::RENDER_RECT) {
			const auto renderRect = [&](auto a) {
				const float x1 = x + a;
				const float x2 = x + w + a;
				const float y1 = y;
				const float y2 = y + h;
				// top
				config.renderer->renderLine(x1, y1, x2, y1, r->color);
				// right
				config.renderer->renderLine(x2, y1, x2, y2, r->color);
				// bottom
				config.renderer->renderLine(x1, y2, x2, y2, r->color);
				// left
				config.renderer->renderLine(x1, y1, x1, y2, r->color);
			};
			// handle wraparound by just drawing multiple times, SDLBackend will cull from here
			renderRect(0.f);
			renderRect(worldadjust);
			renderRect(-worldadjust);
		} else if (r->type == RenderItem::RENDER_LINE) {
			const auto renderLine = [&](auto a) {
				config.renderer->renderLine(x + a, y, x + a + w, y + h, r->color);
			};
			// handle wraparound by just drawing multiple times, SDLBackend will cull from here
			renderLine(0.f);
			renderLine(worldadjust);
			renderLine(-worldadjust);
		}
	}
}

void RenderSystem::add(const RenderItem& item) {
	m_render_items.push_back(item);
}

void RenderSystem::add(const RenderItem& item_, LayerIndex layer) {
	RenderItem item{item_};
	item.layer = layer;
	m_render_items.push_back(item);
}

void RenderSystem::clear() {
	m_render_items.clear();
}