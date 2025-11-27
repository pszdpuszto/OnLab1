#pragma once

#include <vector>
#include <map>

#include "object.hpp"
#include "entity.hpp"
#include "door.hpp"

class Room {
public:
	Room();
	void initDoors(std::map<Door::DoorType, Room*> doors);
	~Room();
	
	std::vector<Object*> getObjects() const { 
		return _objects; 
	};

	void addObject(Object* obj);
	void enemyDead();

	void update();
	void render() const;

protected:
private:
	std::vector<Object*> _objects = std::vector<Object*>();
	int _enemyCount = 0;
	bool _isCleared = false;
};