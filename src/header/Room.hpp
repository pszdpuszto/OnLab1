#pragma once

#include <vector>

#include "object.hpp"
#include "entity.hpp"

class Room {
public:
	Room();
	~Room();
	
	std::vector<Object*> getObjects() const { 
		return _objects; 
	};

	void update();
	void render() const;

protected:
	std::vector<Object*> _objects = std::vector<Object*>();
	std::vector<Entity*> _entities = std::vector<Entity*>();
};