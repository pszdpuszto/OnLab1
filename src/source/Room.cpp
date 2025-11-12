#include "../header/Room.hpp"

Room::Room()
{
}

Room::~Room()
{
	for (Object* obj : _objects) {
		delete obj;
	}
}

void Room::addObject(Object* obj)
{
	_objects.push_back(obj);
}

void Room::update()
{
	for (int i = 0; i < _objects.size();) {
		if (_objects[i]->update()) {
			delete _objects[i];
			_objects.erase(std::next(_objects.begin(), i));
		}
		else {
			i++;
		}
	}
}

void Room::render() const
{
	for (Object* obj : _objects) {
		obj->render();
	}
}
