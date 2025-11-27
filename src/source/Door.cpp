#include "../header/Door.hpp"
#include "../header/Consts.hpp"

std::map<Door::DoorType, SDL_FRect> Door::RECTS = {
	{ NORTH, { Consts::WINDOW_WIDTH / 2.f - 9.f, 1.f, 19.f, 20.f } },
	{ SOUTH, { Consts::WINDOW_WIDTH / 2.f - 9.f, Consts::WINDOW_HEIGHT - 21.f, 19.f, 20.f } },
	{ EAST, { Consts::WINDOW_WIDTH - 21.f, Consts::WINDOW_HEIGHT / 2.f - 9.f, 20.f, 19.f } },
	{ WEST, { 1.f, Consts::WINDOW_HEIGHT / 2.f - 9.f, 20.f, 19.f } }
};

std::map<Door::DoorType, int> Door::ROTS = {
	{ NORTH, 0 },
	{ SOUTH, 180 },
	{ EAST, 90 },
	{ WEST, 270 }
};

std::map<Door::DoorType, SDL_FPoint> Door::NEW_P_POS = {
	{ NORTH, { Consts::WINDOW_WIDTH / 2.f - 7.f, Consts::WINDOW_HEIGHT - Consts::ROOM_WALL_THICKNESS - 25.f } },
	{ SOUTH, { Consts::WINDOW_WIDTH / 2.f - 7.f, Consts::ROOM_WALL_THICKNESS + 5.f } },
	{ EAST, { Consts::ROOM_WALL_THICKNESS + 5.f, Consts::WINDOW_HEIGHT / 2.f - 7.f } },
	{ WEST, { Consts::WINDOW_WIDTH - Consts::ROOM_WALL_THICKNESS - 25.f, Consts::WINDOW_HEIGHT / 2.f - 7.f } }
};

Door::Door(DoorType type, Room* to, state_t state)
	: Object("door", RECTS[type].x, RECTS[type].y, RECTS[type].w, RECTS[type].h),
	_type{ type },
	_to{ to },
	_state{ state } {}

void Door::render()
{
	_sprite.render(_rect, SDL_FLIP_NONE, ROTS[_type]);
}

bool Door::isOpen() const
{
	return _state == OPEN;
}

void Door::unlock()
{
	_state = OPEN;
	_sprite.setState(OPEN);
}

Room* Door::getToRoom() const
{
	return _to;
}

Door::DoorType Door::getDoorType() const
{
	return _type;
}
