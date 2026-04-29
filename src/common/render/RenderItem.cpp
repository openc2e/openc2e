#include "RenderItem.h"

RenderItem create_render_item_texture(Texture texture, Rect2i src, float x, float y, int32_t z) {
	RenderItem item;
	item.type = RenderItem::RENDER_TEXTURE;
	item.tex = texture;
	item.src = src;
	item.dest.width = numeric_cast<float>(item.src.width);
	item.dest.height = numeric_cast<float>(item.src.height);
	item.dest.x = x;
	item.dest.y = y;
	item.z = z;
	return item;
}

RenderItem create_render_item_rect(Rect2f dest, Color color) {
	RenderItem item;
	item.type = RenderItem::RENDER_RECT;
	item.dest = dest;
	item.color = color;
	return item;
}

RenderItem create_render_item_line(float xstart, float xend, float ystart, float yend, Color color) {
	RenderItem item;
	item.type = RenderItem::RENDER_LINE;
	item.dest.x = xstart;
	item.dest.y = ystart;
	item.dest.width = xend - xstart;
	item.dest.height = yend - ystart;
	item.color = color;
	return item;
}