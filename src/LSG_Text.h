#include "main.h"

#ifndef LSG_TEXT_H
#define LSG_TEXT_H

class LSG_Text : public LSG_Component
{
public:
	LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Text() {}

protected:
	bool wrap;

private:
	std::mutex surfaceLock;

public:
	static bool                FontSupportsText(TTF_Font* font, uint16_t* text);
	static TTF_Font*           GetFont(int size, uint16_t* text = nullptr);
	static TTF_Font*           GetFontCJK(int size);
	static std::string         GetFullPath(const std::string& path);
	static LSG_TableRowCompare GetTableRowCompare(int column);
	static std::string         Join(const LSG_Strings& strings, const std::string& separator);
	static uint16_t*           ToUTF16(const std::string& text);
	static std::wstring        ToWide(const std::string& text);

protected:
	SDL_Surface* getSurface(const std::string& text, int fontSize = 0, int fontStyle = -1, SDL_Color* textColor = nullptr);
	SDL_Texture* getTexture(const std::string& text, int fontSize = 0, int fontStyle = -1, SDL_Color* textColor = nullptr);

private:
	static std::string replace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring);
};

#endif
