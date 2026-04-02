#include "LSG_Text.h"

std::mutex LSG_Text::surfaceLock;

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->wrap = (LSG_XML::GetAttribute(this->xmlNode, "wrap") == "true");
}

bool LSG_Text::FontSupportsText(TTF_Font* font, uint16_t* text)
{
	if (!font || !text)
		return false;

	for (size_t i = 0; text[i] != 0; i++)
	{
		if (!std::iswspace(text[i]) && !TTF_GlyphIsProvided(font, text[i]))
			return false;
	}

	return true;
}

/**
 * @throws invalid_argument
 */
TTF_Font* LSG_Text::GetFont(int size, uint16_t* text)
{
	auto fontPath = LSG_Text::GetFullPath("fonts/DejaVuSans.ttf");
	auto fontSize = LSG_Window::GetDPIScaled(size);

	auto font = TTF_OpenFont(fontPath.c_str(), fontSize);

	if (text && !LSG_Text::FontSupportsText(font, text))
	{
		TTF_CloseFont(font);

		font = LSG_Text::GetFontCJK(fontSize);
	}

	if (!font)
		throw std::invalid_argument(std::format("Failed to open default font: {}", TTF_GetError()));

	return font;
}

TTF_Font* LSG_Text::GetFontCJK(int size)
{
	auto fontPath = LSG_Text::GetFullPath("fonts/NotoSansCJK-Regular.ttc");
	auto font     = TTF_OpenFont(fontPath.c_str(), size);

	return font;
}

std::string LSG_Text::GetFullPath(const std::string& path)
{
	#if defined _windows
		return (path.size() > 1 && path[1] != ':' ? std::format("{}{}", LSG_GetBasePath(), path) : path);
	#else
		return (!path.empty() && path[0] != '/' ? std::format("{}{}", LSG_GetBasePath(), path) : path);
	#endif
}

LSG_TableRowCompare LSG_Text::GetTableRowCompare(int column)
{
	auto rowCompare = [column](const LSG_Strings& r1, const LSG_Strings& r2)
	{
		if ((column < 0) || (column >= (int)r1.size()) || (column >= (int)r2.size()))
			return false;

		return std::lexicographical_compare(
			r1[column].begin(), r1[column].end(),
			r2[column].begin(), r2[column].end(),
			[](char c1, char c2) {
				return (std::tolower(c1) < std::tolower(c2));
			}
		);
	};

	return rowCompare;
}

//std::string LSG_TextLabel::getText(const std::string& text)
//{
//	if (!text.empty())
//		return text;
//
//	auto xmlText = this->GetXmlValue();
//
//	//if (xmlText.empty())
//	//	xmlText = LSG_XML::GetAttribute(this->xmlNode, "text");
//
//	//if (!xmlText.empty() && (xmlText[0] == '%')
//	//	xmlText = LSG_Window::Labels[xmlText.substr(1)];
//
//	return xmlText;
//}

SDL_Surface* LSG_Text::getSurface(const std::string& text, int fontSize, int fontStyle, SDL_Color* textColor)
{
	if (text.empty())
		return nullptr;

	LSG_Text::surfaceLock.lock();

	auto color = (!textColor    ? this->textColor      : *textColor);
	auto size  = (fontSize == 0 ? this->getFontSize()  : fontSize);
	auto style = (fontStyle < 0 ? this->getFontStyle() : fontStyle);

	auto text16 = LSG_Text::ToUTF16(text);
	auto font   = LSG_Text::GetFont(size, text16);

	TTF_SetFontStyle(font, style);

	SDL_Surface* surface = nullptr;

	if (this->wrap)
		surface = TTF_RenderUNICODE_Blended_Wrapped(font, text16, color, 0);
	else
		surface = TTF_RenderUNICODE_Blended(font, text16, color);

	TTF_CloseFont(font);
	SDL_free(text16);

	if (!surface)
		throw std::invalid_argument(std::format("Failed to create a Unicode surface for text '{}': {}", text, TTF_GetError()));

	LSG_Text::surfaceLock.unlock();

	return surface;
}

SDL_Texture* LSG_Text::getTexture(const std::string& text, int fontSize, int fontStyle, SDL_Color* textColor)
{
	if (text.empty())
		return nullptr;

	auto surface = this->getSurface(text, fontSize, fontStyle, textColor);
	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	return texture;
}

std::string LSG_Text::Join(const LSG_Strings& strings, const std::string& separator)
{
	std::string result = "";

	for (size_t i = 0; i < strings.size(); i++)
	{
		result.append(strings[i]);

		if (i < (strings.size() - 1))
			result.append(separator);
	}

	return result;
}

std::string LSG_Text::replace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring)
{
	auto result        = std::string(text);
	auto matchPosition = result.find(oldSubstring);

	while (matchPosition != std::string::npos) {
		result        = result.replace(matchPosition, oldSubstring.size(), newSubstring);
		matchPosition = result.find(oldSubstring, (matchPosition + newSubstring.size()));
	}

	return result;
}

LSG_Strings LSG_Text::Split(const std::string& text, char separator)
{
	LSG_Strings result;
	std::string token;

	auto stream = std::stringstream(text);
	
	while (std::getline(stream, token, separator))
		result.push_back(token);

	return result;
}

uint16_t* LSG_Text::ToUTF16(const std::string& text)
{
	auto formattedText = LSG_Text::replace(text, "\\n", "\n");

	#if defined _linux
		auto textUTF16 = (uint16_t*)SDL_iconv_string("UCS-2", "UTF-8", formattedText.c_str(), formattedText.size() + 1);
	#else
		auto textUTF16 = (uint16_t*)SDL_iconv_string("UCS-2-INTERNAL", "UTF-8", formattedText.c_str(), formattedText.size() + 1);
	#endif

	if (!textUTF16)
		throw std::invalid_argument(std::format("Failed to convert UTF8 text '{}'", formattedText));

	return textUTF16;
}

std::wstring LSG_Text::ToWide(const std::string& text)
{
	auto utf16 = LSG_Text::ToUTF16(text);
	auto wide  = std::wstring(reinterpret_cast<const wchar_t*>(utf16));

	SDL_free(utf16);

    return wide;
}
