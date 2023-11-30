#include "main.h"

#ifndef LSG_TEXT_H
#define LSG_TEXT_H

class LSG_Text : public LSG_Component
{
public:
	LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Text() {}

protected:
	int       lastFontSize;
	SDL_Color lastTextColor;
	bool      wrap;

public:
	static TTF_Font*             GetFontArial(int fontSize);
	static std::string           GetFullPath(const std::string& path);
	static LSG_ComponentsCompare GetRowCompare(int column);
	static bool                  GetXmlValueCompare(LSG_Component* c1, LSG_Component* c2);

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

};

#endif
