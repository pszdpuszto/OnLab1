#include "entity.hpp"
#include "consts.hpp"
#include "game.hpp"

Entity::Entity(double x, double y, double w, double h, double speed) : Object(x, y, w, h), _speed(speed * Consts::GLOBAL_SPEED) {}

void Entity::doMove(double dx, double dy)
{
	double newX = getX() + dx * _speed;
	double newY = getY() + dy * _speed;

	// Boundary checks
	if (newX < 0) newX = 0;
	if (newY < 0) newY = 0;
	if (newX + _width > Consts::ROOM_WIDTH) newX = Consts::ROOM_WIDTH - _width;
	if (newY + _height > Consts::ROOM_HEIGHT) newY = Consts::ROOM_HEIGHT - _height;

	for (Object* obj : Game::getInstance()->getCurrentLevel()->getCurrentRoom()->getObjects()) {
		if (this->isCollidingWith(*obj)) {
			return; // Collision detected, do not move
		}
	}
	_x = newX;
	_y = newY;
}


