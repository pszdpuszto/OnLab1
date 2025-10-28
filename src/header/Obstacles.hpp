#pragma once

#include "Object.hpp"

class Barrel : public Object
{
public: Barrel(float x, float y, float size = 16.f) : Object("barrel", x, y, size, size) {}
};

class Rock : public Object
{
public: Rock(float x, float y, float size = 24.f) : Object("rock", x, y, size, size) {}
};

