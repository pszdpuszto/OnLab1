#include "../header/Item.hpp"

#include <sstream>
#include <algorithm>

#include "../header/Game.hpp"

const std::map<Item::Rarity, Utils::RGB> Item::RARITY_COLORS = {
		{ TRASH, { 0x7f, 0x7f, 0x7f } },
		{ BORING, { 0x00, 0xff, 0x88 } },
		{ USEFUL, { 0x00, 0x22, 0xaa } },
		{ RARE, { 0xbb, 0xaa, 0x00 } }
};

Item::Item(std::string name, Rarity rarity, Type type, const Stats::stat_t& stats) :
	_name{ name },
	_rarity{ rarity },
	_type{ type },
	_stats{ stats },
	_sprite{ GAME->createItemSprite(name)} {
	_descrSprite = createDescrSprite();
}

Item::~Item()
{
	delete _descrSprite;
}

Item::Type Item::getType() const
{
	return _type;
}

Stats::stat_t Item::getStats() const
{
	return _stats;
}

bool Item::isType(Type type) const
{
	return _type == type;
}

void Item::render(Utils::floatPoint pos, bool outline) const
{
	if (outline) {
		SDL_FRect outlineRect{
			pos.x,
			pos.y,
			20.f,
			20.f
		};
		GAME->renderRect(
			outlineRect,
			RARITY_COLORS.at(_rarity)
		);
	}
	SDL_FRect destRect{
		pos.x + 2.f,
		pos.y + 2.f,
		16.f,
		16.f
	};
	_sprite.render(destRect);
}

void Item::renderDescription(Utils::floatPoint mousePos) const
{
	_descrSprite->setPosFitToWindow(mousePos.x + 12.f, mousePos.y + 2.f);
	_descrSprite->render();
}

ResourceManager::TextSprite* Item::createDescrSprite() const
{
	std::ostringstream oss;

	// rarity colored name
	Utils::RGB color = RARITY_COLORS.at(_rarity);
	std::string nameToUpper{ _name };
	std::transform(nameToUpper.begin(), nameToUpper.end(), nameToUpper.begin(), ::toupper);
	oss << "$S8$$C" 
		<< static_cast<int>(color.r) << ","
		<< static_cast<int>(color.g) << ","
		<< static_cast<int>(color.b) << "$"
		<< nameToUpper;

	// stats
	oss << "$S8$$C255,255,255$";
	for (const auto& entry : _stats) {
		oss << Stats::statToString(entry.first) << ": +" << entry.second << "$N$";
	}

	return GAME->createTextSprite(
		oss.str(),
		{ 0x00, 0x00, 0x00, 0xaa }
	);
}

Item::Sprite::Sprite(SDL_Renderer* renderer, SDL_Texture* _texture, const SDL_FRect& srcRect)
	: _renderer{ renderer },
	_texture{ _texture },
	_srcRect{ srcRect } {}

void Item::Sprite::render(const SDL_FRect& _destRect) const
{
	SDL_RenderTexture(_renderer, _texture, &_srcRect, &_destRect);
}

Stats::stat_t Stats::operator+(const stat_t& a, const stat_t& b)
{
	stat_t back;
	for (int i = 0; i < Stat_SIZE; ++i) {
		Stat stat = static_cast<Stat>(i);
		bool aHas = a.find(stat) != a.end();
		bool bHas = b.find(stat) != b.end();
		if (aHas && bHas) {
			back[stat] = a.at(stat) + b.at(stat);
		}
		else if (aHas) {
			back[stat] = a.at(stat);
		}
		else if (bHas) {
			back[stat] = b.at(stat);
		}
	}
	return back;
}

Stats::stat_t& Stats::operator+=(stat_t& a, const stat_t& b)
{
	for (int i = 0; i < Stat_SIZE; ++i) {
		Stat stat = static_cast<Stat>(i);
		bool aHas = a.find(stat) != a.end();
		bool bHas = b.find(stat) != b.end();
		if (aHas && bHas) {
			a[stat] += b.at(stat);
		}
		else if (bHas) {
			a[stat] = b.at(stat);
		}
	}
	return a;
}

Stats::stat_t Stats::operator-(const stat_t& a, const stat_t& b)
{
	stat_t back;
	for (int i = 0; i < Stat_SIZE; ++i) {
		Stat stat = static_cast<Stat>(i);
		bool aHas = a.find(stat) != a.end();
		bool bHas = b.find(stat) != b.end();
		if (aHas && bHas) {
			back[stat] = a.at(stat) - b.at(stat);
		}
		else if (aHas) {
			back[stat] = a.at(stat);
		}
		else if (bHas) {
			back[stat] = -b.at(stat);
		}
	}
	return back;
}

Stats::stat_t& Stats::operator-=(stat_t& a, const stat_t& b)
{
	for (int i = 0; i < Stat_SIZE; ++i) {
		Stat stat = static_cast<Stat>(i);
		bool aHas = a.find(stat) != a.end();
		bool bHas = b.find(stat) != b.end();
		if (aHas && bHas) {
			a[stat] -= b.at(stat);
		}
		else if (bHas) {
			a[stat] = -b.at(stat);
		}
	}
	return a;
}

std::string Stats::statToString(Stat stat)
{
	static const std::map<Stat, std::string> statStrings = {
		{ SPD, "Speed" },
		{ HP, "Health" },
		{ REGEN, "Health Regen" },
		{ ARMOR, "Armor" },
		{ DMG, "Damage" },
		{ ATK_SPD, "Attack Speed" },
		{ LIFESTEAL, "Lifesteal" }
	};

	return statStrings.at(stat);
}
