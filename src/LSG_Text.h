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
	int       lastFontStyle;
	SDL_Color lastTextColor;
	bool      wrap;

public:
	static TTF_Font*           GetFontArial(int fontSize);
	static std::string         GetFullPath(const std::string& path);
	static LSG_TableRowCompare GetTableRowCompare(int column);
	static uint16_t*           ToUTF16(const std::string& text);
	static std::wstring        ToWide(const std::string& text);

	template<typename... Args>
	static std::string Format(const char* formatString, const Args&... args)
	{
		if (!formatString)
			return "";

		char buffer[1024] = {};

		auto result = std::snprintf(buffer, 1024, formatString, args...);

		if (result < 1024)
			return std::string(buffer);

		auto bufferSize    = (size_t)(result + 1);
		auto dynamicBuffer = (char*)std::malloc(bufferSize);

		std::snprintf(dynamicBuffer, bufferSize, formatString, args...);

		auto resultString = std::string(dynamicBuffer != NULL ? dynamicBuffer : "");

		std::free(dynamicBuffer);

		return resultString;
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
	SDL_Texture* getTexture(const std::string& text, int fontSize = 0, int fontStyle = -1, SDL_Color* textColor = nullptr);
	bool         hasChanged();

private:
	static std::string replace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring);
};

#endif
