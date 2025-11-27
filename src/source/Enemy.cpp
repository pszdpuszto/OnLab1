#include "../header/Enemy.hpp"
#include "../header/Game.hpp"
#include "../header/Player.hpp"
#include "../header/Projectile.hpp"

#include <iostream>

Enemy::Enemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed) :
	Entity(textureName, x, y, w, h, speed),
	_maxHp{ hp },
	_hp{ hp },
	_dmg{ dmg }
{}

void Enemy::render()
{
	Entity::render();
	if (_hp > 0.f)
		renderHpBar();
}

bool Enemy::update()
{
	if (_inv) _inv--;
	deathLogic();
	moveLogic();
	return Object::update();
}

void Enemy::takeDamage(float amount, float inv)
{
	if (!_inv) {
		_hp -= amount;
		_inv = inv;
	}
}

Enemy::PlayerData Enemy::calcPlayerData() const
{
	using namespace Utils;
	SDL_FRect player_rect = GAME->getPlayer()->getRect();
	SDL_FPoint playerCenter = {
		player_rect.x + player_rect.w / 2.f,
		player_rect.y + player_rect.h / 2.f
	};
	SDL_FPoint enemyCenter = {
		_rect.x + _rect.w / 2.f,
		_rect.y + _rect.h / 2.f
	};
	return {
		atan2f(playerCenter.x - enemyCenter.x, playerCenter.y - enemyCenter.y),
		dist(playerCenter, enemyCenter)
	};
}

void Enemy::deathLogic()
{
	if (_sprite.getState() < DIEING && _hp <= 0.f) {
		_collisionEnabled = false;
		_sprite.setState(DIEING, [this]() {
			_sprite.setState(DEAD);
			GAME->getCurrentLevel()->getCurrentRoom()->enemyDead();
			});
		return;
	}
}

void Enemy::renderHpBar() const
{
	static const float BAR_WIDTH = 20.f;
	static const float BAR_HEIGHT = 3.f;
	SDL_FRect barBack{
		_rect.x + (_rect.w - BAR_WIDTH) / 2.f,
		_rect.y - BAR_HEIGHT - 2.f,
		BAR_WIDTH,
		BAR_HEIGHT
	};
	GAME->renderFullRect(barBack, { 0x00, 0x00, 0x00 });
	SDL_FRect barFront{
		barBack.x + 1.f,
		barBack.y + 1.f,
		(barBack.w - 2.f) * (_hp / _maxHp),
		BAR_HEIGHT - 2.f
	};
	GAME->renderFullRect(barFront, { 0xff, 0x00, 0x00 });
}

MeleeEnemy::MeleeEnemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed) : Enemy{ textureName, x, y, w, h, hp, dmg, speed }
{
	_sprite.setState(MOVING);
}

void MeleeEnemy::handleCollision(Object* collidedWith)
{
	if (collidedWith && collidedWith->getType() == PLAYER) {
		_sprite.setState(ATTACKING, [this]() {
			_sprite.setState(MOVING);
			doAttack();
		});
	}
	Entity::handleCollision(collidedWith);
}

void MeleeEnemy::moveLogic()
{
	if (_sprite.getState() == MOVING)
		move(calcPlayerData().angle);
}

void MeleeEnemy::doAttack(float range)
{
	if (calcPlayerData().dist <= 
		std::max(GAME->getPlayer()->getRect().w, GAME->getPlayer()->getRect().h) / 2.f + 
		std::max(_rect.w, _rect.h) / 2.f + 
		range) {
		GAME->getPlayer()->takeDamage(_dmg);
	}
}

bool SmallSpider::update()
{
	if (_dead) return true;
	return MeleeEnemy::update();
}

void SmallSpider::deathLogic()
{
	if (_sprite.getState() < EXPLODING && _hp <= 0.f) {
		_collisionEnabled = false;
		_sprite.setState(EXPLODING, [this]() {
			_dead = true;
			});
		return;
	}
}

void SmallSpider::doAttack(float _)
{
	MeleeEnemy::doAttack(10.f);
	_sprite.setState(DEAD);
	_dead = true;
}

RangedEnemy::RangedEnemy(const std::string& textureName, float x, float y, float w, float h, float hp, float dmg, float speed, float fireCooldown, float range)
	: Enemy{ textureName, x, y, w, h, hp, dmg, speed },
	_fireCooldown{ fireCooldown * Consts::TARGET_FPS },
	_count{ _fireCooldown },
	_range{ range }
{
	_sprite.setState(IDLE);
}

void RangedEnemy::moveLogic()
{
	if (_sprite.getState() < DIEING && _hp <= 0.f) {
		_collisionEnabled = false;
		_sprite.setState(DIEING, [this]() {
			_sprite.setState(DEAD);
			GAME->getCurrentLevel()->getCurrentRoom()->enemyDead();
			});
		return;
	}

	PlayerData pData = calcPlayerData();

	if (_sprite.getState() == MOVING) {
		move(pData.angle);
		if (pData.dist <= _range) {
			_sprite.setState(IDLE);
		}
	}
	else if (_sprite.getState() == IDLE) {
		if (pData.dist < _range) {
			if (_count) _count--;
			else {
				_sprite.setState(ATTACKING, [this]() {
					_sprite.setState(IDLE);
					doAttack();
					});
			}
		}
		else {
			_sprite.setState(MOVING);
		}
	}
}

Projectile* RangedEnemy::createProjectile(float angle)
{
	return new Bullet(
		_rect.x + _rect.w / 2.f - 2.f + (std::max(_rect.w, _rect.h) / 2.f + 2.f) * sinf(angle),
		_rect.y + _rect.h / 2.f - 2.f + (std::max(_rect.w, _rect.h) / 2.f + 2.f) * cosf(angle),
		angle,
		_dmg,
		2.f,
		this
	);
}

void RangedEnemy::doAttack()
{
	float angle = calcPlayerData().angle;
	GAME->getCurrentLevel()->getCurrentRoom()->addObject(createProjectile(angle));
	_count = _fireCooldown;
}

void SpiderMother::moveLogic()
{
	if (_sprite.getState() >= DIEING)
		return;
	if (_count) _count--;
	else {
		GAME->getCurrentLevel()->getCurrentRoom()->addObject(new SmallSpider(
			_rect.x + _rect.w / 2.f - 8.f,
			_rect.y + _rect.h / 2.f - 8.f
		));
		_count = _spawnCooldown;
	}
}
