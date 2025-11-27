#pragma once

#include "Entity.hpp"
#include "Consts.hpp"
#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "Item.hpp"
#include "Door.hpp"

#include <functional>

class Player : public Entity {
public:
	Player();
	~Player();

	int getInvState() const;
	struct LookData { SDL_FPoint from; float a; } getLookData();

	void openInventory();
	void mouseDown();
	void mouseUp();

	bool update() override;
	void render() override;
	void renderInv() const;

	void useRing();

	void equipmentChange(bool equip, Item* item);

	void heal(float amount);
	void takeDamage(float amount);
	void moveTo(Door::DoorType from);

	ObjectTypes getType() override { return PLAYER; }
protected:
	void handleCollision(Object* collidedWith) override;

private:
	class InventoryManager {
	public:
		InventoryManager(Player* player, ResourceManager::TextSprite** statSprite);
		InventoryManager(const InventoryManager& other) = delete;
		InventoryManager operator=(const InventoryManager& other) = delete;
		~InventoryManager();

		Item* operator[](Item::Type);

		SDL_FPoint getStatPoint();

		void mouseDown();
		void mouseUp();

		void setAttack(bool attack);

		void update();
		void render() const;

	private:
		class ItemSlot {
		public:
			ItemSlot(const Utils::floatPoint& pos, Player* player = nullptr, std::function<bool(Item*)> filter = [](Item* _) {return true;});
			~ItemSlot();

			Item* getItem() const;
			Item* swapItem(Item* item);


			void render() const;
			void renderDescr() const;
		private:
			Player* _player;
			Utils::floatPoint _pos;
			Item* _item;
			std::function<bool(Item*)> _filter;
		};
		static constexpr int INV_COLS = 3;
		static constexpr int INV_ROWS = 4;
		static const SDL_FRect INV_SPRITE_RECT;
		static const SDL_FRect INV_RECT;
		static const SDL_FRect EQ_RECT;
		static Utils::floatPoint STAT_POS;

		ResourceManager::StaticSprite* _invSprite;
		ResourceManager::TextSprite** _statsSprite;

		ItemSlot* _inv[INV_COLS * INV_ROWS];
		std::map<Item::Type, ItemSlot*> _equippedItems;

		Item* _draggedItem = nullptr;
		ItemSlot* _dragSource = nullptr;

		bool _attack = false;

		void startDrag(ItemSlot* src);
		void endDrag(ItemSlot* dest);

		ItemSlot* getInvItemSlotUnderCursor() const;
		ItemSlot* getEqItemSlotUnderCursor() const;
	};
	enum playerState {
		IDLE = 0,
		MOVING
	};
	enum invState {
		CLOSED = 0,
		INVENTORY,
		SKILLTREE
	} _invState = CLOSED;

	float _health;

	Stats::stat_t _stats = {
		{ Stats::SPD, 1.f },
		{ Stats::HP, 200.f },
		{ Stats::REGEN, 0.f },
		{ Stats::ARMOR, 0.f },
		{ Stats::DMG, 1.f },
		{ Stats::ATK_SPD, 1.f },
		{ Stats::LIFESTEAL, 0.f }
	};

	InventoryManager _invMgr;

	ResourceManager::TextSprite* _statsSprite;

	ResourceManager::TextSprite* createStatsSprite();


	void pMove();

	void renderHUD();
	void renderInventory() const;
	void renderSkillTree() const;
};