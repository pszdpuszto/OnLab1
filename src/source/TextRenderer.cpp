#include "../header/TextRenderer.hpp"

void TextRenderer::setFont(const std::string& font)
{
}

void TextRenderer::clearFonts()
{
	if (TTF_WasInit()) {
		for (auto& pair : _fonts) {
			TTF_CloseFont(pair.second);
		}
	}
}
