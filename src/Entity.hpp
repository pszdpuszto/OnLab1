#pragma once

#include "object.hpp"

class Entity : public Object
{
private:
	double _speed;

protected:
	void doMove(double dx, double dy);
	virtual void handleCollision(Object* collidedWith) = 0;

public:
	Entity(double x, double y, double w, double h, double speed);

	virtual bool update() = 0;

};

