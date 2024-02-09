#include "RenderSystem.h"

#include "common/NumericCast.h"
#include "common/backend/Backend.h"

struct RenderSystemConstructTag {};

RenderSystem* get_rendersystem() {
	static RenderSystem s_render_system{RenderSystemConstructTag{}};
	return &s_render_system;
}

RenderSystem::RenderSystem(const RenderSystemConstructTag&) {
}

void RenderSystem::main_camera_set_src_rect(Rect2i src) {
	if (m_world_wrap_width) {
		if (src.x < 0) {
			src.x += m_world_wrap_width;
		} else if (src.x >= m_world_wrap_width) {
			src.x -= m_world_wrap_width;
		}
	}
	m_main_camera_src_rect = src;
}

const Rect2i& RenderSystem::main_camera_get_src_rect() const {
	return m_main_camera_src_rect;
}

void RenderSystem::main_viewport_set_dest_rect(Rect2f src) {
	m_main_viewport_dest_rect = src;
}

const Rect2f& RenderSystem::main_viewport_get_dest_rect() const {
	return m_main_viewport_dest_rect;
}

void RenderSystem::world_set_wrap_width(int32_t wrap_width) {
	m_world_wrap_width = wrap_width;
}

int32_t RenderSystem::world_get_wrap_width() const {
	return m_world_wrap_width;
}

RenderItemHandle RenderSystem::render_item_create(int layer) {
	RenderItem item;
	item.layer = layer;

	RenderItemHandle handle;
	handle.key = m_render_items.add(item);
	handle.parent = this;

	return handle;
}

void RenderSystem::render_item_set_position(const RenderItemHandle& handle, float x, float y, int32_t z) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->dest.x = x;
		item->dest.y = y;
		item->z = z;
	}
}

void RenderSystem::render_item_set_texture(const RenderItemHandle& handle, const Texture& tex, Rect2i location) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_TEXTURE;
		item->tex = tex;
		item->src = location;
		item->dest.width = location.width;
		item->dest.height = location.height;
	}
}

void RenderSystem::render_item_set_unfilled_rect(const RenderItemHandle& handle, float x, float y, float w, float h, Color color) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_RECT;
		item->dest.x = x;
		item->dest.y = y;
		item->dest.width = w;
		item->dest.height = h;
		item->color = color;
	}
}

void RenderSystem::render_item_set_line(const RenderItemHandle& handle, float xstart, float ystart, float xend, float yend, Color color) {
	if (RenderItem* item = m_render_items.try_get(handle.key)) {
		item->type = RENDER_LINE;
		item->dest.x = xstart;
		item->dest.y = ystart;
		item->dest.width = xend - xstart;
		item->dest.height = yend - ystart;
		item->color = color;
	}
}

void RenderSystem::draw() {
	auto renderer = get_backend()->getMainRenderTarget();
	renderer->renderClear();
	renderer->setClip(m_main_viewport_dest_rect);

	std::vector<RenderItem*> render_list;
	render_list.reserve(m_render_items.size());
	for (auto& r : m_render_items) {
		// TODO: speed up by culling non-visible items, so we don't need to
		// sort them and we get a higher likelihood of batching textures. remember
		// to cull taking world wrap into account
		render_list.push_back(&r);
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

	// destx = srcx * (m_main_viewport_dest_rect.width / m_main_camera_src_rect.width) + m_main_viewport_dest_rect.x
	// desty = srcy * (m_main_viewport_dest_rect.height / m_main_camera_src_rect.height) + m_main_viewport_dest_rect.y

	// Now, let's transfom world space to source space
	// srcx = worldx + m_main_camera_src_rect.x
	// srcy = worldy + m_main_camera_src_rect.y

	// Putting it together, we have
	// destx = (worldx + m_main_camera_src_rect.x) * (m_main_viewport_dest_rect.width / m_main_camera_src_rect.width) + m_main_viewport_dest_rect.x
	//       = worldx * (m_main_viewport_dest_rect.width / m_main_camera_src_rect.width) + m_main_camera_src_rect.x * (m_main_viewport_dest_rect.width / m_main_camera_src_rect.width) + m_main_viewport_dest_rect.x
	//       = worldx * xscale + xadjust


	const float xscale = 1.f * m_main_viewport_dest_rect.width / m_main_camera_src_rect.width;
	const float xadjust = m_main_camera_src_rect.x * xscale - m_main_viewport_dest_rect.x;
	const float yscale = 1.f * m_main_viewport_dest_rect.height / m_main_camera_src_rect.height;
	const float yadjust = m_main_camera_src_rect.y * yscale - m_main_viewport_dest_rect.y;

	const float worldadjust = m_world_wrap_width * xscale;

	for (auto* r : render_list) {
		float x = r->dest.x * xscale - xadjust;
		float y = r->dest.y * yscale - yadjust;
		float w = r->dest.width * xscale;
		float h = r->dest.height * yscale;

		if (r->type == RENDER_TEXTURE) {
			Rect2f dest;
			dest.x = x;
			dest.y = y;
			dest.width = w;
			dest.height = h;

			renderer->renderTexture(r->tex, r->src, dest);
			if (m_world_wrap_width) {
				// eh, handle wraparound by just drawing multiple times, SDLBackend will cull from here
				dest.x += worldadjust;
				renderer->renderTexture(r->tex, r->src, dest);
				dest.x -= worldadjust * 2;
				renderer->renderTexture(r->tex, r->src, dest);
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