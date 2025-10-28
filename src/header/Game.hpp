#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "level.hpp"
#include "player.hpp"
#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "ObjectSprite.hpp"

class Game {
public:
	static Game* createInstance(SDL_Window* window);
	static Game* getInstance();
	static void destroyInstance();

	Game(SDL_Window* window);
	~Game();

	Level* getCurrentLevel() const;
	Utils::WASDState getWASDState() const;

	ObjectSprite* createObjectSprite(const std::string& textureName) const;

	void renderRect(const SDL_FRect& rect, const Utils::RGB& color) const;

	void startGameLoop();
private:
	static Game* _instance;

	bool _isRunning;

	SDL_Renderer* _renderer;
	SDL_Window* _window;

	ResourceManager* _resourceManager;

	Level* _currentLevel;
	Player* _player;
	Utils::WASDState _wasdState = 0;

	void tick();
	void handleEvents();
	void update();
	void render();
};