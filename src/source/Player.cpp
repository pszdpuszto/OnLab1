#include "../header/Player.hpp"

#include <sstream>

#include "../header/Game.hpp"
#include "../header/Consts.hpp"
#include "../header/Projectile.hpp"
#include "../header/DroppedItem.hpp"

Player::Player()
	: Entity("player", Consts::ROOM_RECT.w / 2.f - 8.f, Consts::ROOM_RECT.h / 2.f - 10.f, 16.f, 21.f, 1.f * Consts::GLOBAL_SPEED),
	_statsSprite{ createStatsSprite() },
	_invMgr{ this, &_statsSprite }
	 {
	_health = _stats[Stats::HP] / 2.f;
}

Player::~Player()
{
	delete _statsSprite;
}

int Player::getInvState() const
{
	return _invState;
}

Player::LookData Player::getLookData()
{
	Utils::floatPoint mousePos = GAME->getMousePosition();
	float a = atan2f(_rect.x + _rect.w / 2 - mousePos.x, _rect.y + _rect.h / 2 - mousePos.y) + Consts::PI;
	float x = _rect.x + _rect.w / 2 + (_rect.h / 2 * sin(a));
	float y = _rect.y + _rect.h / 2 + (_rect.h / 2 * cos(a));

	return {
		{ x, y },
		a
	};
}

void Player::openInventory()
{
	_invMgr.setAttack(false);
	if (_invState == INVENTORY)
		_invState = CLOSED;
	else
		_invState = INVENTORY;
}

void Player::mouseDown()
{
	if (_invState == CLOSED) {
		_invMgr.setAttack(true);
	}
	else {
		_invMgr.mouseDown();
	}
}

void Player::mouseUp()
{
	if (_invState == CLOSED) {
		_invMgr.setAttack(false);
	}
	else {
		_invMgr.mouseUp();
	}
}

bool Player::update()
{
	pMove();

	_invMgr.update();

	return Object::update();
}

void Player::render()
{
#ifdef DEBUG_HITBOX
	GAME->renderRect(_rect, { 255,0,0 });
#endif
	_sprite.render(_rect, _rect.x+_rect.w/2 >= GAME->getMousePosition().x ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	Weapon* weapon = static_cast<Weapon*>(_invMgr[Item::WEAPON]);
	if (weapon)
		weapon->render();
	renderHUD();
}

void Player::renderInv() const
{
	switch (_invState) {
	case INVENTORY:
		renderInventory();
		break;
	case SKILLTREE:
		renderSkillTree();
		break;
	case CLOSED:
	default:
		break;
	}
}

ResourceManager::TextSprite* Player::createStatsSprite()
{
	std::ostringstream oss;
	oss << "$S8$$C255,255,255$";
	for (int i = 0; i < Stats::Stat_SIZE; i++) {
		Stats::Stat stat = static_cast<Stats::Stat>(i);
		oss << Stats::statToString(stat) << ": " << _stats[stat] << "$N$";
	}
	auto statSprite = GAME->createTextSprite(oss.str());
	statSprite->setPos(_invMgr.getStatPoint());

	return statSprite;
}

void Player::useRing()
{
	Ring* ring = static_cast<Ring*>(_invMgr[Item::RING]);
	if (ring)
		ring->use();
}

void Player::pickUpItem()
{
	DroppedItem* item = GAME->getClosestPickupableItem();
	if (item) {
		InventoryManager::ItemSlot* freeSlot = _invMgr.getFreeSlot();
		if (freeSlot) {
			freeSlot->swapItem(item->pickup());
		}
	}
}

void Player::equipmentChange(bool equip, Item* item)
{
	if (!item)
		return;
	if (equip) {
		_stats += item->getStats();
	}
	else {
		_stats -= item->getStats();
	}
	delete _statsSprite;
	_statsSprite = createStatsSprite();
}

void Player::heal(float amount)
{
	_health += amount;
	if (_health > _stats[Stats::HP])
		_health = _stats[Stats::HP];
}

void Player::takeDamage(float amount)
{
	_health -= amount;
}

void Player::moveTo(Door::DoorType from)
{
	static const float OFFSET_FROM_DOOR = 2.f;
	static const std::map<Door::DoorType, SDL_FPoint> doorPositions = {
		{ Door::NORTH, { Consts::ROOM_RECT.w / 2.f - _rect.w / 2.f, Consts::ROOM_RECT.h-Consts::ROOM_WALL_THICKNESS-_rect.h- OFFSET_FROM_DOOR } },
		{ Door::SOUTH, { Consts::ROOM_RECT.w / 2.f - _rect.w / 2.f, Consts::ROOM_WALL_THICKNESS + OFFSET_FROM_DOOR } },
		{ Door::WEST, { Consts::ROOM_RECT.w - Consts::ROOM_WALL_THICKNESS - _rect.w - OFFSET_FROM_DOOR, Consts::ROOM_RECT.h / 2.f - _rect.h / 2.f } },
		{ Door::EAST, { Consts::ROOM_WALL_THICKNESS + OFFSET_FROM_DOOR, Consts::ROOM_RECT.h / 2.f - _rect.h / 2.f } }
	};
	SDL_FPoint newPos = doorPositions.at(from);
	_rect.x = newPos.x;
	_rect.y = newPos.y;
}

void Player::handleCollision(Object* collidedWith)
{
	if (collidedWith && collidedWith->getType() == DOOR) {
		Door* door = static_cast<Door*>(collidedWith);
		if (door->isOpen()) {
			GAME->moveThroughDoor(door);
		}
	}
	else {
		Entity::handleCollision(collidedWith);
	}
}

void Player::pMove()
{
	using namespace Utils;

	WASDState wasd = GAME->getWASDState();
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
		_sprite.setState(IDLE);
		return;
	}
	_sprite.setState(MOVING);
	move(
		atan2f(static_cast<float>(dir.x), static_cast<float>(dir.y))
	);
}

void Player::renderHUD()
{
	static const SDL_FRect barBack{
		4.f, 4.f,
		200.f, 9.f
	};

	Ring* ring = static_cast<Ring*>(_invMgr[Item::RING]);
	if (ring)
		ring->renderCooldown({ barBack.x, barBack.y - 1, barBack.w, barBack.h });
	Weapon* weapon = static_cast<Weapon*>(_invMgr[Item::WEAPON]);
	if (weapon)
		weapon->renderCooldown({ barBack.x, barBack.y + 1, barBack.w, barBack.h });

	Game* game = GAME;
	SDL_FRect healthBarFront{
		barBack.x + 1.f, barBack.y + 1.f,
		(barBack.w - 2.f) * (_health / _stats.at(Stats::HP)), 4.f
	};
	SDL_FRect xpBarFront{
		barBack.x + 1.f, barBack.y + healthBarFront.h + 2.f,
		(barBack.w - 2.f) * .5f, barBack.h - 2.f - healthBarFront.h - 1.f
	};
	game->renderFullRect(barBack, { 0x00, 0x00, 0x00 });
	game->renderFullRect(healthBarFront, { 0xff, 0x00, 0x00 });
	game->renderFullRect(xpBarFront, { 0x00, 0x00, 0xff });

}

void Player::renderInventory() const
{
	GAME->dimScreen(0.8f);
	_invMgr.render();
}

void Player::renderSkillTree() const
{
	//TODO
}

const SDL_FRect Player::InventoryManager::INV_SPRITE_RECT = {
	Consts::WINDOW_WIDTH / 2.f - 153.f / 2.f,
	Consts::WINDOW_HEIGHT / 2.f - 135.f / 2.f,
	153.f,
	135.f
};

const SDL_FRect Player::InventoryManager::INV_RECT = {
	INV_SPRITE_RECT.x + 86.f,
	INV_SPRITE_RECT.y + 47.f,
	64.f,
	85.f
};

const SDL_FRect Player::InventoryManager::EQ_RECT = {
	INV_SPRITE_RECT.x + 86.f,
	INV_SPRITE_RECT.y + 3.f,
	64.f,
	43.f
};

Utils::floatPoint Player::InventoryManager::STAT_POS = {
	INV_SPRITE_RECT.x + 5.f,
	INV_SPRITE_RECT.y + 56.f
};

Player::InventoryManager::InventoryManager(Player* player, ResourceManager::TextSprite** statSprite)
	: _statsSprite { statSprite }
{
	_invSprite = GAME->createStaticSprite(
		"inventory",
		INV_SPRITE_RECT);

	for (int i = 0; i < INV_COLS * INV_ROWS; ++i) {
		_inv[i] = new ItemSlot{
			{
				INV_RECT.x + 1.f + (i % INV_COLS) * 21.f,
				INV_RECT.y + 1.f + (i / INV_COLS) * 21.f

			}
		};
	}

	_inv[0]->swapItem(new RubyRing);
	_inv[1]->swapItem(new LeatherJacket);
	_inv[2]->swapItem(new VampireAmulet);
	//_inv[3]->swapItem(new CrusaderBoots);
	_inv[4]->swapItem(new BaseballCap);
	_inv[5]->swapItem(new Club);
	_inv[6]->swapItem(new Broom);
	_inv[7]->swapItem(new Dagger);
	_inv[8]->swapItem(new Sword);
	_inv[9]->swapItem(new Pistol);
	_inv[10]->swapItem(new Uzi);
	_inv[11]->swapItem(new Shotgun);
	_inv[3]->swapItem(new Sniper);



	for (int i = 0; i < Item::Type_SIZE; i++) {
		const Item::Type type = static_cast<Item::Type>(i);
		_equippedItems[type] = new ItemSlot{
			{
				EQ_RECT.x + 1.f + (i % INV_COLS) * 21.f,
				EQ_RECT.y + 1.f + (i / INV_COLS) * 21.f
			},
			player,
			[type](Item* item) { return item->isType(type); }
		};
	}

}

Player::InventoryManager::~InventoryManager()
{
	for (int i = 0; i < INV_COLS * INV_ROWS; ++i) {
		if (_inv[i] != nullptr) {
			delete _inv[i];
		}
	}
}

Item* Player::InventoryManager::operator[](Item::Type type)
{
	return _equippedItems[type]->getItem();
}

SDL_FPoint Player::InventoryManager::getStatPoint()
{
	return { STAT_POS.x, STAT_POS.y + 2.f };
}

Player::InventoryManager::ItemSlot* Player::InventoryManager::getFreeSlot() const
{
	for (auto i : _inv) {
		if (i->getItem() == nullptr) {
			return i;
		}
	}
	return nullptr;
}

void Player::InventoryManager::mouseDown()
{
	if (GAME->isMouseInRect(INV_RECT)) {
		startDrag(getInvItemSlotUnderCursor());
	}
	else if (GAME->isMouseInRect(EQ_RECT)) {
		startDrag(getEqItemSlotUnderCursor());
	}
}

void Player::InventoryManager::mouseUp()
{
	if (GAME->isMouseInRect(INV_RECT)) {
		endDrag(getInvItemSlotUnderCursor());
	}
	else if (GAME->isMouseInRect(EQ_RECT)) {
		endDrag(getEqItemSlotUnderCursor());
	} 
	else {
		endDrag(nullptr);
	}
}

void Player::InventoryManager::setAttack(bool attack)
{
	_attack = attack;
}

void Player::InventoryManager::update()
{
	if (_equippedItems[Item::RING]->getItem())
		static_cast<Ring*>(_equippedItems[Item::RING]->getItem())->update();

	if (_equippedItems[Item::WEAPON]->getItem())
		static_cast<Weapon*>(_equippedItems[Item::WEAPON]->getItem())->update(_attack);
}

void Player::InventoryManager::render() const
{
	/* UI */
	_invSprite->render();
	(*_statsSprite)->render();

	/* Equipped items */
	for (int i = 0; i < Item::Type_SIZE; i++) {
		_equippedItems.at(static_cast<Item::Type>(i))->render();
	}

	/* Inventory */
	for (int i = 0; i < INV_COLS * INV_ROWS; ++i) {
		_inv[i]->render();
	}

	/* Dragged item */
	if (_draggedItem) {
		Utils::floatPoint mousePos = GAME->getMousePosition();
		mousePos.x -= 10.f;
		mousePos.y -= 10.f;
		_draggedItem->render(mousePos, false);
	}

	/* item description */
	if (GAME->isMouseInRect(INV_RECT)) {
		getInvItemSlotUnderCursor()->renderDescr();
	}
	else if (GAME->isMouseInRect(EQ_RECT)) {
		getEqItemSlotUnderCursor()->renderDescr();
	}
}

void Player::InventoryManager::startDrag(ItemSlot* src)
{
	if (src == nullptr || src->getItem() == nullptr || _draggedItem != nullptr)
		return;

	_draggedItem = src->swapItem(nullptr);
	_dragSource = src;
}

void Player::InventoryManager::endDrag(ItemSlot* dest)
{
	if (_draggedItem == nullptr || _dragSource == nullptr) {
		_draggedItem = nullptr;
		_dragSource = nullptr;
		return;
	}

	if (dest) {
		if (dest == _dragSource) {
			_dragSource->swapItem(_draggedItem);
		}
		else {
			_dragSource->swapItem(dest->swapItem(_draggedItem));
		}
	}
	else if (GAME->isMouseInRect(INV_SPRITE_RECT)) {
		_dragSource->swapItem(_draggedItem);
	}
	else {
		GAME->getCurrentLevel()->getCurrentRoom()->addObject(new DroppedItem(_draggedItem));
	}

	_draggedItem = nullptr;
	_dragSource = nullptr;
}

Player::InventoryManager::ItemSlot* Player::InventoryManager::getInvItemSlotUnderCursor() const
{
	Utils::floatPoint mousePos = GAME->getMousePosition();
	int col = static_cast<int>((mousePos.x - INV_RECT.x - 1) / 21.f);
	int row = static_cast<int>((mousePos.y - INV_RECT.y - 1) / 21.f);
	int index = row * INV_COLS + col;
	if (index < 0 || index > INV_COLS * INV_ROWS) {
		throw "FUXJ";
	}
	return _inv[index];
}

Player::InventoryManager::ItemSlot* Player::InventoryManager::getEqItemSlotUnderCursor() const
{
	Utils::floatPoint mousePos = GAME->getMousePosition();
	int col = static_cast<int>((mousePos.x - EQ_RECT.x - 1) / 21.f);
	int row = static_cast<int>((mousePos.y - EQ_RECT.y - 1) / 21.f);
	int index = row * INV_COLS + col;
	if (index >= 0 && index < Item::Type_SIZE) {
		return _equippedItems.at(static_cast<Item::Type>(index));
	}
	return nullptr;
}

Player::InventoryManager::ItemSlot::ItemSlot(const Utils::floatPoint& pos, Player* player, std::function<bool(Item*)> filter)
	: _player{ player },
	_pos{ pos },
	_item{ nullptr }, 
	_filter{ filter } {}

Player::InventoryManager::ItemSlot::~ItemSlot()
{
	if (_item)
		delete _item;
}

Item* Player::InventoryManager::ItemSlot::getItem() const
{
	return _item;
}

Item* Player::InventoryManager::ItemSlot::swapItem(Item* item)
{
	if (item == nullptr || _filter(item)) {
		if (_player) {
			_player->equipmentChange(false, _item);
			_player->equipmentChange(true, item);
		}
		Item* back = _item;
		_item = item;
		return back;
	}
	else {
		return item;
	}
}

void Player::InventoryManager::ItemSlot::render() const
{
	if (_item) {
		//GAME->renderFullRect({ _pos.x+2.f, _pos.y+2.f, 16.f, 16.f }, { 120, 144, 156 });
		_item->render(_pos);
	}
}

void Player::InventoryManager::ItemSlot::renderDescr() const
{
	if (_item)
		_item->renderDescription(GAME->getMousePosition());
}
