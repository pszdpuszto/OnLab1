#include "../header/game.hpp"

#include "../header/testLevel.hpp"

Game* Game::createInstance(SDL_Window* window) {
	if (_instance != nullptr) {
		delete _instance;
	}
	return _instance = new Game(window);
}

Game* Game::getInstance() {
	return _instance;
}

void Game::destroyInstance() {
	if (_instance != nullptr) {
		delete _instance;
		_instance = nullptr;
	}
}

Game::Game(SDL_Window* window)
	: _isRunning{ false },
	_window{ window },
	_player{ nullptr },
	_currentLevel{ nullptr }
{
	_renderer = SDL_CreateRenderer(_window, NULL);
	SDL_SetRenderLogicalPresentation(_renderer,
		Consts::WINDOW_WIDTH,
		Consts::WINDOW_HEIGHT,
		SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_LETTERBOX
	);
	_resourceManager = new ResourceManager(_renderer);
}

Game::~Game()
{
	delete _currentLevel;
	delete _player;

	SDL_DestroyRenderer(_renderer);
}

Level* Game::getCurrentLevel() const {
	return _currentLevel;
}

Utils::WASDState Game::getWASDState() const
{
	return _wasdState;
}

ObjectSprite* Game::createObjectSprite(const std::string& textureName) const
{
	ResourceManager::TextureData textureData = _resourceManager->getObjectTextureData(textureName);
	return new ObjectSprite(_renderer, textureData.texture, textureData.width, textureData.height, textureData.frameCount);
}

void Game::renderRect(const SDL_FRect& rect, const Utils::RGB& color) const
{
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, &rect);
}

void Game::startGameLoop()
{

	_player = new Player();
	_currentLevel = new TestLevel();
	_isRunning = true;
	while (_isRunning) {
		Uint64 currentTick = SDL_GetTicks();
		tick();
		Uint64 elapsedTick = SDL_GetTicks() - currentTick;
		const static Uint64 targetFrameTick = 1000 / Consts::TARGET_FPS;
		if (elapsedTick < targetFrameTick) {
			Uint64 delayTick = targetFrameTick - elapsedTick;
			SDL_Delay(delayTick); // cap at target FPS
		}
	}

}

Game* Game::_instance = nullptr;

void Game::tick()
{
	handleEvents();
	update();
	render();
}

void Game::handleEvents()
{
	using namespace Utils;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			_isRunning = false;
			return;
		case SDL_EVENT_KEY_DOWN:
			switch (event.key.key) {
			case SDLK_W: _wasdState |= WASD::W; break;
			case SDLK_S: _wasdState |= WASD::S; break;
			case SDLK_A: _wasdState |= WASD::A; break;
			case SDLK_D: _wasdState |= WASD::D; break;
			default: break;
			}
			break;
		case SDL_EVENT_KEY_UP:
			switch (event.key.key) {
			case SDLK_W: _wasdState &= ~WASD::W; break;
			case SDLK_S: _wasdState &= ~WASD::S; break;
			case SDLK_A: _wasdState &= ~WASD::A; break;
			case SDLK_D: _wasdState &= ~WASD::D; break;

			case SDLK_F:
				if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN) {
					SDL_SetWindowFullscreen(_window, 0);
				} else {
				SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
				}
				break;
			default: break;
			}
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
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
	SDL_FRect bg = { 0, 0, Consts::WINDOW_WIDTH, Consts::WINDOW_HEIGHT };
	SDL_RenderTexture(_renderer, _resourceManager->getSimpleTexture("bg"), nullptr, &bg);

	_currentLevel->getCurrentRoom()->render();
	_player->render();

	SDL_RenderPresent(_renderer);

}

