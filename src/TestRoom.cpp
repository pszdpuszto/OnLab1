#include "TestRoom.hpp"

TestRoom::TestRoom() : Room()
{
	_objects.push_back(new Object(50, 50, 10, 10));
	_objects.push_back(new Object(100, 20, 20, 20));
}