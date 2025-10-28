#include "../header/TestRoom.hpp"

#include "../header/Obstacles.hpp"

TestRoom::TestRoom() : Room()
{
	_objects.push_back(new Rock(50, 50));
	_objects.push_back(new Barrel(100, 20));
}