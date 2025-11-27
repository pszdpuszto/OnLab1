#include "../header/game.hpp"

#include "../header/testLevel.hpp"
#include "../header/DroppedItem.hpp"

#include <iostream>

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
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderLogicalPresentation(_renderer,
		Consts::WINDOW_WIDTH,
		Consts::WINDOW_HEIGHT,
		SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_LETTERBOX
	);
	_resourceManager = new ResourceManager(_renderer, "font");
	initSprites();

	SDL_HideCursor();
}

Game::~Game()
{
	delete _currentLevel;
	delete _player;

	for (int i = 0; i < CursorType_SIZE; ++i) {
		delete _cursorSprite[i];
	}
	delete _bgSprite;
	delete _resourceManager;

	SDL_DestroyRenderer(_renderer);
	SDL_ShowCursor();
}

Level* Game::getCurrentLevel() const {
	return _currentLevel;
}

Player* Game::getPlayer() const
{
	return _player;
}

Utils::WASDState Game::getWASDState() const
{
	return _wasdState;
}

Utils::floatPoint Game::getMousePosition() const
{
	return _mousePosition;
}

void Game::moveThroughDoor(Door* door)
{
	getCurrentLevel()->moveToRoom(door->getToRoom());
	_player->moveTo(door->getDoorType());
}

void Game::closestPickupableItemCandidate(DroppedItem* item)
{
	static const float PICKUP_RADIUS = 30.f;
	SDL_FPoint playerCenter = {
		_player->getRect().x + _player->getRect().w / 2.f,
		_player->getRect().y + _player->getRect().h / 2.f
	};
	SDL_FPoint itemCenter = {
		item->getRect().x + item->getRect().w / 2.f,
		item->getRect().y + item->getRect().h / 2.f
	};
	float distance = Utils::dist(playerCenter, itemCenter);
	if (distance < PICKUP_RADIUS && (!_closestDroppedItem.item || _closestDroppedItem.dist > distance)) {
		_closestDroppedItem = { item, distance };
	}
}

DroppedItem* Game::getClosestPickupableItem()
{
	return _closestDroppedItem.item;
}

Object::Sprite Game::createObjectSprite(const std::string& textureName) const
{
	ResourceManager::ObjectTextureData textureData = _resourceManager->getObjectTextureData(textureName);
	return Object::Sprite{
		_renderer, 
		textureData._texture, 
		textureData.width, 
		textureData.height, 
		textureData.frameCount
	};
}

Item::Sprite Game::createItemSprite(const std::string& name) const
{
	return Item::Sprite{
		_renderer,
		_resourceManager->getTexture("items"),
		_resourceManager->getItemSpriteSrcRect(name)
	};
}

Weapon::Sprite Game::createWeaponSprite(const std::string& name, float width, float height, float animationLength)
{
	return Weapon::Sprite{
		_renderer,
		_resourceManager->getTexture("weapons"),
		_resourceManager->getWeaponSpriteSrcRect(name, width, height),
		animationLength
	};
}

SDL_Texture* Game::getMuzzleTexture()
{
	return _resourceManager->getTexture("muzzleFlash");
}

ResourceManager::StaticSprite* Game::createStaticSprite(const std::string& textureName, const SDL_FRect& destRect) const
{
	return new ResourceManager::StaticSprite{
		_renderer,
		_resourceManager->getTexture(textureName),
		destRect
	};
}

ResourceManager::TextSprite* Game::createTextSprite(const std::string& text, const SDL_Color& bgColor) const
{
	return _resourceManager->createTextSprite(text, bgColor);
}

void Game::renderFullRect(const SDL_FRect& rect, const Utils::RGB& color) const
{
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(_renderer, &rect);
}

void Game::renderRect(const SDL_FRect& rect, const Utils::RGB& color) const
{
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
	SDL_RenderRect(_renderer, &rect);
}

void Game::dimScreen(float intensity) const
{
	SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, static_cast<Uint8>(0xff * intensity));
	SDL_RenderFillRect(_renderer, nullptr);
}

void Game::renderLine(const SDL_FPoint p1, const SDL_FPoint p2, const SDL_Color color) const
{
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderLine(_renderer, p1.x, p1.y, p2.x, p2.y);
}


bool Game::isMouseInRect(const SDL_FRect& rect) const
{
	Utils::floatPoint mousePos = getMousePosition();
	return Utils::isColliding(
		mousePos.x, mousePos.y, 0.f, 0.f,
		rect.x, rect.y, rect.w, rect.h
	);
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
			Uint32 delayTick = static_cast<Uint32>(targetFrameTick - elapsedTick);
			SDL_Delay(delayTick); // cap at target FPS
		}
	}

}

Game* Game::_instance = nullptr;

void Game::initSprites()
{
	_cursorSprite[POINT] = createStaticSprite("cursorpoint", { 0.f, 0.f, 9.f, 9.f });
	_cursorSprite[AIM] = createStaticSprite("cursoraim", { 0.f, 0.f, 9.f, 9.f });
	_bgSprite = createStaticSprite("bg", { 0.f, 0.f, Consts::WINDOW_WIDTH, Consts::WINDOW_HEIGHT });
}

void Game::setMousePosition(float x, float y)
{
	int winW, winH;
	SDL_GetWindowSize(_window, &winW, &winH);
	x *= (static_cast<float>(Consts::WINDOW_WIDTH) / static_cast<float>(winW));
	y *= (static_cast<float>(Consts::WINDOW_HEIGHT) / static_cast<float>(winH));
	_mousePosition = { x, y };
}

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
			case SDLK_ESCAPE: _isRunning = false; break;

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

			case SDLK_E:
				_player->openInventory();
				break;

			case SDLK_Q:
				_player->pickUpItem();
				break;

			case SDLK_R:
				_player->useRing();
				break;
			default: break;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			setMousePosition(event.motion.x, event.motion.y);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			_player->mouseDown();
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			_player->mouseUp();
			break;
		default:
			break;
		}
	}
}

void Game::update()
{
	if (!_player->getInvState()) {
		_closestDroppedItem = { nullptr, 0.f };
		_currentLevel->getCurrentRoom()->update();
		_player->update();
	}
}

void Game::render()
{
	/* CLEAR SCREEN */
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);

	/* BACKGROUND */
	_bgSprite->render();

	/* OBJECTS */
	_currentLevel->getCurrentRoom()->render();

	/* PLAYER & HUD */
	_player->render();

	/* INVENTORY */
	_player->renderInv();

	/* CURSOR */
	Utils::floatPoint mousePos = getMousePosition();
	_cursorSprite[POINT]->setPos(mousePos.x, mousePos.y);
	_cursorSprite[AIM]->setPos(mousePos.x-4.5f, mousePos.y-4.5f);

	if (_player->getInvState()) {
		_cursorSprite[POINT]->render();
	}
	else {
		_cursorSprite[AIM]->render();
	}

	/* PRESENT FRAME */
	SDL_RenderPresent(_renderer);
}

