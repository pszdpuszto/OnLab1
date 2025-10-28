#include "../header/Room.hpp"

Room::Room()
{
}

Room::~Room()
{
	for (Object* obj : _objects) {
		delete obj;
	}
	for (Entity* ent : _entities) {
		delete ent;
	}
}

void Room::update()
{
	for (Entity* ent : _entities) {
		ent->update();
	}
}

void Room::render() const
{
	for (Object* obj : _objects) {
		obj->render();
	}
}
