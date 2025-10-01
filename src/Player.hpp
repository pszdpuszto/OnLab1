#pragma once

#include "object.hpp"
#include "consts.hpp"

class Player : public Object {
private:
	double _speed = 2.0 * Consts::GLOBAL_SPEED;
	void move();
	double _px;
	double _py;
public:
	unsigned char wasd = 0;
	Player();
	// ~player();


	void update();
	void render();
};