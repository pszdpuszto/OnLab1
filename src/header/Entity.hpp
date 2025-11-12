#pragma once

#include <string>
#include <map>

#include "object.hpp"
#include "Consts.hpp"
#include "Utils.hpp"

class Entity : public Object
{
public:
	Entity(const std::string& textureName, float x, float y, float w, float h, float speed);

	virtual void render() override;

	ObjectTypes getType() override { return ENTITY; }

protected:
	void move(float angle, float speedModifier = 1.f);
	virtual void handleCollision(Object* collidedWith);
	bool _moveRight;

private:
	float _speed;

	struct previousPosition {
		float x;
		float y;
	} _prevPos;

	void calcPosAfterCollision(SDL_FRect other);
};

