#include "main.h"

#ifndef LSG_TEXT_H
#define LSG_TEXT_H

class LSG_Text : public LSG_Component, public LSG_ScrollBar
{
public:
	LSG_Text(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Text() {}

protected:
	bool        enableScroll;
	int         lastFontSize;
	SDL_Color   lastTextColor;
	std::string text;
	bool        wrap;

public:
	int  GetScrollX();
	int  GetScrollY();
	void SetText(const std::string& text);
	void SetText();

public:
	static TTF_Font*          GetFontArial(int fontSize);
	static TTF_Font*          GetFontMonoSpace(int fontSize);
	static std::string        GetFullPath(const std::string& path);
	static bool               GetStringCompare(const std::string& s1, const std::string& s2);
	static LSG_StringsCompare GetStringsCompare(int sortColumn);

	template<typename... Args>
	static std::string Format(const char* formatString, const Args&... args)
	{
		if (!formatString)
			return "";

		char buffer[1024];
		std::snprintf(buffer, 1024, formatString, args...);

		return std::string(buffer);
	}

	#if defined _windows
	template<typename... Args>
	static std::wstring FormatW(const wchar_t* formatString, const Args&... args)
	{
		if (!formatString)
			return L"";

		wchar_t buffer[1024] = {};
		std::swprintf(buffer, 1024, formatString, args...);

		return std::wstring(buffer);
	}
	#endif

protected:
	SDL_Texture* getTexture(const std::string& text);
	bool         hasChanged();
	void         renderTexture(SDL_Renderer* renderer, const SDL_Rect& backgroundArea);

private:
	TTF_Font* getFont(uint16_t* text, int fontSize);

};

#endif
