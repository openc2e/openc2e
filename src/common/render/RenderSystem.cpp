#include "RenderSystem.h"

#include "common/NumericCast.h"
#include "common/backend/Backend.h"

RenderSystem::RenderSystem() {
}

void RenderSystem::draw(const DrawConfig& config_) {
	DrawConfig config = config_;
	if (config.world_wrap_width) {
		config.world_src.x %= config.world_wrap_width;
	}

	auto renderer = get_backend()->getMainRenderTarget();
	renderer->renderClear();
	renderer->setClip(config.screen_dest);

	std::vector<RenderItem*> render_list;
	render_list.reserve(m_render_items.size());
	for (auto& r : m_render_items) {
		// TODO: speed up by culling non-visible items, so we don't need to
		// sort them and we get a higher likelihood of batching textures. remember
		// to cull taking world wrap into account

		if (
			r.dest.intersects(config.world_src) ||
			r.dest.move(numeric_cast<float>(config.world_wrap_width), 0).intersects(config.world_src) ||
			r.dest.move(numeric_cast<float>(-config.world_wrap_width), 0).intersects(config.world_src)) {
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
	const float xadjust = numeric_cast<float>(config.world_src.x) * xscale - config.screen_dest.x;
	const float yscale = config.screen_dest.height / numeric_cast<float>(config.world_src.height);
	const float yadjust = numeric_cast<float>(config.world_src.y) * yscale - config.screen_dest.y;

	const float worldadjust = numeric_cast<float>(config.world_wrap_width) * xscale;

	for (auto* r : render_list) {
		float x = r->dest.x * xscale - xadjust;
		float y = r->dest.y * yscale - yadjust;
		float w = r->dest.width * xscale;
		float h = r->dest.height * yscale;

		if (r->type == RenderItem::RENDER_TEXTURE) {
			Rect2f dest;
			dest.x = x;
			dest.y = y;
			dest.width = w;
			dest.height = h;

			renderer->renderTexture(r->tex, r->src, dest);
			if (config.world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				dest.x += worldadjust;
				renderer->renderTexture(r->tex, r->src, dest);
				dest.x -= worldadjust * 2;
				renderer->renderTexture(r->tex, r->src, dest);
			}

		} else if (r->type == RenderItem::RENDER_RECT) {
			const auto renderRect = [&](auto a) {
				const float x1 = x + a;
				const float x2 = x + w + a;
				const float y1 = y;
				const float y2 = y + h;
				// top
				renderer->renderLine(x1, y1, x2, y1, r->color);
				// right
				renderer->renderLine(x2, y1, x2, y2, r->color);
				// bottom
				renderer->renderLine(x1, y2, x2, y2, r->color);
				// left
				renderer->renderLine(x1, y1, x1, y2, r->color);
			};
			renderRect(0.f);
			if (config.world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderRect(worldadjust);
				renderRect(-worldadjust);
			}
		} else if (r->type == RenderItem::RENDER_LINE) {
			renderer->renderLine(x, y, x + w, y + h, r->color);
			if (config.world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderer->renderLine(x + worldadjust, y, x + worldadjust + w, y + h, r->color);
				renderer->renderLine(x - worldadjust, y, x - worldadjust + w, y + h, r->color);
			}
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