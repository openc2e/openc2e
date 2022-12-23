#include "RenderSystem.h"

#include "common/NumericCast.h"


RenderSystem::RenderSystem(Backend* backend)
	: m_backend(backend) {
}

void RenderSystem::RenderSystem::set_layer_position(int layer, int32_t x, int32_t y) {
	auto& ref = m_render_layers[layer];
	ref.x = x;
	ref.y = y;
}

void RenderSystem::set_layer_wrap_width(int layer, int32_t wrap_width) {
	m_render_layers[layer].wrap_width = wrap_width;
}

RenderItemHandle RenderSystem::create_render_item(int layer) {
	RenderItem item;
	item.layer = layer;

	RenderItemHandle handle;
	handle.key = m_render_items.add(item);
	handle.parent = this;

	return handle;
}

void RenderSystem::set_render_item_position(const RenderItemHandle& handle, int32_t x, int32_t y, int32_t z) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->x = x;
		item->y = y;
		item->z = z;
	}
}

void RenderSystem::set_render_item_texture(const RenderItemHandle& handle, const Texture& tex) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_TEXTURE;
		item->tex = tex;
		item->width = numeric_cast<int32_t>(tex.width);
		item->height = numeric_cast<int32_t>(tex.height);
	}
}

void RenderSystem::set_render_item_unfilled_rect(const RenderItemHandle& handle, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_RECT;
		item->x = x;
		item->y = y;
		item->width = w;
		item->height = h;
		item->color = color;
	}
}

void RenderSystem::draw() {
	auto renderer = m_backend->getMainRenderTarget();

	std::vector<RenderItem*> render_list;
	render_list.reserve(m_render_items.size());
	for (auto& r : m_render_items) {
		// TODO: speed up by culling non-visible items, so we don't need to
		// sort them. won't affect result as SDLBackend already culls draw
		// calls
		render_list.push_back(&r);
	}
	std::sort(render_list.begin(), render_list.end(), [](auto* left, auto* right) {
		// TODO: speed up by caching composite sort key on renderitem, secondly
		// by including it in this vector to make sort faster
		// TODO: speed up rendering later by putting items with the same texture
		// next to each other
		if (left->layer != right->layer) {
			return left->layer < right->layer;
		}
		if (left->z != right->z) {
			return left->z < right->z;
		}
		return left < right;
	});

	for (auto* r : render_list) {
		// TODO: optimize by only searching for layer when layer index changes
		const RenderLayer& current_layer = m_render_layers[r->layer];
		int32_t x = r->x - current_layer.x;
		int32_t y = r->y - current_layer.y;

		if (r->type == RENDER_TEXTURE) {
			Rect src;
			src.x = 0;
			src.y = 0;
			src.width = numeric_cast<int32_t>(r->tex.width);
			src.height = numeric_cast<int32_t>(r->tex.height);

			Rect dest;
			dest.x = x;
			dest.y = y;
			dest.width = src.width;
			dest.height = src.height;

			renderer->renderTexture(r->tex, src, dest);
			if (current_layer.wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				dest.x += current_layer.wrap_width;
				renderer->renderTexture(r->tex, src, dest);
				dest.x -= current_layer.wrap_width * 2;
				renderer->renderTexture(r->tex, src, dest);
			}

		} else if (r->type == RENDER_RECT) {
			const auto renderRect = [&](int xadjust) {
				const int x1 = x + xadjust;
				const int x2 = x + r->width + xadjust;
				const int y1 = y;
				const int y2 = y + r->height;
				// top
				renderer->renderLine(x1, y1, x2, y1, r->color);
				// right
				renderer->renderLine(x2, y1, x2, y2, r->color);
				// bottom
				renderer->renderLine(x1, y2, x2, y2, r->color);
				// left
				renderer->renderLine(x1, y1, x1, y2, r->color);
			};
			renderRect(0);
			if (current_layer.wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderRect(current_layer.wrap_width);
				renderRect(-current_layer.wrap_width);
			}
		}
	}
}