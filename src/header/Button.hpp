#pragma once
#include <SDL3/sdl.h>

#include <string>
#include <functional>

class Button
{
public:
	class Sprite {
	public:
		Sprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect rect);
		void render() const;
	private:
		SDL_Renderer* _renderer;
		SDL_Texture* _texture;
		SDL_FRect _destRect;
	};

	Button(std::string textureName, SDL_FRect destRect, std::function<void()> callback);
	~Button();

	void render() const;
	void handleClick();
private:
	SDL_FRect _rect;
	std::function<void()> _callback;

	Sprite _sprite;

};

