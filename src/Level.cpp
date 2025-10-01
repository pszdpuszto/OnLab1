#include "level.hpp"

Level::Level()
{
}
Level::~Level()
{
	for (Room* r : _rooms) {
		delete r;
	}
}