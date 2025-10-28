#include "../header/Player.hpp"

#include "../header/Game.hpp"
#include "../header/Consts.hpp"

Player::Player()
	: Entity("player", Consts::ROOM_RECT.w / 2.f - 8.f, Consts::ROOM_RECT.h / 2.f - 10.f, 16.f, 21.f, 1.f * Consts::GLOBAL_SPEED) {
	_health = _stats.maxHealth;
};

bool Player::update()
{
	pMove();
	return Object::update();
}

void Player::render() const
{
	Entity::render();
	renderHUD();
}

void Player::pMove()
{
	using namespace Utils;

	WASDState wasd = Game::getInstance()->getWASDState();
	intPoint dir = { 0 };

	if (wasd & WASD::W)
		dir.y -= 1;
	if (wasd & WASD::S)
		dir.y += 1;
	if (wasd & WASD::A)
		dir.x -= 1;
	if (wasd & WASD::D)
		dir.x += 1;

	if (dir.x == 0 && dir.y == 0) {
		changeSpriteState(IDLE);
		return;
	}
	changeSpriteState(MOVING);
	move(
		atan2f(static_cast<float>(dir.x), static_cast<float>(dir.y))
	);
}

void Player::renderHUD() const
{
	Game* game = Game::getInstance();
	SDL_FRect healthBarBack{
		4.f, 4.f,
		200.f, 6.f
	};
	game->renderRect(healthBarBack, { 0x00, 0x00, 0x00 });
	SDL_FRect healthBarFront{
		healthBarBack.x + 1.f, healthBarBack.y + 1.f,
		(healthBarBack.w - 2.f) * (_health / _stats.maxHealth), healthBarBack.h - 2.f
	};
	game->renderRect(healthBarFront, { 0xff, 0x00, 0x00 });
}

