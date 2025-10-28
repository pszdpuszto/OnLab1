#pragma once

#include "level.hpp"
#include "room.hpp"

class TestLevel : public Level {
public:
	TestLevel() {
		_currentRoom = generateRooms();
	}
private:
	virtual Room* generateRooms() override final;
};