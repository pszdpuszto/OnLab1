#include "../header/DroppedItem.hpp"

#include <cmath>

#include "../header/Consts.hpp"
#include "../header/Game.hpp"

DroppedItem::DroppedItem(Item* item, float x, float y) 
	: Object("itemShadow", x, y, 16.f, 20.f, false),
	_item{ item },
	_count{ 0 },
	_pickupText{ GAME->createTextSprite("$S8$$C0,255,0$Press Q", {0x00, 0x00, 0x00, 0x00})} { }

DroppedItem::DroppedItem(Item* item)
	: DroppedItem{ item, GAME->getPlayer()->getRect().x + GAME->getPlayer()->getRect().w / 2.f - 8.f, GAME->getPlayer()->getRect().y + GAME->getPlayer()->getRect().h / 2.f - 8.f }
{
}

DroppedItem::~DroppedItem()
{
	if (_item)
		delete _item;
	delete _pickupText;
}

bool DroppedItem::update()
{
	Object::update();
	_count = (_count + 1) % (Consts::TARGET_FPS * 2);
	GAME->closestPickupableItemCandidate(this);
	return !_item;
}

void DroppedItem::render()
{
	Object::render();
	_item->render({ _rect.x - 2.f, _rect.y + sinf(static_cast<float>(_count) / Consts::TARGET_FPS * Consts::PI) * 2.f}, true);
	if (GAME->getClosestPickupableItem() == this) {
		_pickupText->setPos({
			_rect.x + _rect.w / 2.f - _pickupText->getRect().w / 2.f,
			_rect.y - 12.f
			});
		_pickupText->render();
	}
}

Item* DroppedItem::pickup()
{
	Item* back = _item;
	_item = nullptr;
	return back;
}

