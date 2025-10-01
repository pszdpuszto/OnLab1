#pragma once

#include <vector>

#include "room.hpp"

class Level {
protected:
	std::vector<Room*> _rooms = std::vector<Room*>();
	Room* _currentRoom;
	
	virtual Room* generateRooms() = 0;
public:
	Level();
	~Level();

	Room* getCurrentRoom() const {
		return _currentRoom;
	}
};