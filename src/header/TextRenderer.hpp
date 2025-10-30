#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

#include <string>
#include <map>

static class TextRenderer
{
public:
	static void setFont(const std::string& font);
	static void clearFonts();
private:
	static std::string _fontPath;
	static std::map<int, TTF_Font*> _fonts;
};

