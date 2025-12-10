#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <map>
#include <string>
#include <vector>
#include "Utils.hpp"

class ResourceManager
{
public:
	/**
	* Spritesheet data. The texture itself is managed by ResourceManager.
	*/
	struct ObjectTextureData {
		SDL_Texture* _texture;
		int width;
		int height;
		std::vector<int> frameCount;
	};

	class StaticSprite {
	public:
		StaticSprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& destRect);

		void setPos(float x, float y);
		void setPos(SDL_FPoint point);

		void render() const;
	protected:
		SDL_Renderer* _renderer;
		SDL_Texture* _texture; // Managed by ResourceManager
		SDL_FRect _destRect;

	};

	class TextSprite : public StaticSprite {
	public:
		TextSprite(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_FRect& destRect);
		TextSprite(const TextSprite& other) = delete;
		TextSprite operator=(const TextSprite& other) = delete;
		~TextSprite(); // _texture isn't managed by ResourceManager

		SDL_FRect getRect() const;

		void setPosFitToWindow(float x, float y);
	};

	ResourceManager(SDL_Renderer* renderer, const std::string& font);
	~ResourceManager();

	const ObjectTextureData& getObjectTextureData(const std::string& textureName);
	SDL_Texture* getTexture(const std::string& textureName);
	SDL_FRect getItemSpriteSrcRect(const std::string& itemName);
	SDL_FRect getWeaponSpriteSrcRect(const std::string& weaponName, float width, float height);

	/**
	* Creates an SDL_Texture from a string using the game's font.
	* @param text The string to create a texture from. It can contain multiple lines separated by '$N$'. The font size can be specified by using the tag
	* $S<size>$ before the text, where <size> is an integer representing the font size. This also represents a new line.
	* For example, "$S24$Hello" will render "Hello" with a font size of 24.
	* The color of the text can similarly be specified using the tag $C<r>,<g>,<b>$, where <r>, <g>, and <b> are integers between 0 and 255
	* representing the red, green, and blue components of the color. This is also treated as a new line.
	* Color and size changes affect all subsequent text until changed again and can only affect entire lines.
	* @param bgColor The background color of the text texture. Default is transparent.
	* @return A pointer to the created SDL_Texture. IMPORTANT: Unlike the other textures managed by the ResourceManager,
	* the caller is responsible for freeing this texture using SDL_DestroyTexture when it is no longer needed.
	*/
	TextSprite* createTextSprite(const std::string& text, const SDL_Color& bgColor={0x00,0x00,0x00,0x00});

private:
	static const std::string ASSET_PATH;
	static const SDL_Color COLOR_KEY;

	SDL_Renderer* _renderer;
	std::map<std::string, ObjectTextureData> _objectTextures;
	std::map<std::string, SDL_Texture*> _simpleTextures;
	std::map<std::string, int> _itemSpriteIds;
	std::map<std::string, int> _weaponSpriteIds;

	std::string _fontPath;
	std::map<float, TTF_Font*> _fonts;

	SDL_Texture* create404Texture();
	void loadItemSpriteIds();
	void loadWeaponSpriteIds();

	SDL_Texture* loadTexture(const std::string& textureName);
	bool loadTextureData(const std::string& textureName, ObjectTextureData& textureData);
	TTF_Font* getFont(float fontSize);
};

