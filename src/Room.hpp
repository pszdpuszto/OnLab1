#pragma once

#include <vector>;

#include "object.hpp";
#include "entity.hpp"

class Room {
protected:
	std::vector<Object*> _objects = std::vector<Object*>();
	std::vector<Entity*> _entities = std::vector<Entity*>();

public:
	Room();
	~Room();
	
	std::vector<Object*> getObjects() const { 
		return _objects; 
	};

	void update();
	void render() const;

};