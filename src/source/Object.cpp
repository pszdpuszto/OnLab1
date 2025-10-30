#include "../header/Object.hpp"

#include "../header/Utils.hpp"
#include "../header/Game.hpp"

#include <iostream>

Object::Sprite::Sprite(SDL_Renderer* renderer, SDL_Texture* _texture, int width, int height, const std::vector<int>& frameCount)
	: _srcRect{ 0.f, 0.f, static_cast<float>(width), static_cast<float>(height) },
	_count{ 0.f },
	_spriteState{ 0 },
	_frameCount{ frameCount },
	_texture{ _texture },
	_renderer{ renderer },
	_callback{ nullptr } {
}


void Object::Sprite::setState(int state, void(*callback)())
{
	if (_spriteState == state || _frameCount.empty() || _frameCount.size() <= state)
		return;

	_spriteState = state;
	_count = 0.f;
	_srcRect.x = 0;
	_srcRect.y = _srcRect.h * state;
	_callback = callback;
}

void Object::Sprite::update()
{
	if (_frameCount.empty() || _frameCount[_spriteState] == 0)
		return;

	_count += static_cast<float>(Consts::ANIMATION_FPS) / static_cast<float>(Consts::TARGET_FPS);
	if (_count >= _frameCount[_spriteState]) {
		_count = 0.f;
		if (_callback != nullptr) {
			_callback();
		}
	}
	_srcRect.x = static_cast<float>(_srcRect.w * floor(_count));
}

void Object::Sprite::render(const SDL_FRect& _destRect, SDL_FlipMode mirror, double rotation) const
{
	SDL_RenderTextureRotated(_renderer, _texture, &_srcRect, &_destRect, rotation, nullptr, mirror);
}

Object::Object(const std::string& textureName, float x, float y, float w, float h, bool hasCollision)
	: _rect{ x, y, w, h },
	_collisionEnabled{ hasCollision },
	_sprite{ GAME->createObjectSprite(textureName) } {}

SDL_FRect Object::getRect() const
{
	return _rect;
}

bool Object::update()
{
	_sprite.update();
	return false;
}

void Object::render() const
{
	_sprite.render(_rect);
}

bool Object::isCollidingWith(const Object* other) const
{
	return Utils::isColliding(_rect, other->_rect);
}
