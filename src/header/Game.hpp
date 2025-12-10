#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "level.hpp"
#include "player.hpp"
#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "Item.hpp"
#include "Button.hpp"

#define GAME Game::getInstance()

//#define DEBUG_HITBOX

class DroppedItem;

class Game {
public:
	static Game* createInstance(SDL_Window* window);
	static Game* getInstance();
	static void destroyInstance();

	Game(SDL_Window* window);
	~Game();

	Level* getCurrentLevel() const;
	Player* getPlayer() const;
	Utils::WASDState getWASDState() const;
	SDL_FPoint getMousePosition() const;

	void moveThroughDoor(Door* door);
	void closestPickupableItemCandidate(DroppedItem* item);
	DroppedItem* getClosestPickupableItem() const;

	ResourceManager::StaticSprite* createStaticSprite(const std::string& textureName, const SDL_FRect& destRect) const;
	ResourceManager::TextSprite* createTextSprite(const std::string& text, const SDL_Color& bgColor={0x00,0x00,0x00,0x00}) const;
	Object::Sprite createObjectSprite(const std::string& textureName) const;
	Item::Sprite createItemSprite(const std::string& name) const;
	Weapon::Sprite createWeaponSprite(const std::string& name, float width, float height, float animationLength) const;
	Button::Sprite createButtonSprite(const std::string& textureName, const SDL_FRect& destRect) const;
	SDL_Texture* getTexture(std::string name);

	void renderFullRect(const SDL_FRect& rect, const SDL_Color color, float alpha=1.f) const;
	void renderRect(const SDL_FRect& rect, const SDL_Color& color) const;
	void dimScreen(float intensity) const;

	void renderLine(const SDL_FPoint p1, const SDL_FPoint p2, const SDL_Color color) const;

	bool isMouseInRect(const SDL_FRect& rect) const;

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
	SDL_FPoint _mousePosition = { 0.f, 0.f };

	struct { DroppedItem* item; float dist; } _closestDroppedItem = { nullptr, 0.f };

	void initSprites();

	void setMousePosition(float x, float y);

	void tick();
	void handleEvents();
	void update();
	void render();
};