#pragma once

//#define MIN (x, y) ((x) < (y) ? (x) : (y))

namespace Utils {

	struct RGB {
		Uint8 r;
		Uint8 g;
		Uint8 b;
	};

	struct intPoint {
		int x;
		int y;
	};

	struct floatPoint {
		float x;
		float y;
	};

	enum WASD {
		W = 0b0001,
		S = 0b0010,
		A = 0b0100,
		D = 0b1000
	};

	typedef unsigned char WASDState;

	static bool isColliding(double x1, double y1, double w1, double h1,
		double x2, double y2, double w2, double h2) {
		return (x1 < x2 + w2 &&
			x1 + w1 > x2 &&
			y1 < y2 + h2 &&
			y1 + h1 > y2);
	}
	static bool isColliding(SDL_FRect rect1, SDL_FRect rect2) {
		return isColliding(rect1.x, rect1.y, rect1.w, rect1.h,
			rect2.x, rect2.y, rect2.w, rect2.h);
	}

};