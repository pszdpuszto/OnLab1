#pragma once
#include "Entity.hpp"
#include "Projectile.hpp"
#include "Consts.hpp"

#include <string>

class Enemy : public Entity
{
public:
	Enemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed);

	ObjectTypes getType() override { return ENEMY; }

	void render() override;
	bool update() override;

	void takeDamage(float amount, float inv=0.f);

protected:
	enum state_t {
		IDLE = 0,
		MOVING = 1,
		ATTACKING = 2,
		DIEING = 3,
		DEAD = 4
	};
	struct PlayerData { float angle; float dist; };
	PlayerData calcPlayerData() const;
	virtual void moveLogic() = 0;
	virtual void deathLogic();
	float _maxHp, _hp, _dmg;
private:
	void renderHpBar() const;
	float _inv = 0;
};

class MeleeEnemy : public Enemy
{
public:
	MeleeEnemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed);
protected:
	void handleCollision(Object* collidedWith) override;
	virtual void moveLogic() override;
	virtual void doAttack(float range=2.f);
};

class Goblin : public MeleeEnemy
{
public: Goblin(float x, float y) : MeleeEnemy{ "goblin", x, y, 14.f, 19.f, 100.f, 15.f, .75f } {};
};

class Orc : public MeleeEnemy
{
public: Orc(float x, float y) : MeleeEnemy{ "orc", x, y, 24.f, 24.f, 200.f, 50.f, .5f } {};
};

class Thief : public MeleeEnemy
{
public: Thief(float x, float y) : MeleeEnemy{ "thief", x, y, 20.f, 21.f, 50.f, 50.f, 1.25f } {};
};

class SmallSpider : public MeleeEnemy
{
public: SmallSpider(float x, float y) : MeleeEnemy{ "smallSpider", x, y, 16.f, 16.f, 50.f, 60.f, 1.5f } {};
protected:
	bool update() override;
	void deathLogic() override;
	void doAttack(float _=10.f) override;
private :
	enum state_t {
		IDLE = 0,
		MOVING = 1,
		EXPLODING = 2,
		DEAD = 3
	};
	bool _dead = false;
};

class RangedEnemy : public Enemy
{
public:
	RangedEnemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed, float fireCooldown, float range);
protected:
	void moveLogic() override;
	virtual Projectile* createProjectile(float angle);
private:
	float _range;
	float _fireCooldown;
	float _count;

	void doAttack();
};

class GoblinThrower : public RangedEnemy
{
public: GoblinThrower(float x, float y) : RangedEnemy{ "goblinThrow", x, y, 14.f, 19.f, 60.f, 10.f, .6f, 1.f, 75.f } {};
};

class SpiderMother : public Enemy 
{
public: SpiderMother(float x, float y)
	: Enemy{ "spiderBig", x, y, 24.f, 24.f, 200.f, 0.f, 0.f },
	_spawnCooldown{ 5.f * Consts::TARGET_FPS },
	_count{ _spawnCooldown } {};
protected: 
	void moveLogic() override;
private:
	float _spawnCooldown, _count;
};