#include "RenderSystem.h"

#include "common/NumericCast.h"


RenderSystem::RenderSystem(Backend* backend)
	: m_backend(backend) {
}

void RenderSystem::main_camera_set_position(int32_t x, int32_t y) {
	if (m_world_wrap_width) {
		if (x < 0) {
			x += m_world_wrap_width;
		} else if (x >= m_world_wrap_width) {
			x -= m_world_wrap_width;
		}
	}
	m_main_camera_x = x;
	m_main_camera_y = y;
}

void RenderSystem::world_set_wrap_width(int32_t wrap_width) {
	m_world_wrap_width = wrap_width;
}

RenderItemHandle RenderSystem::render_item_create(int layer) {
	RenderItem item;
	item.layer = layer;

	RenderItemHandle handle;
	handle.key = m_render_items.add(item);
	handle.parent = this;

	return handle;
}

void RenderSystem::render_item_set_position(const RenderItemHandle& handle, int32_t x, int32_t y, int32_t z) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->x = x;
		item->y = y;
		item->z = z;
	}
}

void RenderSystem::render_item_set_texture(const RenderItemHandle& handle, const Texture& tex) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_TEXTURE;
		item->tex = tex;
		item->width = numeric_cast<int32_t>(tex.width);
		item->height = numeric_cast<int32_t>(tex.height);
	}
}

void RenderSystem::render_item_set_unfilled_rect(const RenderItemHandle& handle, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_RECT;
		item->x = x;
		item->y = y;
		item->width = w;
		item->height = h;
		item->color = color;
	}
}

void RenderSystem::render_item_set_line(const RenderItemHandle& handle, int32_t xstart, int32_t ystart, int32_t xend, int32_t yend, uint32_t color) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_LINE;
		item->x = xstart;
		item->y = ystart;
		item->width = xend - xstart;
		item->height = yend - ystart;
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
		int32_t x = r->x - m_main_camera_x;
		int32_t y = r->y - m_main_camera_y;

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
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				dest.x += m_world_wrap_width;
				renderer->renderTexture(r->tex, src, dest);
				dest.x -= m_world_wrap_width * 2;
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
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderRect(m_world_wrap_width);
				renderRect(-m_world_wrap_width);
			}
		} else if (r->type == RENDER_LINE) {
			renderer->renderLine(x, y, x + r->width, y + r->height, r->color);
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderer->renderLine(x + m_world_wrap_width, y, x + m_world_wrap_width + r->width, y + r->height, r->color);
				renderer->renderLine(x - m_world_wrap_width, y, x - m_world_wrap_width + r->width, y + r->height, r->color);
			}
		}
	}
}