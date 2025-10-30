#include "../header/ResourceManager.hpp"

#include  <SDL3/SDL.h>
#include <fstream>

#include "../header/Game.hpp"

const Utils::RGB ResourceManager::COLOR_KEY = { 0xff, 0x00, 0xff };
const std::string ResourceManager::ASSET_PATH = "Assets/";

ResourceManager::ResourceManager(SDL_Renderer* renderer, const std::string& font)
	: _renderer{ renderer },
	_fontPath{ ASSET_PATH + font + ".ttf" } {

	_simpleTextures["404"] = create404Texture();
	loadItemSpriteIds();
};

ResourceManager::~ResourceManager()
{
	if (SDL_WasInit(SDL_INIT_VIDEO)) {
		for (auto& item : _objectTextures) {
			SDL_DestroyTexture(item.second._texture);
		}
		for (auto& item : _simpleTextures) {
			SDL_DestroyTexture(item.second);
		}
	}
	if (TTF_WasInit()) {
		for (auto& item : _fonts) {
			TTF_CloseFont(item.second);
		}
	}
}

const ResourceManager::ObjectTextureData& ResourceManager::getObjectTextureData(const std::string& textureName)
{
	if (_objectTextures.find(textureName) != _objectTextures.end()) {
		return _objectTextures[textureName];
	}

	SDL_Texture* _texture = loadTexture(textureName);
	if (_texture == nullptr) {
		SDL_Log("Using 404 texture for %s", textureName.c_str());
		return _objectTextures[textureName] = { _simpleTextures["404"], {} };
	}
	ObjectTextureData textureData{ _texture };
	loadTextureData(textureName, textureData);

	return _objectTextures[textureName] = textureData;
}

SDL_Texture* ResourceManager::getTexture(const std::string& textureName)
{
	if (_simpleTextures.find(textureName) != _simpleTextures.end()) {
		return _simpleTextures[textureName];
	}
	SDL_Texture* _texture = loadTexture(textureName);
	if (_texture == nullptr) {
		SDL_Log("Using 404 texture for %s", textureName.c_str());
		return _simpleTextures[textureName] = _simpleTextures["404"];
	}
	return _simpleTextures[textureName] = _texture;
}

SDL_FRect ResourceManager::getItemSpriteSrcRect(const std::string& itemName)
{
	SDL_Texture* itemTexture = getTexture("items");
	int cols = itemTexture->w / 16;
	int spriteId = _itemSpriteIds[itemName];
	return SDL_FRect{
		static_cast<float>((spriteId % cols) * 16),
		static_cast<float>((spriteId / cols) * 16),
		16.f,
		16.f
	};
}

ResourceManager::TextSprite* ResourceManager::createTextSprite(const std::string& text, const SDL_Color& bgColor)
{
	static const std::string delim = "$";
	std::vector<SDL_Surface*> lines;
	SDL_Color color = { 0xff, 0xff, 0xff, 0xff }; // default white
	float fontSize = 16.f; // default size
	std::string processedText = text;
	bool open = false;
	size_t delimPos = 0;
	while (delimPos != std::string::npos) {
		delimPos = processedText.find(delim);
		std::string before = delimPos != std::string::npos ? processedText.substr(0, delimPos) : processedText;
		
		if (open) {
			switch (before[0]) {
			case 'N':
				// new line
				break;
			case 'S':
				fontSize = std::stof(before.substr(1));
				break;
			case 'C': {
				size_t firstComma = before.find(',', 1);
				size_t secondComma = before.find(',', firstComma + 1);
				if (firstComma != std::string::npos && secondComma != std::string::npos) {
					int r = std::stoi(before.substr(1, firstComma));
					int g = std::stoi(before.substr(firstComma + 1, secondComma));
					int b = std::stoi(before.substr(secondComma + 1));
					color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 0xff };
				}
			}
				break;
			default:
				break;
			}
			open = false;
		} else {
			open = true;
			if (!before.empty()) {
				SDL_Surface* lineSurface = TTF_RenderText_Blended(getFont(fontSize), before.c_str(), NULL, color);

				lines.push_back(lineSurface);
			}
		}
		processedText = processedText.substr(delimPos + 1);
	}
	static const int PADDING = 2;
	int totalHeight = PADDING;
	for (SDL_Surface* line : lines) {
		totalHeight += line->h + PADDING;
	}
	int width = 0;
	for (SDL_Surface* line : lines) {
		if (line->w > width) {
			width = line->w;
		}
	}
	width += PADDING * 2;
	SDL_Surface* fullSurface = SDL_CreateSurface(width, totalHeight, SDL_PIXELFORMAT_RGBA8888);
	SDL_FillSurfaceRect(fullSurface, nullptr, SDL_MapRGBA(SDL_GetPixelFormatDetails(fullSurface->format), nullptr, bgColor.r, bgColor.g, bgColor.b, bgColor.a));
	int currentY = PADDING;
	for (SDL_Surface* line : lines) {
		SDL_Rect _destRect = { PADDING, currentY, line->w, line->h };
		SDL_BlitSurface(line, nullptr, fullSurface, &_destRect);
		currentY += line->h + PADDING;
		SDL_DestroySurface(line);
	}

	SDL_Texture* _texture = SDL_CreateTextureFromSurface(_renderer, fullSurface);
	SDL_DestroySurface(fullSurface);
	SDL_SetTextureScaleMode(_texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

	return new TextSprite{
		_renderer,
		_texture,
		{ 0.f, 0.f, static_cast<float>(_texture->w), static_cast<float>(_texture->h) }
	};
}

SDL_Texture* ResourceManager::create404Texture()
{
	SDL_Texture* texture404 = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 2, 2);
	SDL_SetRenderTarget(_renderer, texture404);
	SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(_renderer);
	SDL_SetRenderDrawColor(_renderer, 0xff, 0x00, 0xff, 0xff);
	SDL_RenderLine(_renderer, 0, 0, 2, 2);
	SDL_SetRenderTarget(_renderer, nullptr);

	SDL_SetTextureScaleMode(texture404, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

	return texture404;
}

void ResourceManager::loadItemSpriteIds()
{
	std::ifstream file{ ASSET_PATH + "items.fdf" };
	if (!file.is_open()) {
		SDL_Log("Failed to open item_sprites.ids: %s", SDL_GetError());
		return;
	}
	/**
	* File format:
	* <item_name_id_0>
	* <item_name_id_1>
	* ...
	* <item_name_id_n>
	*/
	std::string name;
	int id = 0;
	while (std::getline(file, name)) {
		_itemSpriteIds[name] = id++;
	}
}

SDL_Texture* ResourceManager::loadTexture(const std::string& textureName)
{

	std::string path = ASSET_PATH + textureName + ".bmp";
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

	SDL_Texture* _texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_DestroySurface(surface);

	if (_texture == nullptr) {
		SDL_Log("Failed to create texture from surface for %s: %s", path.c_str(), SDL_GetError());
		return nullptr;
	}
	SDL_SetTextureScaleMode(_texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
	return _texture;
}

bool ResourceManager::loadTextureData(const std::string& textureName, ObjectTextureData& textureData)
{
	std::ifstream file{ ASSET_PATH + textureName + ".fdf" };
	if (!file.is_open()) { // not a fatal error, just use full texture
		textureData.width = textureData._texture->w;
		textureData.height = textureData._texture->h;
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

TTF_Font* ResourceManager::getFont(float fontSize)
{
	if (_fonts.find(fontSize) != _fonts.end()) {
		return _fonts[fontSize];
	}
	TTF_Font* font = TTF_OpenFont(_fontPath.c_str(), fontSize);
	return _fonts[fontSize] = font;
}

ResourceManager::StaticSprite::StaticSprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& destRect)
	: _renderer{ renderer },
	_texture{ texture }, 
	_destRect{ destRect } {}

void ResourceManager::StaticSprite::setPos(float x, float y)
{
	_destRect.x = x;
	_destRect.y = y;
}

void ResourceManager::StaticSprite::render() const
{
	SDL_RenderTexture(_renderer, _texture, nullptr, &_destRect);
}

ResourceManager::TextSprite::~TextSprite()
{
	Utils::destroyTexture(_texture);
}

void ResourceManager::TextSprite::setPosFitToWindow(float x, float y)
{
	StaticSprite::setPos(x, y);

	if (_destRect.x < 0)
		_destRect.x = 0;
	if (_destRect.x + _destRect.w > Consts::WINDOW_WIDTH)
		_destRect.x = Consts::WINDOW_WIDTH - _destRect.w;
	if (_destRect.y < 0)
		_destRect.y = 0;
	if (_destRect.y + _destRect.h > Consts::WINDOW_HEIGHT)
		_destRect.y = Consts::WINDOW_HEIGHT - _destRect.h;

}

ResourceManager::TextSprite::TextSprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& destRect) 
	: StaticSprite(renderer, texture, destRect) {}
