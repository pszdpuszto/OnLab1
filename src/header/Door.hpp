#pragma once
#include "Object.hpp"

#include <map>

class Room;

class Door : public Object
{
public: 
	enum DoorType {
		NORTH = 0,
		SOUTH,
		EAST,
		WEST
	};
	enum state_t {
		CLOSED = 0,
		OPEN = 1
	};
	Door(DoorType type, Room* to, state_t state=CLOSED);


	bool isOpen() const;
	void unlock();
	Room* getToRoom() const;
	DoorType getDoorType() const;

	void render() override;

	virtual ObjectTypes getType() { return DOOR; }
private:
	static std::map<DoorType, SDL_FRect> RECTS;
	static std::map<DoorType, int> ROTS;
	static std::map<DoorType, SDL_FPoint> NEW_P_POS;
	DoorType _type;
	state_t _state;
	Room* _to;
};

