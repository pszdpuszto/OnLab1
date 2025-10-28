#include "../header/Application.hpp"

#include <SDL3/SDL.h>

#include "../header/Game.hpp"

Application::Application()
{
}

Application::~Application()
{
}

int Application::run()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_Window* window = SDL_CreateWindow("Game Window", 800, 450, SDL_WINDOW_RESIZABLE);

	Game* myGame = Game::createInstance(window);

	myGame->startGameLoop();

	Game::destroyInstance();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return SDL_APP_SUCCESS;
}
