#include "../header/Entity.hpp"

#include "../header/Game.hpp"
#include "../header/Consts.hpp"

Entity::Entity(const std::string& textureName, float x, float y, float w, float h, float speed)
	: Object(textureName, x, y, w, h), _speed(speed* Consts::GLOBAL_SPEED), _moveRight{ false }, _prevPos{x ,y} { }

void Entity::render() const
{
	_sprite->render(_rect, _moveRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

void Entity::move(float a, float speedModifier)
{
	using namespace Consts;

	_moveRight = a < 0.f;

	_prevPos = { _rect.x, _rect.y };
	_rect.x += sinf(a) * _speed * speedModifier;
	_rect.y += cosf(a) * _speed * speedModifier;

	if (_rect.x < ROOM_WALL_THICKNESS) _rect.x = ROOM_WALL_THICKNESS;
	if (_rect.y < ROOM_WALL_THICKNESS) _rect.y = ROOM_WALL_THICKNESS;
	if (_rect.x + _rect.w > WINDOW_WIDTH - ROOM_WALL_THICKNESS) _rect.x = WINDOW_WIDTH - ROOM_WALL_THICKNESS - _rect.w;
	if (_rect.y + _rect.h > WINDOW_HEIGHT - ROOM_WALL_THICKNESS) _rect.y = WINDOW_HEIGHT - ROOM_WALL_THICKNESS - _rect.h;

	for (Object* obj : Game::getInstance()->getCurrentLevel()->getCurrentRoom()->getObjects()) {
		if (this->isCollidingWith(obj)) {
			handleCollision(obj);

			return;
		}
	}
	_rect.x = _rect.x;
	_rect.y = _rect.y;
}

void Entity::handleCollision(Object* collidedWith)
{
	calcPosAfterCollision(collidedWith->getRect());
}

void Entity::calcPosAfterCollision(SDL_FRect other)
{
	if (_prevPos.x + _rect.w <= other.x) {
		_rect.x = other.x - _rect.w;
	}
	else if (_prevPos.x >= other.x + other.w) {
		_rect.x = other.x + other.w;
	}
	if (_prevPos.y + _rect.h <= other.y) {
		_rect.y = other.y - _rect.h;
	}
	else if (_prevPos.y >= other.y + other.h) {
		_rect.y = other.y + other.h;
	}
}


