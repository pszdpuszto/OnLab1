#pragma once

#include "Entity.hpp"
#include "Consts.hpp"
#include "Utils.hpp"

class Player : public Entity {
public:
	Player();
	// ~player();

	bool update() override;
	void render() const override;
private:
	float _health;

	struct pStats {
		float damage = 1.f;
		float attackSpeed = 1.f;
		float regeneration = 0.f;
		float movementSpeed = 1.f;
		float maxHealth = 200.f;
		float lifesteal = 0.f;
		float armor = 0.f;
	} _stats;

	enum playerState {
		IDLE = 0,
		MOVING = 1
	};

	void pMove();
	void renderHUD() const;
};