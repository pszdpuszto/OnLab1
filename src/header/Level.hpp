#pragma once

#include <vector>

#include "room.hpp"

class Level {
public:
	Level();
	~Level();

	Room* getCurrentRoom() const {
		return _currentRoom;
	}

protected:
	std::vector<Room*> _rooms = std::vector<Room*>();
	Room* _currentRoom;
	
	virtual Room* generateRooms() = 0;
};