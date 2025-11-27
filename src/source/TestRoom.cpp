#include "../header/TestRoom.hpp"

#include "../header/Obstacles.hpp"
#include "../header/Enemy.hpp"

TestRoom::TestRoom() : Room()
{
	addObject(new Rock(50, 100));
	addObject(new Barrel(100, 20));
	addObject(new Goblin(20, 100));
	addObject(new Orc(200, 100));
	addObject(new Thief(20,120));
	addObject(new SpiderMother(200, 150));
	addObject(new GoblinThrower(150, 150));
}