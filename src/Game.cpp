#include "game.hpp"
#include "testLevel.hpp"
#include <iostream>

Game* Game::_instance = nullptr;

void Game::tick()
{
	handleEvents();
	update();
	render();
}

void Game::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_EVENT_QUIT:
			_isRunning = false;
			return;
		case SDL_EVENT_WINDOW_RESIZED:
			_windowWidth = event.window.data1;
			_windowHeight = event.window.data2;
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			handleKeyboardEvent(event);
			break;
		default:
			break;
		}
	}
}

void Game::update()
{
	_currentLevel->getCurrentRoom()->update();
	_player->update();
}

void Game::render()
{
	SDL_SetRenderDrawColor(_renderer, 0, 100, 150, 255);
	SDL_RenderClear(_renderer);

	_currentLevel->getCurrentRoom()->render();
	_player->render();

	SDL_RenderPresent(_renderer);

}

void Game::handleKeyboardEvent(const SDL_Event event)
{
	if (event.type == SDL_EVENT_KEY_DOWN) {
		switch (event.key.key) {
		case SDLK_W:
			_player->wasd |= 1;
			break;
		case SDLK_S:
			_player->wasd |= 2;
			break;
		case SDLK_A:
			_player->wasd |= 4;
			break;
		case SDLK_D:
			_player->wasd |= 8;
			break;
		default:
			break;
		}
	} else {
		switch (event.key.key) {
		case SDLK_W:
			_player->wasd &= 30;
			break;
		case SDLK_S:
			_player->wasd &= 29;
			break;
		case SDLK_A:
			_player->wasd &= 27;
			break;
		case SDLK_D:
			_player->wasd &= 23;
			break;
		default:
			break;
		}
	}
}

Game::Game(SDL_Window* window)
{
	_isRunning = false;

	_window = window;
	SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);
	_renderer = SDL_CreateRenderer(_window, NULL);


	_player = new Player();
	_currentLevel = new TestLevel();
}

Game::~Game()
{
	delete _currentLevel;
}

void Game::renderObject(double x, double y, double w, double h, int r, int g, int b)
{
	const float scaleX = _windowWidth / Consts::ROOM_WIDTH;
	const float scaleY = _windowHeight / Consts::ROOM_HEIGHT;
	const SDL_FRect rect = { x * scaleX, y * scaleY, w * scaleX, h * scaleY };
	SDL_SetRenderDrawColor(_renderer, r, g, b, 255);
	SDL_RenderFillRect(_renderer, &rect);
}

void Game::startGameLoop()
{
	_isRunning = true;
	while (_isRunning) {
		Uint64 currentTick = SDL_GetTicks();
		tick();
		Uint64 elapsedTick = SDL_GetTicks() - currentTick;
		if (elapsedTick < 33) {
			Uint64 delayTick = 33 - elapsedTick;
			SDL_Delay(delayTick); // cap at 60 FPS
		}
	}

}
