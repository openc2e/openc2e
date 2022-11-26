#pragma once

#include "common/creaturesImage.h"

class Room {
  public:
	int left;
	int top;
	int right;
	int bottom;
	int type;
};

class MapManager {
  public:
	creaturesImage background;
	std::vector<Room> rooms;
};