#include "../header/Room.hpp"
#include "../header/Door.hpp"

#include "../header/Game.hpp"

Room::Room()
{}

void Room::initDoors(std::map<Door::DoorType, Room*> doors)
{
	for (std::pair<Door::DoorType, Room*> i : doors) {
		addObject(new Door(i.first, i.second));
	}
}

Room::~Room()
{
	for (Object* obj : _objects) {
		delete obj;
	}
}

void Room::addObject(Object* obj)
{
	if (obj->getType() == Object::ENEMY) _enemyCount++;
	_objects.push_back(obj);
}

void Room::enemyDead(Enemy* enemy)
{
	_enemyCount--;
	GAME->getPlayer()->addXP(enemy->getXp());
}

void Room::update()
{
	for (int i = 0; i < _objects.size();) {
		if (_objects[i]->update()) {
			if (_objects[i]->getType() == Object::ENEMY) _enemyCount--;
			delete _objects[i];
			_objects.erase(std::next(_objects.begin(), i));
		}
		else {
			i++;
		}
	}
	if (!_isCleared && _enemyCount == 0) {
		SDL_Log("Enemies left in room: %d", _enemyCount);
		_isCleared = true;
		for (Object* obj : _objects) {
			if (obj->getType() == Object::DOOR) {
				static_cast<Door*>(obj)->unlock();
			}
		}
	}
}

void Room::render() const
{
	for (Object* obj : _objects) {
		obj->render();
	}
}
