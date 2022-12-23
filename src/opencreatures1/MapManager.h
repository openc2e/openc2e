#pragma once

#include "common/creaturesImage.h"

class Room {
  public:
	int left;
	int top;
	int right;
	int bottom;
	int type;

	int width() const {
		return right - left;
	}
	int height() const {
		return bottom - top;
	}
};

class MapManager {
  public:
	creaturesImage background;
	std::vector<Room> rooms;
};