#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <map>

#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "Consts.hpp"
#include "Projectile.hpp"

#pragma region stats

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
#pragma endregion

#pragma region item

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

	~Item();

	Type getType() const;
	Stats::stat_t getStats() const;

	bool isType(Type type) const;

	void render(Utils::floatPoint pos, bool outline=true) const;
	void renderDescription(Utils::floatPoint mousePos) const;

protected:
	Item(std::string name, Rarity rarity, Type type, const Stats::stat_t& stats);

	virtual std::string getExtraDescription() const;
	virtual void generateDescrSprite();

private:
	static const std::map<Rarity, Utils::RGB> RARITY_COLORS;

	std::string _name;
	Rarity _rarity;
	Type _type;
	Stats::stat_t _stats;

	Sprite _sprite;
	ResourceManager::TextSprite* _descrSprite;
};
#pragma endregion

#pragma region rings

class Ring : public Item {
public:
	Ring(std::string name, Rarity rarity, float cooldown) : Item{ name, rarity, RING, {} }, _cooldown{ cooldown * Consts::TARGET_FPS }, _count{ 0.f } {};
	void use();

	void update();
	void renderCooldown(SDL_FRect destRect) const;
protected:
	virtual void doUse() = 0;
private:
	float _cooldown, _count;
};

class HealingRing : public Ring {
public:
	HealingRing(std::string name, Rarity rarity, float healAmount, float cooldown) : Ring{ name, rarity, cooldown }, _healAmount{ healAmount } {};
protected:
	void doUse() override;
private:
	float _healAmount;
};

class TinCanThingy : public HealingRing {
public: TinCanThingy() : HealingRing{ "Tincan thingy", TRASH, 20.f, 10.f } {};
};

class EmeraldRing : public HealingRing {
public: EmeraldRing() : HealingRing{ "Emerald ring", BORING, 35.f, 10.f } {};
};

class RubyRing : public HealingRing {
public: RubyRing() : HealingRing{ "Ruby ring", USEFUL, 50.f, 10.f } {};
};

class DiamondRing : public HealingRing {
public: DiamondRing() : HealingRing{ "Diamond ring", RARE, 100.f, 15.f } {};
};

class DeathRing : public Ring {
public: 
	DeathRing() : Ring{ "Eing of Death", USEFUL, 30.f } {};
protected:
	void doUse() override;
};
#pragma endregion

#pragma region amulets

class Amulet : public Item {
public:
	Amulet(std::string name, Rarity rarity, const Stats::stat_t& stats) : Item{ name, rarity, AMULET, stats } {};
};

class FightingAmulet : public Amulet {
public:	FightingAmulet() : Amulet{ "Amulet of Fighting", RARE, {	
														{ Stats::DMG, .2f },
														{ Stats::ATK_SPD, .2f}
													} } {};
};

class QuicknessAmulet : public Amulet {
public:	QuicknessAmulet() : Amulet{ "Amulet of Speed", USEFUL, {
														{ Stats::SPD, .2f },
														{ Stats::ATK_SPD, .5f },
													} } {};
};

class StrengthAmulet : public Amulet {
public:	StrengthAmulet() : Amulet{ "Amulet of Strength", BORING, {
														{ Stats::DMG, .4f },
													} } {};
};

class RegenAmulet : public Amulet {
public:	RegenAmulet() : Amulet{ "Amulet of Regeneration", RARE, {
														{ Stats::REGEN, 10.f / Consts::TARGET_FPS },
													} } {};
};

class VampireAmulet : public Amulet {
public:	VampireAmulet() : Amulet{ "Amulet of Speed", RARE, {
														{ Stats::LIFESTEAL, .4f }
													} } {};
};
#pragma endregion

#pragma region armors

class Armor : public Item {
public:
	Armor(std::string name, Rarity rarity, const Stats::stat_t& stats) : Item{ name, rarity, ARMOR, stats } {};
};

class TShirt : public Armor {
public:	TShirt() : Armor{ "T-Shirt", TRASH, {
														{ Stats::ARMOR, .1f },
													} } {};
};

class LeatherJacket : public Armor {
public:	LeatherJacket() : Armor{ "Leather Jacket", BORING, {
														{ Stats::ARMOR, .2f },
													} } {};
};

class IronChestplate : public Armor {
public:	IronChestplate() : Armor{ "Iron Chestplate", USEFUL, {
														{ Stats::SPD, -.2f },
														{ Stats::ARMOR, .4f },
													} } {};
};

class CrusaderArmor : public Armor {
public:	CrusaderArmor() : Armor{ "Crusader Armor", RARE, {
														{ Stats::ARMOR, .4f },
													} } {};
};
#pragma endregion

#pragma region boots

class Boots : public Item {
public:
	Boots(std::string name, Rarity rarity, const Stats::stat_t& stats) : Item{ name, rarity, BOOTS, stats } {};
};

class Sandals : public Boots {
public:	Sandals() : Boots{ "Sandals", TRASH, {
														{ Stats::SPD, .1f },
													} } {};
};

class Sneakers : public Boots {
public:	Sneakers() : Boots{ "Sneakers", USEFUL, {
														{ Stats::SPD, .4f },
													} } {};
};

class IronBoots : public Boots {
public:	IronBoots() : Boots{ "Iron-soled boots", BORING, {
														{ Stats::ARMOR, .1f },
													} } {};
};

class CrusaderBoots : public Boots {
public:	CrusaderBoots() : Boots{ "Crusader Boots", RARE, {
														{ Stats::SPD, .2f },
														{ Stats::ARMOR, .2f },
													} } {};
};
#pragma endregion

#pragma region helmets

class Helmet : public Item {
public:
	Helmet(std::string name, Rarity rarity, const Stats::stat_t& stats) : Item{ name, rarity, HELMET, stats } {};
};

class BaseballCap : public Helmet {
public:	BaseballCap() : Helmet{ "Baseball Cap", TRASH, {
														{ Stats::SPD, .1f },
														{ Stats::ATK_SPD, .1f },
													} } {};
};

class IronHelmet : public Helmet {
public:	IronHelmet() : Helmet{ "Iron Helmet", BORING, {
														{ Stats::ARMOR, .4f },
														{ Stats::ATK_SPD, .1f },
													} } {};
};

class MadCrown : public Helmet {
public:	MadCrown() : Helmet{ "Crown of the Madman", USEFUL, {
														{ Stats::ARMOR, -.5f },
														{ Stats::DMG, .5f },
													} } {
	};
};
#pragma endregion

#pragma region weapons

class Weapon : public Item {
public:
	class Sprite {
	public:
		Sprite(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FRect srcRect, float animationLength);

		void startAttack();
		void update();
		virtual void render();

	protected:
		SDL_FRect _srcRect;
		SDL_FRect _destRect;
		float _count, _animationLength;
		SDL_Texture* _texture;
		SDL_Renderer* _renderer;
	};
	Weapon(std::string name, Rarity rarity, float dmg, float cooldown, Sprite* sprite);
	~Weapon();

	void update(bool attack);
	void render();
	void renderCooldown(SDL_FRect destRect) const;

protected:
	float _dmg;
	Sprite* _sprite;

	void resetCooldown();
	virtual void doAttack(bool attack);

	std::string getExtraDescription() const;
private:
	float _cooldown, _count;
};

#pragma region meleeWeapons

class MeleeWeapon : public Weapon {
public:
	MeleeWeapon(std::string name, Rarity rarity, float dmg, float cooldown, float width, float height, float attackLength=.5f);

private:
	class MeleeSprite : public Weapon::Sprite {
	public:
		MeleeSprite(Sprite sprite) : Sprite{ sprite } {};

		void render() override;
		const SDL_FPoint* getHitbox() const;
		float getAnimationLength() const;
	private:
		SDL_FPoint _hitbox[4] = { 0.f };
	};

	void doAttack(bool attack) override;
};

class Club : public MeleeWeapon {
public: Club() : MeleeWeapon{ "Club", TRASH, 20.f, 1.f, 23.f, 8.f } {};
};

class Broom : public MeleeWeapon {
public: Broom() : MeleeWeapon{ "Broom", TRASH, 20.f, 1.f, 51.f, 11.f } {};
};

class Dagger : public MeleeWeapon {
public: Dagger() : MeleeWeapon{ "Dagger", BORING, 20.f, .5f, 16.f, 8.f, .25f } {};
};

class Sword : public MeleeWeapon {
public: Sword() : MeleeWeapon{ "Sword", USEFUL, 50.f, 1.f, 36.f, 13.f } {};
};
#pragma endregion

#pragma region RangedWeapons

class RangedWeapon : public Weapon {
public:
	RangedWeapon(std::string name, Rarity rarity, float dmg, float cooldown, float width, float height, float velocity=3.f);

protected:
	void doAttack(bool attack) override;

	virtual void spawnBullet();
	Bullet* createBullet(float offset=0.f);
private:
	class RangedSprite : public Weapon::Sprite {
	public:
		RangedSprite(Sprite sprite);

		void render() override;
		SDL_FPoint getBulletPoint() const;
	private:
		SDL_FPoint _bulletPoint = { 0.f, 0.f };
		SDL_Texture* _muzzleFlashTexture;
		SDL_FRect _muzzleSrcRect = { 0.f,0.f,5.f,5.f };
		SDL_FRect _muzzleDestRect = { 0.f,0.f,_muzzleSrcRect.w,_muzzleSrcRect.h };
		int _muzzleCount = 0;
	};

	float _velocity;
};

class Pistol : public RangedWeapon {
public: Pistol() : RangedWeapon{ "Pistol", TRASH, 10.f, .5f, 16.f, 19.f } {};
};

// TODO: bow

class Uzi : public RangedWeapon {
public: Uzi() : RangedWeapon{ "Uzi", BORING, 3.f, .1f, 13.f, 21.f } {};
};

class Shotgun : public RangedWeapon {
public: Shotgun() : RangedWeapon{ "Shotgun", USEFUL, 20.f, 2.f, 31.f, 9.f } {};
protected:
	void spawnBullet() override;
};

class Sniper : public RangedWeapon {
public: Sniper() : RangedWeapon{ "Sniper rifle", RARE, 70.f, 1.5f, 32.f, 9.f, 6.f } {};
protected:
	void spawnBullet() override;
};

#pragma endregion

#pragma endregion
