#pragma once

#include <SDL3/SDL.h>

namespace Consts {
	constexpr float PI = 3.14159265359f;

	constexpr const char* WINDOW_TITLE = "Game Window";
	constexpr float GLOBAL_SPEED = 1.f;
	constexpr int TARGET_FPS = 60;
	constexpr int ANIMATION_FPS = 12;

	constexpr int ROOM_WALL_THICKNESS = 20;
	constexpr int WINDOW_WIDTH = 400;
	constexpr int WINDOW_HEIGHT = 225;

	const SDL_FRect ROOM_RECT {
		ROOM_WALL_THICKNESS, 
		ROOM_WALL_THICKNESS,
		WINDOW_WIDTH - 2 * ROOM_WALL_THICKNESS, 
		WINDOW_HEIGHT - 2 * ROOM_WALL_THICKNESS
	};
}