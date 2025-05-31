#include "LSG_Text.h"

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->lastFontSize  = 0;
	this->lastFontStyle = -1;
	this->lastTextColor = {};

	this->wrap = (LSG_XML::GetAttribute(this->xmlNode, "wrap") == "true");
}

/**
 * @throws invalid_argument
 */
TTF_Font* LSG_Text::GetFontArial(int fontSize)
{
	#if defined _android
		auto FONT_PATH = "/system/fonts/NotoSansCJK-Regular.ttc";
	#elif defined _ios
		auto fullPath  = LSG_Text::GetFullPath("ui/Arial Unicode.ttf");
		auto FONT_PATH = fullPath.c_str();
	#elif defined _linux
		auto NOTO_SANS_CJK = "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc";
		auto DEJAVU_SANS   = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
		auto FONT_PATH     = (std::filesystem::exists(NOTO_SANS_CJK) ? NOTO_SANS_CJK : DEJAVU_SANS);
	#elif defined  _macosx
		auto FONT_PATH = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
	#elif defined _windows
		auto FONT_PATH = "C:\\Windows\\Fonts\\ARIALUNI.TTF";
	#endif

	auto font = TTF_OpenFont(FONT_PATH, LSG_Graphics::GetDPIScaled(fontSize));

	if (!font)
		throw std::invalid_argument(std::format("Failed to open default font '{}': {}", FONT_PATH, TTF_GetError()));

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

SDL_Texture* LSG_Text::getTexture(const std::string& text, int fontSize, int fontStyle, SDL_Color* textColor)
{
	if (text.empty())
		return nullptr;

	auto color = (!textColor    ? this->textColor      : *textColor);
	auto size  = (fontSize == 0 ? this->getFontSize()  : fontSize);
	auto style = (fontStyle < 0 ? this->getFontStyle() : fontStyle);

	auto font = LSG_Text::GetFontArial(size);

	TTF_SetFontStyle(font, style);

	SDL_Surface* surface   = nullptr;
	auto         textUTF16 = LSG_Text::ToUTF16(text);

	if (this->wrap)
		surface = TTF_RenderUNICODE_Blended_Wrapped(font, textUTF16, color, 0);
	else
		surface = TTF_RenderUNICODE_Blended(font, textUTF16, color);

	TTF_CloseFont(font);
	SDL_free(textUTF16);

	if (!surface)
		throw std::invalid_argument(std::format("Failed to create a Unicode surface: {}", TTF_GetError()));

	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	this->lastFontSize  = size;
	this->lastFontStyle = style;
	this->lastTextColor = SDL_Color(color);

	return texture;
}

bool LSG_Text::hasChanged()
{
	bool isColorChanged     = !LSG_Graphics::IsColorEquals(this->textColor, this->lastTextColor);
	bool isFontSizeChanged  = (this->getFontSize()  != this->lastFontSize);
	bool isFontStyleChanged = (this->getFontStyle() != this->lastFontStyle);

	return (isColorChanged || isFontSizeChanged || isFontStyleChanged);
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
