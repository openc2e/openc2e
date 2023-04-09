#pragma once

#include "common/render/RenderItemHandle.h"
#include "opencreatures1/ImageGallery.h"

#include <array>
#include <vector>

class Room {
  public:
	int left;
	int top;
	int right;
	int bottom;
	int type;

	RenderItemHandle renderitem;

	int width() const {
		return right - left;
	}
	int height() const {
		return bottom - top;
	}
};

class MapManager {
  public:
	void add_room(Room&&);
	const auto& get_rooms() const { return rooms; }

	void set_background(ImageGallery background);
	const ImageGallery& get_background() const { return background; }

	void set_groundlevel(const std::array<uint32_t, 261>&);
	const auto& get_groundlevel() const { return groundlevel; }

  private:
	std::vector<Room> rooms;
	ImageGallery background;
	RenderItemHandle background_renderitem;
	std::array<uint32_t, 261> groundlevel;
	std::array<RenderItemHandle, 261> groundlevel_renderitems;
};