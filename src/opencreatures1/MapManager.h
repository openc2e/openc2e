#pragma once

#include "common/creaturesImage.h"
#include "common/render/RenderItemHandle.h"

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
	void set_background(creaturesImage background);

  private:
	std::vector<Room> rooms;
	creaturesImage background;
	RenderItemHandle background_renderitem;
};