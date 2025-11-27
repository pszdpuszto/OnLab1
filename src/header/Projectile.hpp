#pragma once

#include "Entity.hpp"
#include "Consts.hpp"

class Projectile : public Entity
{
public:
	Projectile(const std::string& textureName, float x, float y, float w, float h, float speed, float a, float dmg, Entity* source);

	bool update() override;
	void render() override;

	ObjectTypes getType() override { return PROJECTILE; }

protected:
	void handleCollision(Object* that) override;

private:
	float _a;
	float _dmg;
	bool _die;
	Entity* _source;
};

class Bullet : public Projectile
{
public:
	Bullet(float x, float y, float a, float dmg, float velocity, Entity* source) : Projectile{ "bullet", x, y, 4, 4, Consts::GLOBAL_SPEED * velocity, a, dmg, source } {};
};
