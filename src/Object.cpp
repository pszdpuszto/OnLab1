
#include "object.hpp"
#include "utils.hpp"
#include "game.hpp"

Object::Object(double x, double y, double w, double h)
{
	_x = x;
	_y = y;
	_width = w;
	_height = h;
}

double Object::getX() const
{
	return _x;
}

double Object::getY() const
{
	return _y;
}

void Object::render() const
{
	Game::getInstance()->renderObject(_x, _y, _width, _height, 0, 0, 0);
}

bool Object::isCollidingWith(const Object& other) const
{
	return Utils::isColliding(_x, _y, _width, _height,
		other._x, other._y, other._width, other._height);
}
