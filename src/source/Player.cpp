#include "../header/Player.hpp"

#include <sstream>
#include <algorithm>

#include "../header/Game.hpp"
#include "../header/Consts.hpp"
#include "../header/Projectile.hpp"
#include "../header/DroppedItem.hpp"

Player::Player()
	: Entity("player", Consts::ROOM_RECT.w / 2.f - 8.f, Consts::ROOM_RECT.h / 2.f - 10.f, 16.f, 21.f, 1.f * Consts::GLOBAL_SPEED),
	_statsSprite{ createStatsSprite() },
	_invMgr{ this, &_statsSprite },
	_skillTree{ this },
	_lvl{ 1 },
	_xpNeeded{ getXpNeeded(_lvl) }
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
	SDL_FPoint mousePos = GAME->getMousePosition();
	float a = atan2f(_rect.x + _rect.w / 2 - mousePos.x, _rect.y + _rect.h / 2 - mousePos.y) + Consts::PI;
	float x = _rect.x + _rect.w / 2 + (_rect.h / 2 * sin(a));
	float y = _rect.y + _rect.h / 2 + (_rect.h / 2 * cos(a));

	return {
		{ x, y },
		a
	};
}

Stats::stat_t Player::getStats() const
{
	return _stats;
}

int Player::getLevel() const
{
	return _lvl;
}

void Player::openInventory()
{
	_invMgr.setAttack(false);
	if (_invState > CLOSED)
		_invState = CLOSED;
	else
		_invState = INVENTORY;
}

void Player::switchSkillTree()
{
	if (_invState == SKILLTREE)
		_invState = INVENTORY;
	else
		_invState = SKILLTREE;
}

void Player::mouseDown()
{
	switch (_invState) {
	case CLOSED:
		_invMgr.setAttack(true);
		break;
	case INVENTORY:
		_invMgr.mouseDown();
		break;
	case SKILLTREE:
		_skillTree.mouseDown();
		break;
	default: break;
	}
}

void Player::mouseUp()
{
	switch (_invState) {
	case CLOSED:
		_invMgr.setAttack(false);
		break;
	case INVENTORY:
		_invMgr.mouseUp();
		break;
	case SKILLTREE:
		_skillTree.mouseUp();
		break;
	default: break;
	}
}

bool Player::update()
{
	pMove();

	if (_invState == CLOSED) {
		heal(_stats.at(Stats::REGEN) * Consts::TARGET_FPS);
	}

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

int Player::getXpNeeded(int level)
{
	if (level < 5)
		return level * 100;
	else if (level < 15)
		return 500 + (level - 5) * 50;
	else
		return 1000;
}

ResourceManager::TextSprite* Player::createStatsSprite()
{
	std::ostringstream oss;
	oss << "$S8$$C255,255,255$";
	for (std::pair<Stats::Stat, float> entry : _stats) {
		oss << Stats::statToString(entry, false) << "$N$";
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

void Player::addSkillStat(Stats::stat_t stat)
{
	_stats += stat;
	delete _statsSprite;
	_statsSprite = createStatsSprite();
}

void Player::unlockSlot(Item::Type type)
{
	_invMgr.unlockSlot(type);
}

void Player::heal(float amount)
{
	_health += amount;
	if (_health > _stats[Stats::HP])
		_health = _stats[Stats::HP];
}

void Player::takeDamage(float amount)
{
	_health -= amount * (1.f - _stats.at(Stats::ARMOR));
}

void Player::dealDamage(float amount)
{
	heal(amount * _stats.at(Stats::LIFESTEAL));
}

void Player::addXP(int amount)
{
	_xp += amount;
	if (_xp >= _xpNeeded) {
		levelUp();
	}
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
	SDL_Point dir = { 0 };

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
		atan2f(static_cast<float>(dir.x), static_cast<float>(dir.y)),
		_stats.at(Stats::SPD)
	);
}

void Player::levelUp()
{
	_lvl++;
	_xp -= _xpNeeded;
	_xpNeeded = getXpNeeded(_lvl);
	_skillTree.levelUp(_lvl);
}

void Player::renderHUD()
{
	static const SDL_FRect barBack{
		16.f, 4.f,
		200.f, 9.f
	};

	_skillTree.renderLevel({
		barBack.x - 12.f, barBack.y - 4.f,
		15.f, 15.f
	});

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
		(barBack.w - 2.f) * (static_cast<float>(_xp) / static_cast<float>(_xpNeeded)), barBack.h - 2.f - healthBarFront.h - 1.f
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
	GAME->dimScreen(0.8f);
	_skillTree.render();
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

const SDL_FRect Player::InventoryManager::SKILLTREE_BTN_RECT = {
	INV_SPRITE_RECT.x + 21.f,
	INV_SPRITE_RECT.y + 42.f,
	34.f,
	9.f
};

SDL_FPoint Player::InventoryManager::STAT_POS = {
	INV_SPRITE_RECT.x + 5.f,
	INV_SPRITE_RECT.y + 56.f
};

Player::InventoryManager::InventoryManager(Player* player, ResourceManager::TextSprite** statSprite) :
	_statsSprite { statSprite },
	_skillTreeBtn{ "skillTreeBtn", SKILLTREE_BTN_RECT, std::bind(&Player::switchSkillTree, player)}
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

	_inv[0]->swapItem(new DeathRing);
	_inv[1]->swapItem(new LeatherJacket);
	_inv[2]->swapItem(new VampireAmulet);
	_inv[3]->swapItem(new CrusaderBoots);
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
			[type](Item* item) { return item->isType(type); },
			type == Item::ARMOR || type == Item::RING || type == Item::AMULET
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
	else if (_draggedItem) {
		endDrag(nullptr);
	}
	else {
		_skillTreeBtn.handleClick();
	}
}

void Player::InventoryManager::setAttack(bool attack)
{
	_attack = attack;
}

void Player::InventoryManager::unlockSlot(Item::Type type)
{
	_equippedItems.at(type)->unlock();
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

	/* Skilltree button */
	_skillTreeBtn.render();

	/* Dragged item */
	if (_draggedItem) {
		SDL_FPoint mousePos = GAME->getMousePosition();
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
	SDL_FPoint mousePos = GAME->getMousePosition();
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
	SDL_FPoint mousePos = GAME->getMousePosition();
	int col = static_cast<int>((mousePos.x - EQ_RECT.x - 1) / 21.f);
	int row = static_cast<int>((mousePos.y - EQ_RECT.y - 1) / 21.f);
	int index = row * INV_COLS + col;
	if (index >= 0 && index < Item::Type_SIZE) {
		return _equippedItems.at(static_cast<Item::Type>(index));
	}
	return nullptr;
}

Player::InventoryManager::ItemSlot::ItemSlot(const SDL_FPoint& pos, Player* player, std::function<bool(Item*)> filter, bool locked)
	: _player{ player },
	_pos{ pos },
	_item{ nullptr }, 
	_filter{ filter },
	_locked{ locked } {
}

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
	if (!_locked && (item == nullptr || _filter(item))) {
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

void Player::InventoryManager::ItemSlot::unlock()
{
	_locked = false;
}

void Player::InventoryManager::ItemSlot::render() const
{
	if (_item) {
		_item->render(_pos);
	}
	if (_locked) {
		renderLock();
	}
}

void Player::InventoryManager::ItemSlot::renderDescr() const
{
	if (_item)
		_item->renderDescription(GAME->getMousePosition());
}

ResourceManager::StaticSprite* Player::InventoryManager::ItemSlot::LOCK_SPRITE = nullptr;


void Player::InventoryManager::ItemSlot::renderLock() const
{
	if (LOCK_SPRITE == nullptr) {
		LOCK_SPRITE = GAME->createStaticSprite("lock", { 0.f, 0.f, 20.f, 20.f });
	}
	GAME->renderFullRect({_pos.x, _pos.y, 20.f, 20.f}, { 0,0,0 }, .6f);
	LOCK_SPRITE->setPos({ _pos.x, _pos.y });
	LOCK_SPRITE->render();
}

const SDL_FRect Player::SkillTree::SKILLTREE_RECT = {
	Consts::WINDOW_WIDTH / 2.f - 180.f / 2.f,
	Consts::WINDOW_HEIGHT / 2.f - 135.f / 2.f,
	180.f, 
	135.f
};

const SDL_FRect Player::SkillTree::SKILLPOINTS_RECT = {
	SKILLTREE_RECT.x + 82.f,
	SKILLTREE_RECT.y + 43.f,
	15.f, 
	15.f
};

const SDL_FRect Player::SkillTree::LEVEL_RECT = {
	SKILLTREE_RECT.x + 82.f,
	SKILLTREE_RECT.y + 19.f,
	15.f,
	15.f
};

const SDL_FRect Player::SkillTree::BACK_BTN_RECT = {
	SKILLTREE_RECT.x + 78.f,
	SKILLTREE_RECT.y + 89.f,
	19.f,
	9.f
};

Player::SkillTree::SkillNode::SkillNode(std::string name, Stats::stat_t stat, SDL_FRect rect, std::function<void(Stats::stat_t)> addStats, SkillNode* parent, std::string extraDescr) :
	_stats{ stat },
	_addStats{ addStats },
	_rect{ rect },
	_parent{ parent } {
	createDescriptionSprite(name, extraDescr);
}
Player::SkillTree::SkillNode::~SkillNode()
{
	delete _descriptionSprite;
}
;

bool Player::SkillTree::SkillNode::isUnlocked() const
{
	return _unlocked;
}

void Player::SkillTree::SkillNode::render(bool hasPoints) const
{
	if (_unlocked) {
		GAME->renderRect(_rect, { 255, 255, 255 });
	}
	else if (!(hasPoints && unlockable())) {
		GAME->renderFullRect(_rect, { 0, 0, 0 }, .6f);
	}
}

void Player::SkillTree::SkillNode::renderDescr(SDL_FPoint mousePos)
{
	if (GAME->isMouseInRect(_rect)) {
		_descriptionSprite->setPosFitToWindow(GAME->getMousePosition().x + 12.f, GAME->getMousePosition().y + 2.f);
		_descriptionSprite->render();
	}
}

bool Player::SkillTree::SkillNode::handleClick()
{
	if (GAME->isMouseInRect(_rect) && !_unlocked && unlockable()) {
		_unlocked = true;
		_addStats(_stats);
		return true;
	}
	return false;
}

void Player::SkillTree::SkillNode::createDescriptionSprite(std::string name, std::string extraDescr)
{
	if (_descriptionSprite) {
		delete _descriptionSprite;
	}

	std::ostringstream oss;
	oss << "$S8$$C255,255,255$" << name << "$N$$C127,127,127$";
	for (const auto& entry : _stats) {
		oss << Stats::statToString(entry) << "$N$";
	}

	oss << extraDescr;

	_descriptionSprite = GAME->createTextSprite(oss.str(), { 0, 0, 0, 0xaa });
}

bool Player::SkillTree::SkillNode::unlockable() const
{
	return !_parent || _parent->isUnlocked();
}

Player::SkillTree::SkillTree(Player* player) :
	_backBtn{ "backBtn", BACK_BTN_RECT, std::bind(&Player::switchSkillTree, player)},
	_skillTreeSprite{ GAME->createStaticSprite("skills", SKILLTREE_RECT) },
	_skillPointsSprite{ createNumberSprite(0) },
	_LevelSprite{ createNumberSprite(player->getLevel()) }
{
	createSkills(player);
	_skillPointsSprite->setPos({ SKILLPOINTS_RECT.x, SKILLPOINTS_RECT.y });
	_LevelSprite->setPos({ LEVEL_RECT.x, LEVEL_RECT.y });
}

Player::SkillTree::~SkillTree()
{
	delete _skillTreeSprite;
	delete _skillPointsSprite;
	delete _LevelSprite;
}

void Player::SkillTree::render() const
{
	_skillTreeSprite->render();
	_skillPointsSprite->render();
	_LevelSprite->render();
	_backBtn.render();

	std::for_each(_skills.begin(), _skills.end(), [=](SkillNode* s) { s->render(_skillPoints > 0); });
	std::for_each(_skills.begin(), _skills.end(), [=](SkillNode* s) { s->renderDescr(GAME->getMousePosition()); });
}

void Player::SkillTree::levelUp(int level)
{
	delete _LevelSprite;
	_LevelSprite = createNumberSprite(level);
	_LevelSprite->setPos({ LEVEL_RECT.x, LEVEL_RECT.y });
	
	_skillPoints++;
	updateSkillPointsSprite();
}

void Player::SkillTree::mouseDown()
{

}

void Player::SkillTree::mouseUp()
{
	_backBtn.handleClick();
	if (_skillPoints <= 0)
		return;
	for (SkillNode* skill : _skills) {
		if (skill->handleClick()) {
			_skillPoints--;
			updateSkillPointsSprite();
			break;
		}
	}
}

void Player::SkillTree::renderLevel(SDL_FRect dest) const
{
	_LevelSprite->setPos({ dest.x, dest.y });
	_LevelSprite->render();
}

ResourceManager::TextSprite* Player::SkillTree::createNumberSprite(int value)
{
	return GAME->createTextSprite("$S15$$C0,0,0$" + std::to_string(value));
}

void Player::SkillTree::updateSkillPointsSprite()
{
	delete _skillPointsSprite;
	_skillPointsSprite = createNumberSprite(_skillPoints);
	_skillPointsSprite->setPos({ SKILLPOINTS_RECT.x, SKILLPOINTS_RECT.y });
}

void Player::SkillTree::createSkills(Player* player)
{
	SkillNode* middle = new SkillNode(
		"Stylish",
		{ },
		{ SKILLTREE_RECT.x + 81.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		[=](Stats::stat_t _) { player->unlockSlot(Item::AMULET); },
		nullptr,
		"Unlocks the Amulet slot."
	);
	_skills.push_back(middle);

	SkillNode* r = new SkillNode(
		"Combat expertise",
		{ { Stats::DMG, .1f } },
		{ SKILLTREE_RECT.x + 107.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		middle
	);
	_skills.push_back(r);

	SkillNode* ru = new SkillNode(
		"A difficult childhood",
		{ { Stats::DMG, .2f } },
		{ SKILLTREE_RECT.x + 107.f, SKILLTREE_RECT.y + 37.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		r
	);
	_skills.push_back(ru);

	SkillNode* ruu = new SkillNode(
		"Half ogre",
		{ { Stats::DMG, .25f } },
		{ SKILLTREE_RECT.x + 107.f, SKILLTREE_RECT.y + 11.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		ru
	);
	_skills.push_back(ruu);

	SkillNode* rur = new SkillNode(
		"Leech therapy",
		{ { Stats::LIFESTEAL, .1f } },
		{ SKILLTREE_RECT.x + 133.f, SKILLTREE_RECT.y + 37.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		ru
	);
	_skills.push_back(rur);

	SkillNode* rr = new SkillNode(
		"Superstitions",
		{ },
		{ SKILLTREE_RECT.x + 133.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		[=](Stats::stat_t _) { player->unlockSlot(Item::RING); },
		r,
		"Unlocks the Ring slot."
	);
	_skills.push_back(rr);

	SkillNode* rrr = new SkillNode(
		"Quick hands",
		{ { Stats::ATK_SPD, .15f } },
		{ SKILLTREE_RECT.x + 159.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		rr
	);
	_skills.push_back(rrr);

	SkillNode* rd = new SkillNode(
		"Ammo thief",
		{ { Stats::ATK_SPD, .1f } },
		{ SKILLTREE_RECT.x + 107.f, SKILLTREE_RECT.y + 89.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		r
	);
	_skills.push_back(rd);

	SkillNode* rdr = new SkillNode(
		"Lightning fingers",
		{ { Stats::ATK_SPD, .1f } },
		{ SKILLTREE_RECT.x + 133.f, SKILLTREE_RECT.y + 89.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		rd
	);
	_skills.push_back(rdr);

	SkillNode* rdd = new SkillNode(
		"The power of cowardice",
		{ { Stats::SPD, .1f } },
		{ SKILLTREE_RECT.x + 107.f, SKILLTREE_RECT.y + 115.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		rd
	);
	_skills.push_back(rdd);

	SkillNode* l = new SkillNode(
		"Slum experience",
		{ { Stats::HP, 25.f } },
		{ SKILLTREE_RECT.x + 55.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		middle
	);
	_skills.push_back(l);

	SkillNode* lu = new SkillNode(
		"Tough nut",
		{ { Stats::HP, 50.f } },
		{ SKILLTREE_RECT.x + 55.f, SKILLTREE_RECT.y + 37.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		l
	);
	_skills.push_back(lu);

	SkillNode* luu = new SkillNode(
		"Another half ogre",
		{ { Stats::HP, 75.f } },
		{ SKILLTREE_RECT.x + 55.f, SKILLTREE_RECT.y + 11.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		lu
	);
	_skills.push_back(luu);

	SkillNode* lul = new SkillNode(
		"Vampiric breed",
		{ { Stats::LIFESTEAL, .1f } },
		{ SKILLTREE_RECT.x + 29.f, SKILLTREE_RECT.y + 37.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		lu
	);
	_skills.push_back(lul);

	SkillNode* ll = new SkillNode(
		"Dungeon fashion",
		{  },
		{ SKILLTREE_RECT.x + 29.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		[=](Stats::stat_t _) { player->unlockSlot(Item::ARMOR); },
		l,
		"Unlocks the Armor slot."
	);
	_skills.push_back(ll);

	SkillNode* lll = new SkillNode(
		"Natural iron skin",
		{ { Stats::ARMOR, .1f } },
		{ SKILLTREE_RECT.x + 3.f, SKILLTREE_RECT.y + 63.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		ll
	);
	_skills.push_back(lll);

	SkillNode* ld = new SkillNode(
		"Field nurse",
		{ { Stats::REGEN, 2.f / Consts::TARGET_FPS } },
		{ SKILLTREE_RECT.x + 55.f, SKILLTREE_RECT.y + 89.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		l
	);
	_skills.push_back(ld);

	SkillNode* ldl = new SkillNode(
		"Unnatural iron skin",
		{ { Stats::REGEN, 4.f / Consts::TARGET_FPS } },
		{ SKILLTREE_RECT.x + 29.f, SKILLTREE_RECT.y + 89.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		ld
	);
	_skills.push_back(ldl);

	SkillNode* ldd = new SkillNode(
		"Faster than furious",
		{ { Stats::SPD, .1f } },
		{ SKILLTREE_RECT.x + 55.f, SKILLTREE_RECT.y + 115.f, 13.f, 13.f },
		std::bind(&Player::addSkillStat, player, std::placeholders::_1),
		ld
	);
	_skills.push_back(ldd);
}
