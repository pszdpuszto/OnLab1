#include "../header/Item.hpp"

#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <format>

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
	generateDescrSprite();
}

std::string Item::getExtraDescription() const
{
	return "";
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

void Item::generateDescrSprite()
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

	oss << getExtraDescription();

	_descrSprite = GAME->createTextSprite(
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

void Ring::use()
{
	if (!_count) {
		doUse();
		_count = _cooldown;
	}
		
}

void Ring::update()
{
	if (_count)
		_count--;
}

void Ring::renderCooldown(SDL_FRect destRect)
{
	destRect.w *= (_count / _cooldown);
	GAME->renderFullRect(destRect, { 255, 255, 0 });
}

void HealingRing::doUse()
{
	GAME->getPlayer()->heal(_healAmount);
}

void DeathRing::doUse()
{
	// TODO
}

Weapon::Weapon(std::string name, Rarity rarity, float dmg, float cooldown, Sprite* sprite)
	: Item{ name, rarity, WEAPON, {} }, _dmg{ dmg }, _cooldown{ cooldown * Consts::TARGET_FPS }, _count{ 0.f }, _sprite{ sprite } 
{
	generateDescrSprite();
}

Weapon::~Weapon()
{
	delete _sprite;
}

void Weapon::update(bool attack)
{
	_sprite->update();
	if (_count)
		_count--;
	else
		doAttack(attack);
}

void Weapon::render()
{
	_sprite->render();
}

void Weapon::renderCooldown(SDL_FRect destRect)
{
	destRect.w *= (_count / _cooldown);
	GAME->renderFullRect(destRect, { 255, 0, 255 });
}

void Weapon::resetCooldown()
{
		_count = _cooldown;
}

void Weapon::doAttack(bool attack)
{
	if (attack) {
		_sprite->startAttack();
		resetCooldown();
	}
}

std::string Weapon::getExtraDescription() const
{
	return std::format("Damage: {}$N$Attack/second: {}", _dmg, Consts::TARGET_FPS / _cooldown);
}

MeleeWeapon::MeleeWeapon(std::string name, Rarity rarity, float dmg, float cooldown, float width, float height, float attackLength) :
	Weapon{ name, rarity, dmg, cooldown, new MeleeSprite{GAME->createWeaponSprite(name, width, height, attackLength * Consts::TARGET_FPS)} } {
};

void MeleeWeapon::doAttack(bool attack)
{
	// TODO: hit detection

	Weapon::doAttack(attack);
}

Weapon::Sprite::Sprite(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FRect srcRect, float animationLength) : 
	_renderer{ renderer }, 
	_texture{ texture }, 
	_srcRect{ srcRect },
	_destRect{ 0.f, 0.f, _srcRect.w, _srcRect.h },
	_count{ 0.f },
	_animationLength{ animationLength } {}

void Weapon::Sprite::startAttack()
{
	_count = _animationLength;
}

void Weapon::Sprite::update()
{
	if (_count)
		_count--;
}

void Weapon::Sprite::render()
{
	if (!_count)
		return;
#ifdef DEBUG_HITBOX
	GAME->renderRect(_destRect, { 255,0,0 });
#endif
	SDL_RenderTextureRotated(_renderer, _texture, &_srcRect, &_destRect, 0.f, nullptr, SDL_FLIP_NONE);
}


void MeleeWeapon::MeleeSprite::render()
{
	if (!_count)
		return;

	Player::LookData ld = GAME->getPlayer()->getLookData();

	/* flip */
	bool flip = ld.a < Consts::PI;

	/* angle */
	float aDeg = Utils::RadToDeg(ld.a);
	float a =  (flip ? 30.f - aDeg : 330.f - aDeg);
	if (flip) {
		a += (1.f-_count / _animationLength) * 120.f;
	} else {
		a -= (1.f-_count / _animationLength) * 120.f;
	}

	/* rotation point */
	SDL_FPoint rPoint = { flip ? 0.f : _srcRect.w, _srcRect.h / 2.f };
	
	/* destRect */
	// closer to player
	static const float inset = 4.f;
	ld.from.x -= inset * sin(ld.a);
	ld.from.y -= inset * cos(ld.a);

	_destRect.x = flip ? ld.from.x : ld.from.x - _destRect.w;
	_destRect.y = ld.from.y - _destRect.h / 2.f;

#ifdef DEBUG_HITBOX
	GAME->renderRect(_destRect, { 255,0,0 });
	GAME->renderFullRect({ ld.from.x, ld.from.y, 1.f, 1.f }, { 0,255,0 });
#endif
	SDL_RenderTextureRotated(_renderer, _texture, &_srcRect, &_destRect, a, &rPoint, flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
}

SDL_FRect MeleeWeapon::MeleeSprite::getHitbox()
{
	return SDL_FRect();
}

RangedWeapon::RangedSprite::RangedSprite(Sprite sprite) : Sprite{ sprite }, _muzzleFlashTexture{ GAME->getMuzzleTexture() }
{
}

void RangedWeapon::RangedSprite::render()
{
	Player::LookData ld = GAME->getPlayer()->getLookData();

	/* flip */
	bool flip = ld.a < Consts::PI;

	/* angle */
	float aDeg = Utils::RadToDeg(ld.a);
	float a = (flip ? 90.f - aDeg : 270.f - aDeg);

	/* rotation point */
	SDL_FPoint rPoint = { flip ? 0.f : _srcRect.w, _srcRect.h / 2.f };

	/* destRect */
	// closer to player
	float inset = _srcRect.w/2.f;
	ld.from.x -= inset * sin(ld.a);
	ld.from.y -= inset * cos(ld.a);

	_destRect.x = flip ? ld.from.x : ld.from.x - _destRect.w;
	_destRect.y = ld.from.y - _destRect.h / 2.f;

	/* _bulletPoint */
	_bulletPoint = {
		ld.from.x + _srcRect.w * sin(ld.a),
		ld.from.y + _srcRect.w * cos(ld.a)
	};

#ifdef DEBUG_HITBOX
	GAME->renderRect(_destRect, { 255,0,0 });
	GAME->renderRect({ ld.from.x, ld.from.y, 1.f, 1.f }, { 0,255,0 });
	GAME->renderRect({ _bulletPoint.x, _bulletPoint.y, 1.f, 1.f }, { 0,255,0 });
#endif
	SDL_RenderTextureRotated(_renderer, _texture, &_srcRect, &_destRect, a, &rPoint, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	/* muzzle flash */
	if (_count) {
		_muzzleDestRect.x = _bulletPoint.x - _muzzleSrcRect.w / 2.f + (_muzzleSrcRect.w / 2.f * sin(ld.a));
		_muzzleDestRect.y = _bulletPoint.y - _muzzleSrcRect.h / 2.f + (_muzzleSrcRect.w / 2.f * cos(ld.a));
		int frameIndex = static_cast<int>((1.f - _count / _animationLength) * 4.f);
		_muzzleSrcRect.x = _muzzleSrcRect.w * frameIndex;
#ifdef DEBUG_HITBOX
		GAME->renderRect(_muzzleDestRect, { 0,0,255 });
#endif
		SDL_RenderTextureRotated(_renderer, _muzzleFlashTexture, &_muzzleSrcRect, &_muzzleDestRect, a, nullptr, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
		

}

SDL_FPoint RangedWeapon::RangedSprite::getBulletPoint()
{
	return _bulletPoint;
}

RangedWeapon::RangedWeapon(std::string name, Rarity rarity, float dmg, float cooldown, float width, float height, float velocity) :
	Weapon{ name, rarity, dmg, cooldown, new RangedSprite{GAME->createWeaponSprite(name, width, height, Consts::TARGET_FPS * .25f)} },
	_velocity{ velocity } {}
void RangedWeapon::doAttack(bool attack)
{
	Weapon::doAttack(attack);
	if (attack)
		spawnBullet();
}

void RangedWeapon::spawnBullet()
{
	float randRad = Utils::DegToRad(static_cast<float>(rand() % 10));
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createBullet(randRad));
}

Bullet* RangedWeapon::createBullet(float offset)
{
	SDL_FPoint bPoint = static_cast<RangedSprite*>(_sprite)->getBulletPoint();
	return new Bullet(bPoint.x - 2.f, bPoint.y - 2.f, GAME->getPlayer()->getLookData().a + offset, _dmg, _velocity);
}

void Shotgun::spawnBullet()
{
	float deg5 = Utils::DegToRad(5.f);
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createBullet(deg5));
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createBullet(0.f));
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createBullet(-deg5));
}

void Sniper::spawnBullet()
{
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createBullet(0.f));
}
