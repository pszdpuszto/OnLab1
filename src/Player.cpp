#include "player.hpp"
#include "consts.hpp"
#include "game.hpp"

Player::Player() : Object(Consts::ROOM_WIDTH / 2.f - 5.f, Consts::ROOM_HEIGHT / 2.f - 5.f, 10.f, 10.f) 
{
	_px = _x;
	_py = _y;	
}

void Player::update()
{
	move();
}

void Player::render()
{
	Game::getInstance()->renderObject(_x, _y, _width, _height, 0, 255, 0);
}

void Player::move()
{
	int hor = 0;
	int ver = 0;
	if (wasd & 1)
		ver -= 1;
	if (wasd & 2)
		ver += 1;
	if (wasd & 4)
		hor -= 1;
	if (wasd & 8)
		hor += 1;

	double newx = getX() + hor * _speed;
	double newy = getY() + ver * _speed;

	// Boundary checks
	if (newx < 0) newx = 0;
	if (newy < 0) newy = 0;
	if (newx + _width > Consts::ROOM_WIDTH) newx = Consts::ROOM_WIDTH - _width;
	if (newy + _height > Consts::ROOM_HEIGHT) newy = Consts::ROOM_HEIGHT - _height;

	for (Object* obj : Game::getInstance()->getCurrentLevel()->getCurrentRoom()->getObjects()) {
		if (this->isCollidingWith(*obj)) {
			_x = _px;
			_y = _py;
			return; // Collision detected, do not move
		}
	}
	_px = _x;
	_py = _y;
	_x = newx;
	_y = newy;

}
