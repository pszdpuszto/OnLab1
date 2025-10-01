#pragma once

#include <SDL3/SDL.h>
#include "level.hpp"
#include "player.hpp"

class Game {
private:
	static Game* _instance;

	bool _isRunning;

	SDL_Renderer* _renderer;
	SDL_Window* _window;
	int _windowWidth = 800;
	int _windowHeight = 450;

	Level* _currentLevel;
	Player* _player;

	void tick();
	void handleEvents();
	void update();
	void render();
	void handleKeyboardEvent(const SDL_Event event);
public:

	static Game* createInstance(SDL_Window* window) {
		if (_instance != nullptr) {
			delete _instance;
		}
		return _instance = new Game(window);
	}

	static Game* getInstance() {
		return _instance;
	}

	static void destroyInstance() {
		if (_instance != nullptr) {
			delete _instance;
			_instance = nullptr;
		}
	}

	Game(SDL_Window* window);
	~Game();

	Level* getCurrentLevel() const {
		return _currentLevel;
	};

	void renderObject(double x, double y, double w, double h, int r, int g, int b);

	void startGameLoop();
};