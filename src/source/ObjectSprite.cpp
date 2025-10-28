#include "../header/ObjectSprite.hpp"

#include "../header/Game.hpp"
#include "../header/Consts.hpp"

ObjectSprite::ObjectSprite(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, const std::vector<int>& frameCount)
	: _srcRect{ 0.f, 0.f, static_cast<float>(width), static_cast<float>(height) }, _count{ 0.f }, _spriteState{ 0 }, _frameCount{ frameCount }, 
	_texture{ texture }, _renderer{ renderer }, _callback { nullptr } {};

void ObjectSprite::setState(int state, void(*callback)())
{
	if (_frameCount.empty() || _frameCount.size() <= state)
		return;
	
	_spriteState = state;
	_count = 0.f;
	_srcRect.x = 0;
	_srcRect.y = _srcRect.h * state;
	_callback = callback;
}

void ObjectSprite::update()
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

void ObjectSprite::render(const SDL_FRect& destRect, SDL_FlipMode mirror, double rotation)
{
	SDL_RenderTextureRotated(_renderer, _texture, &_srcRect, &destRect, rotation, nullptr, mirror);
}
