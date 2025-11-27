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
	void moveToRoom(Room* room) {
		_currentRoom = room;
	}

protected:
	std::vector<Room*> _rooms = std::vector<Room*>();
	Room* _currentRoom;
	
	virtual Room* generateRooms() = 0;
};