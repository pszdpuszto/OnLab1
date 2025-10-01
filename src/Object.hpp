#pragma once

class Object {
protected:
	double _x, _y;
	double _width, _height;
public:
	Object(double x, double y, double w, double h);

	double getX() const;
	double getY() const;

	void render() const;
	bool isCollidingWith(const Object& other) const;
};