#include "../header/Object.hpp"

#include "../header/Utils.hpp"
#include "../header/Game.hpp"

Object::Object(const std::string& textureName, float x, float y, float w, float h, bool hasCollision)
	: _rect{ x, y, w, h },
	_collisionEnabled{ hasCollision },
	_spriteState{ 0 } {
	_sprite = Game::getInstance()->createObjectSprite(textureName);
};

SDL_FRect Object::getRect() const
{
	return _rect;
}

bool Object::update()
{
	_sprite->update();
	return false;
}

void Object::render() const
{
	_sprite->render(_rect);
}

int Object::getSpriteState() const
{
	return _spriteState;
}

void Object::changeSpriteState(int state, void(*callback)())
{
	if (_spriteState == state)
		return;

	_spriteState = state;
	_sprite->setState(state, callback);
}

bool Object::isCollidingWith(const Object* other) const
{
	return Utils::isColliding(_rect, other->_rect);
}
