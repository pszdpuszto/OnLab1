
#include <SDL3/SDL.h>

#include "consts.hpp"
#include "game.hpp"

int main() {

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_Window* window = SDL_CreateWindow("SDL3 Window", 800, 450, SDL_WINDOW_RESIZABLE);

	Game* myGame = Game::createInstance(window);

	myGame->startGameLoop();

	SDL_Quit();

	return 0;
}