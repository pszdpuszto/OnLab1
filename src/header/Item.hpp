#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <map>

#include "Utils.hpp"
#include "ResourceManager.hpp"

namespace Stats {
	enum Stat {
		SPD,
		HP,
		REGEN,
		ARMOR,
		DMG,
		ATK_SPD,
		LIFESTEAL,
		Stat_SIZE
	};
	typedef std::map<Stat, float> stat_t;

	stat_t operator+(const stat_t& a, const stat_t& b);
	stat_t& operator+=(stat_t& a, const stat_t& b);
	stat_t operator-(const stat_t& a, const stat_t& b);
	stat_t& operator-=(stat_t& a, const stat_t& b);

	std::string statToString(Stat stat);
}

class Item {
public:
	class Sprite {
	public:
		Sprite(SDL_Renderer* renderer, SDL_Texture* _texture, const SDL_FRect& srcRect);

		void render(const SDL_FRect& _destRect) const;
	private:
		SDL_Renderer* _renderer;
		SDL_Texture* _texture;
		SDL_FRect _srcRect;
	};
	enum Rarity {
		TRASH = 0,
		BORING,
		USEFUL,
		RARE,
		Rarity_SIZE
	};
	enum Type {
		HELMET = 0,
		ARMOR,
		BOOTS,
		WEAPON,
		RING,
		AMULET,
		Type_SIZE
	};

	Item(std::string name, Rarity rarity, Type type, const Stats::stat_t& stats);
	~Item();

	Type getType() const;
	Stats::stat_t getStats() const;

	bool isType(Type type) const;

	void render(Utils::floatPoint pos, bool outline=true) const;
	void renderDescription(Utils::floatPoint mousePos) const;

private:
	static const std::map<Rarity, Utils::RGB> RARITY_COLORS;

	std::string _name;
	Rarity _rarity;
	Type _type;
	Stats::stat_t _stats;

	Sprite _sprite;
	ResourceManager::TextSprite* _descrSprite;

	ResourceManager::TextSprite* createDescrSprite() const;
};

