#pragma once

#include "opencreatures1/ImageGallery.h"

#include <array>
#include <vector>

class Room {
  public:
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
	int32_t type;

	int32_t width() const {
		return right - left;
	}
	int32_t height() const {
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

	int32_t get_world_wrap_width() const;

	void render(class RenderSystem&) const;

  private:
	std::vector<Room> rooms;
	ImageGallery background;
	std::array<uint32_t, 261> groundlevel;
};