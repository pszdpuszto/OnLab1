#pragma once

#include "Consts.hpp"

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

	static bool isColliding(const SDL_FPoint* rect1, SDL_FRect rect2) {
		static auto ccw = [](SDL_FPoint A, SDL_FPoint B, SDL_FPoint C) {
			return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
		};

		static auto intersect = [](SDL_FPoint A, SDL_FPoint B, SDL_FPoint C, SDL_FPoint D) {
			return (ccw(A, C, D) != ccw(B, C, D)) && (ccw(A, B, C) != ccw(A, B, D));
		};

		SDL_FPoint rect2Points[4] = {
			{ rect2.x, rect2.y },
			{ rect2.x + rect2.w, rect2.y },
			{ rect2.x + rect2.w, rect2.y + rect2.h },
			{ rect2.x, rect2.y + rect2.h }
		};

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				//SDL_Log("Checking edge (%f, %f) - (%f, %f) and (%f, %f) - (%f, %f)", rect1[i].x, rect1[i].y, rect1[(i + 1) % 4].x, rect1[(i + 1) % 4].y, rect2Points[j].x, rect2Points[j].y, rect2Points[(j + 1) % 4].x, rect2Points[(j + 1) % 4].y);
				if (intersect(
					rect1[i],
					rect1[(i + 1) % 4],
					rect2Points[j],
					rect2Points[(j + 1) % 4]
				)) {
					return true;
				}
			}
		}
		return false;
	}

	static float dist(SDL_FPoint p1, SDL_FPoint p2) {
		return sqrtf((p2.x - p1.x) * (p2.x - p1.x) +
			(p2.y - p1.y) * (p2.y - p1.y));
	}

	static inline void destroyTexture(SDL_Texture* _texture) {
		if (_texture != nullptr && SDL_WasInit(SDL_INIT_VIDEO)) {
			SDL_DestroyTexture(_texture);
		}
	}

	static float RadToDeg(float rad) {
		return rad / Consts::PI * 180.f;
	}

	static float DegToRad(float deg) {
		return deg / 180.f * Consts::PI;
	}
};