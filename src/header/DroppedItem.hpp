#pragma once
#include "Object.hpp"
#include "Item.hpp"
#include "ResourceManager.hpp"

class DroppedItem : public Object
{
public:
	DroppedItem(Item* item, float x, float y);
	DroppedItem(Item* item);
	~DroppedItem();

	bool update() override;
	void render() override;

	Item* pickup();
protected:
	Item* _item;
	uint16_t _count;
	ResourceManager::TextSprite* _pickupText = nullptr;
};

