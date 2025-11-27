#include "../header/Projectile.hpp"
#include "../header/Enemy.hpp"
#include "../header/Player.hpp"

Projectile::Projectile(const std::string& textureName, float x, float y, float w, float h, float speed, float a, float dmg, Entity* source)
	: Entity{textureName, x, y, w, h, speed},
	_a{a },
	_dmg{ dmg },
	_die{ false },
	_source{ source } {}

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
	if (that && that != _source) {
		switch (that->getType()) {
			case PROJECTILE:
				return;
			case PLAYER:
				static_cast<Player*>(that)->takeDamage(_dmg);
				break;
			case ENEMY:
				static_cast<Enemy*>(that)->takeDamage(_dmg);
				break;
			default:
				break;
		}
	}
	_die = true;
}
