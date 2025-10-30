#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "level.hpp"
#include "player.hpp"
#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "Item.hpp"

#define GAME Game::getInstance()

class Game {
public:
	static Game* createInstance(SDL_Window* window);
	static Game* getInstance();
	static void destroyInstance();

	Game(SDL_Window* window);
	~Game();

	Level* getCurrentLevel() const;
	Utils::WASDState getWASDState() const;
	Utils::floatPoint getMousePosition() const;

	ResourceManager::StaticSprite* createStaticSprite(const std::string& textureName, const SDL_FRect& destRect) const;
	ResourceManager::TextSprite* createTextSprite(const std::string& text, const SDL_Color& bgColor={0x00,0x00,0x00,0x00}) const;
	Object::Sprite createObjectSprite(const std::string& textureName) const;
	Item::Sprite createItemSprite(const std::string& name) const;

	void renderFullRect(const SDL_FRect& rect, const Utils::RGB& color) const;
	void renderRect(const SDL_FRect& rect, const Utils::RGB& color) const;
	void dimScreen(float intensity) const;

	bool isMouseInRect(const SDL_FRect& rect);

	void startGameLoop();
private:
	static Game* _instance;

	bool _isRunning;

	SDL_Renderer* _renderer;
	SDL_Window* _window;

	ResourceManager* _resourceManager;
	enum CursorType { POINT = 0, AIM, CursorType_SIZE };
	ResourceManager::StaticSprite* _cursorSprite[CursorType_SIZE];
	ResourceManager::StaticSprite* _bgSprite;

	Level* _currentLevel;
	Player* _player;
	Utils::WASDState _wasdState = 0;
	Utils::floatPoint _mousePosition = { 0.f, 0.f };

	void initSprites();

	void setMousePosition(float x, float y);

	void tick();
	void handleEvents();
	void update();
	void render();
};