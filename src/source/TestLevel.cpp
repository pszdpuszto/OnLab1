#include "../header/TestLevel.hpp"

#include "../header/TestRoom.hpp"

Room* TestLevel::generateRooms()
{
	Room* firstRoom = new TestRoom();
	Room* northRoom = new TestRoom();
	Room* eastRoom = new TestRoom();
	Room* southRoom = new TestRoom();
	Room* westRoom = new TestRoom();

	firstRoom->initDoors({
		{ Door::NORTH, northRoom },
		{ Door::EAST, eastRoom },
		{ Door::SOUTH, southRoom },
		{ Door::WEST, westRoom }
	});

	northRoom->initDoors({
		{ Door::SOUTH, firstRoom }
	});
	eastRoom->initDoors({
		{ Door::WEST, firstRoom }
	});
	southRoom->initDoors({
		{ Door::NORTH, firstRoom }
	});
	westRoom->initDoors({
		{ Door::EAST, firstRoom }
	});

	_rooms.push_back(firstRoom);
	_rooms.push_back(northRoom);
	_rooms.push_back(eastRoom);
	_rooms.push_back(southRoom);
	_rooms.push_back(westRoom);

	return firstRoom;
}