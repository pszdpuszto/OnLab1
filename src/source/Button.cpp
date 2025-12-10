#include "../header/Button.hpp"
#include "../header/game.hpp"

Button::Sprite::Sprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect rect) :
	_renderer{ renderer },
	_texture{ texture },
	_destRect{ rect } {}

void Button::Sprite::render() const
{
	static SDL_FRect srcRect{ 0.f, 0.f, static_cast<float>(_texture->w), _texture->h / 2.f };
	static SDL_FRect hoverSrcRect{ 0.f, _texture->h / 2.f, static_cast<float>(_texture->w), _texture->h / 2.f };

	SDL_RenderTexture(_renderer, _texture, GAME->isMouseInRect(_destRect) ? &hoverSrcRect : &srcRect, &_destRect);
}


Button::Button(std::string textureName, SDL_FRect destRect, std::function<void()> callback) :
	_rect{ destRect },
	_callback{ callback },
	_sprite{ GAME->createButtonSprite(textureName, destRect)} {}

Button::~Button()
{
}

void Button::render() const
{
	_sprite.render();
}

void Button::handleClick()
{
	if (GAME->isMouseInRect(_rect))
		_callback();
}
