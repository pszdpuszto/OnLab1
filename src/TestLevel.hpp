#pragma once

#include "level.hpp"
#include "room.hpp"

class TestLevel : public Level {
private:
	virtual Room* generateRooms() override final;
public:
	TestLevel() {
		_currentRoom = generateRooms();
	}
};