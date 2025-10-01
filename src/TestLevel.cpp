#include "testLevel.hpp"
#include "TestRoom.hpp"

Room* TestLevel::generateRooms()
{
	Room* firstRoom = new TestRoom();
	_rooms.push_back(firstRoom);
	return firstRoom;
}