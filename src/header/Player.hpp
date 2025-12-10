#pragma once

#include "Entity.hpp"
#include "Consts.hpp"
#include "Utils.hpp"
#include "ResourceManager.hpp"
#include "Item.hpp"
#include "Door.hpp"
#include "Button.hpp"

#include <functional>
#include <string>

class Player : public Entity {
public:
	Player();
	~Player();

	int getInvState() const;
	struct LookData { SDL_FPoint from; float a; } getLookData();
	Stats::stat_t getStats() const;
	int getLevel() const;

	void openInventory();
	void switchSkillTree();
	void mouseDown();
	void mouseUp();

	bool update() override;
	void render() override;
	void renderInv() const;

	void useRing();

	void pickUpItem();

	void equipmentChange(bool equip, Item* item);
	void addSkillStat(Stats::stat_t stat);
	void unlockSlot(Item::Type type);

	void heal(float amount);
	void takeDamage(float amount);
	void dealDamage(float amount);
	void addXP(int amount);
	void moveTo(Door::DoorType from);

	ObjectTypes getType() override { return PLAYER; }
protected:
	void handleCollision(Object* collidedWith) override;

private:
	class InventoryManager {
	public:
		class ItemSlot {
		public:
			ItemSlot(const SDL_FPoint& pos, Player* player = nullptr, std::function<bool(Item*)> filter = [](Item* _) {return true;}, bool locked=false);
			~ItemSlot();

			Item* getItem() const;
			Item* swapItem(Item* item);

			void unlock();
			void render() const;
			void renderDescr() const;
		private:
			static ResourceManager::StaticSprite* LOCK_SPRITE;

			Player* _player;
			SDL_FPoint _pos;
			Item* _item;
			std::function<bool(Item*)> _filter;
			bool _locked = false;

			void renderLock() const;
		};

		InventoryManager(Player* player, ResourceManager::TextSprite** statSprite);
		InventoryManager(const InventoryManager& other) = delete;
		InventoryManager operator=(const InventoryManager& other) = delete;
		~InventoryManager();

		Item* operator[](Item::Type);

		SDL_FPoint getStatPoint();

		ItemSlot* getFreeSlot() const;

		void mouseDown();
		void mouseUp();

		void setAttack(bool attack);
		void unlockSlot(Item::Type type);

		void update();
		void render() const;

	private:
		static constexpr int INV_COLS = 3;
		static constexpr int INV_ROWS = 4;
		static const SDL_FRect INV_SPRITE_RECT;
		static const SDL_FRect INV_RECT;
		static const SDL_FRect EQ_RECT;
		static const SDL_FRect SKILLTREE_BTN_RECT;
		static SDL_FPoint STAT_POS;

		ResourceManager::StaticSprite* _invSprite;
		ResourceManager::TextSprite** _statsSprite;

		ItemSlot* _inv[INV_COLS * INV_ROWS];
		std::map<Item::Type, ItemSlot*> _equippedItems;
		Button _skillTreeBtn;

		Item* _draggedItem = nullptr;
		ItemSlot* _dragSource = nullptr;

		bool _attack = false;

		void startDrag(ItemSlot* src);
		void endDrag(ItemSlot* dest);

		ItemSlot* getInvItemSlotUnderCursor() const;
		ItemSlot* getEqItemSlotUnderCursor() const;
	};

	class SkillTree {
	public:
		SkillTree(Player* player);
		~SkillTree();

		void render() const;
		void levelUp(int level);

		void mouseDown();
		void mouseUp();

		void renderLevel(SDL_FRect dest) const;
	private:
		class SkillNode {
		public:
			SkillNode(std::string name, Stats::stat_t stat, SDL_FRect rect, std::function<void(Stats::stat_t)> addStats, SkillNode* parent = nullptr, std::string extraDescr="");
			~SkillNode();
			
			bool isUnlocked() const;

			void render(bool hasPoints) const;
			void renderDescr(SDL_FPoint mousePos);

			bool handleClick();
		private:
			Stats::stat_t _stats;
			std::function<void(Stats::stat_t)> _addStats;
			bool _unlocked = false;
			SDL_FRect _rect;
			SkillNode* _parent;

			ResourceManager::TextSprite* _descriptionSprite;

			void createDescriptionSprite(std::string name, std::string extraDescr);
			bool unlockable() const;
		};

		static const SDL_FRect SKILLTREE_RECT;
		static const SDL_FRect SKILLPOINTS_RECT;
		static const SDL_FRect LEVEL_RECT;
		static const SDL_FRect BACK_BTN_RECT;

		static ResourceManager::TextSprite* createNumberSprite(int value);

		int _skillPoints = 0;
		Button _backBtn;
		std::vector<SkillNode*> _skills;

		ResourceManager::StaticSprite* _skillTreeSprite;
		ResourceManager::TextSprite* _skillPointsSprite;
		ResourceManager::TextSprite* _LevelSprite;

		void updateSkillPointsSprite();
		void createSkills(Player* player);
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

	static int getXpNeeded(int level);

	float _health;
	int _lvl;
	int _xp = 0;
	int _xpNeeded;

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
	SkillTree _skillTree;

	ResourceManager::TextSprite* _statsSprite;

	ResourceManager::TextSprite* createStatsSprite();

	void pMove();
	void levelUp();

	void renderHUD();
	void renderInventory() const;
	void renderSkillTree() const;
};