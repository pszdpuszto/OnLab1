#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>
#include "ObjectSprite.hpp"

class Object {
public:
	Object(const std::string& textureName, float x, float y, float w, float h, bool hasCollision = true);
	Object(const Object& other) = delete;
	Object operator=(const Object& other) = delete;

	SDL_FRect getRect() const;

	virtual bool update();
	virtual void render() const;

protected:
	SDL_FRect _rect;
	bool _collisionEnabled;
	int _spriteState;
	ObjectSprite* _sprite;

	int getSpriteState() const;
	void changeSpriteState(int state, void (*callback)() = nullptr);
	bool isCollidingWith(const Object* other) const;
};