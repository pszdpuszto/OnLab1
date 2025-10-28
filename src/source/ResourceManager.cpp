
#include "../header/ResourceManager.hpp"

#include  <SDL3/SDL.h>
#include <fstream>

#include "../header/Game.hpp"

Utils::RGB ResourceManager::COLOR_KEY = { 0xff, 0x00, 0xff };

const ResourceManager::TextureData& ResourceManager::getObjectTextureData(const std::string& textureName)
{
	if (_objectTextures.find(textureName) != _objectTextures.end()) {
		return _objectTextures[textureName];
	}

	SDL_Texture* texture = loadTexture(textureName);
	if (texture == nullptr) {
		SDL_Log("Using 404 texture for %s", textureName.c_str());
		return _objectTextures[textureName] = { _simpleTextures["404"], {} };
	}
	TextureData textureData{ texture };
	loadTextureData(textureName, textureData);

	return _objectTextures[textureName] = textureData;
}

SDL_Texture* ResourceManager::getSimpleTexture(const std::string& textureName)
{
	if (_simpleTextures.find(textureName) != _simpleTextures.end()) {
		return _simpleTextures[textureName];
	}
	SDL_Texture* texture = loadTexture(textureName);
	if (texture == nullptr) {
		SDL_Log("Using 404 texture for %s", textureName.c_str());
		return _simpleTextures[textureName] = _simpleTextures["404"];
	}
	return _simpleTextures[textureName] = texture;
}

SDL_Texture* ResourceManager::loadTexture(const std::string& textureName)
{

	std::string path = "Assets/" + textureName + ".bmp";
	SDL_Surface* surface = SDL_LoadBMP(path.c_str());
	if (surface == nullptr) {
		SDL_Log("Failed to load surface for texture %s: %s", path.c_str(), SDL_GetError());
		return nullptr;
	}

	SDL_SetSurfaceColorKey(
		surface, 
		true, 
		SDL_MapRGB(
			SDL_GetPixelFormatDetails(surface->format), 
			SDL_GetSurfacePalette(surface), 
			COLOR_KEY.r, COLOR_KEY.g, COLOR_KEY.b)
		);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_DestroySurface(surface);

	if (texture == nullptr) {
		SDL_Log("Failed to create texture from surface for %s: %s", path.c_str(), SDL_GetError());
		return nullptr;
	}
	SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
	return texture;
}

bool ResourceManager::loadTextureData(const std::string& textureName, TextureData& textureData)
{
	std::ifstream file{ "Assets/" + textureName + ".fdf" };
	if (!file.is_open()) { // not a fatal error, just use full texture
		textureData.width = textureData.texture->w;
		textureData.height = textureData.texture->h;
		return false;
	}
	std::vector<int> frameCounts;
	int i;
	/**
	 * File format:
	 * <width> <height>
	 * <frame_count_state_0> <frame_count_state_0> ... <frame_count_state_n>
	*/

	file >> textureData.width >> textureData.height;
	while (file >> i) {
		frameCounts.push_back(i);
	}
	textureData.frameCount = frameCounts;
	return true;
}

ResourceManager::ResourceManager(SDL_Renderer* renderer) : _renderer{ renderer } {

	SDL_Texture* texture404 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);
	SDL_SetRenderTarget(renderer, texture404);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
	SDL_RenderLine(renderer, 0, 0, 2, 2);
	SDL_SetRenderTarget(renderer, nullptr);

	SDL_SetTextureScaleMode(texture404, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
	_simpleTextures["404"] = texture404;
};

ResourceManager::~ResourceManager()
{
	for (auto& item : _objectTextures) {
		SDL_DestroyTexture(item.second.texture);
	}
}
