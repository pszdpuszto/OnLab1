#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <string>

class ObjectSprite
{
public:
	ObjectSprite(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, const std::vector<int>& frameCount);

	void setState(int state, void (*callback)() = nullptr);

	void update();
	void render(const SDL_FRect& destRect, SDL_FlipMode mirror = SDL_FLIP_NONE, double rotation = 0.f);
private:
	SDL_FRect _srcRect;
	float _count;
	int _spriteState;
	const std::vector<int> _frameCount;

	SDL_Texture* _texture;
	SDL_Renderer* _renderer;

	void (*_callback)();
};

