#include "main.h"

#ifndef LSG_TEXT_H
#define LSG_TEXT_H

class LSG_Text : public LSG_Component
{
public:
	LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Text() {}

protected:
	SDL_Texture*     ellipsisTexture;
	LSG_TextOverflow textOverflow;
	bool             wrap;

private:
	static std::mutex surfaceLock;

public:
	static bool                FontSupportsText(TTF_Font* font, uint16_t* text);
	static TTF_Font*           GetFont(int size, uint16_t* text = nullptr);
	static TTF_Font*           GetFontCJK(int size);
	static std::string         GetFullPath(const std::string& path);
	static LSG_TableRowCompare GetTableRowCompare(int column);
	static SDL_Surface*        GetSurface(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap);
	static SDL_Texture*        GetTexture(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap);
	static std::string         Join(const LSG_Strings& strings, const std::string& separator);
	static LSG_Strings         Split(const std::string& text, char separator);
	static uint16_t*           ToUTF16(const std::string& text);
	static std::wstring        ToWide(const std::string& text);

protected:
	SDL_Surface* getSurface(const std::string& text) const;
	SDL_Texture* getTexture(const std::string& text) const;
	void         renderTextWithEllipse(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& destination, int maxWidth) const;

private:
	static SDL_Surface* getSurface(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap);
	static SDL_Texture* getTexture(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap);
	static std::string  replace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring);
};

#endif
