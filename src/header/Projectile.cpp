#include "Projectile.hpp"

Projectile::Projectile(const std::string& textureName, float x, float y, float w, float h, float speed, float a, float dmg)
	: Entity{textureName, x, y, w, h, speed},
	_a{a },
	_dmg{ dmg },
	_die{ false } {}

bool Projectile::update()
{
	Entity::update();
	move(_a);
	return _die;
}

void Projectile::render()
{
	_sprite.render(_rect, SDL_FLIP_NONE, _a / Consts::PI * 180.f);

}

void Projectile::handleCollision(Object* that)
{
	if (that && that->getType() == PROJECTILE)
		return;
	
	_die = true;
}
