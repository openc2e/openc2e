#include "RenderSystem.h"

#include "common/NumericCast.h"


RenderSystem::RenderSystem(Backend* backend)
	: m_backend(backend) {
}

void RenderSystem::main_camera_set_src_rect(Rect src) {
	if (m_world_wrap_width) {
		if (src.x < 0) {
			src.x += m_world_wrap_width;
		} else if (src.x >= m_world_wrap_width) {
			src.x -= m_world_wrap_width;
		}
	}
	m_main_camera_src_rect = src;
}

void RenderSystem::main_camera_set_dest_rect(Rect src) {
	m_main_camera_dest_rect = src;
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
	renderer->renderClear();

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

	// What's the transformation for this camera? First, let's transform source space to dest space

	// destx = srcx * (m_main_camera_dest_rect.width / m_main_camera_source_rect.width) + m_main_camera_dest_rect.x
	// desty = srcy * (m_main_camera_dest_rect.height / m_main_camera_source_rect.height) + m_main_camera_dest_rect.y

	// Now, let's transfom world space to source space
	// srcx = worldx + m_main_camera_src_rect.x
	// srcy = worldy + m_main_camera_src_rect.y

	// Putting it together, we have
	// destx = (worldx + m_main_camera_src_rect.x) * (m_main_camera_dest_rect.width / m_main_camera_source_rect.width) + m_main_camera_dest_rect.x
	//       = worldx * (m_main_camera_dest_rect.width / m_main_camera_source_rect.width) + m_main_camera_src_rect.x * (m_main_camera_dest_rect.width / m_main_camera_source_rect.width) + m_main_camera_dest_rect.x
	//       = worldx * xscale + xadjust


	const float xscale = 1.f * m_main_camera_dest_rect.width / m_main_camera_src_rect.width;
	const float xadjust = m_main_camera_src_rect.x * xscale - m_main_camera_dest_rect.x;
	const float yscale = 1.f * m_main_camera_dest_rect.height / m_main_camera_src_rect.height;
	const float yadjust = m_main_camera_src_rect.y * yscale - m_main_camera_dest_rect.y;

	const float worldadjust = m_world_wrap_width * xscale;

	for (auto* r : render_list) {
		float x = r->x * xscale - xadjust;
		float y = r->y * yscale - yadjust;
		float w = r->width * xscale;
		float h = r->height * yscale;

		if (r->type == RENDER_TEXTURE) {
			Rect src;
			src.x = 0;
			src.y = 0;
			src.width = numeric_cast<int32_t>(r->tex.width);
			src.height = numeric_cast<int32_t>(r->tex.height);

			RectF dest;
			dest.x = x;
			dest.y = y;
			dest.width = w;
			dest.height = h;

			renderer->renderTexture(r->tex, src, dest);
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				dest.x += worldadjust;
				renderer->renderTexture(r->tex, src, dest);
				dest.x -= worldadjust * 2;
				renderer->renderTexture(r->tex, src, dest);
			}

		} else if (r->type == RENDER_RECT) {
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
			renderRect(0);
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderRect(worldadjust);
				renderRect(-worldadjust);
			}
		} else if (r->type == RENDER_LINE) {
			renderer->renderLine(x, y, x + w, y + h, r->color);
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				renderer->renderLine(x + worldadjust, y, x + worldadjust + w, y + h, r->color);
				renderer->renderLine(x - worldadjust, y, x - worldadjust + w, y + h, r->color);
			}
		}
	}
}