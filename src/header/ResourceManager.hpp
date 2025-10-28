#pragma once

#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <vector>
#include "Utils.hpp"

class ResourceManager
{
public:
	struct TextureData {
		SDL_Texture* texture;
		int width;
		int height;
		std::vector<int> frameCount;
	};

	ResourceManager(SDL_Renderer* renderer);
	~ResourceManager();

	const TextureData& getObjectTextureData(const std::string& textureName);
	SDL_Texture* getSimpleTexture(const std::string& textureName);
private:
	static Utils::RGB COLOR_KEY;

	SDL_Renderer* _renderer;
	std::map<std::string, TextureData> _objectTextures;
	std::map<std::string, SDL_Texture*> _simpleTextures;

	SDL_Texture* loadTexture(const std::string& textureName);
	bool loadTextureData(const std::string& textureName, TextureData& textureData);
};

