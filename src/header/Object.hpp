#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>
#include <vector>

class Object {
public:
	class Sprite {
	public:
		Sprite(SDL_Renderer* renderer, SDL_Texture* _texture, int width, int height, const std::vector<int>& frameCount);

		void setState(int state, void (*callback)() = nullptr);

		void update();
		void render(const SDL_FRect& _destRect, SDL_FlipMode mirror = SDL_FLIP_NONE, double rotation = 0.f) const;
	private:
		SDL_FRect _srcRect;
		float _count;
		int _spriteState;
		const std::vector<int> _frameCount;

		SDL_Texture* _texture;
		SDL_Renderer* _renderer;

		void (*_callback)();
	};

	Object(const std::string& textureName, float x, float y, float w, float h, bool hasCollision = true);
	Object(const Object& other) = delete;
	Object operator=(const Object& other) = delete;

	SDL_FRect getRect() const;

	virtual bool update();
	virtual void render() const;

protected:

	SDL_FRect _rect;
	bool _collisionEnabled;
	Sprite _sprite;

	bool isCollidingWith(const Object* other) const;
};