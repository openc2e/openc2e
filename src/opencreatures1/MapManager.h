#pragma once

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
	std::vector<Room> rooms;
};