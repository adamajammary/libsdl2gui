#include "LSG_Text.h"

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->lastFontSize  = 0;
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
		auto FONT_PATH = "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc";
		//auto FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"; // Missing CJK
	#elif defined  _macosx
		auto FONT_PATH = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
	#elif defined _windows
		auto FONT_PATH = "C:\\Windows\\Fonts\\ARIALUNI.TTF";
	#endif

	auto font = TTF_OpenFont(FONT_PATH, LSG_Graphics::GetDPIScaled(fontSize));

	if (!font)
		throw std::invalid_argument(LSG_Text::Format("Failed to open default font '%s': %s", FONT_PATH, TTF_GetError()));

	return font;
}

std::string LSG_Text::GetFullPath(const std::string& path)
{
	#if defined _windows
		return (path.size() > 1 && path[1] != ':' ? LSG_Text::Format("%s%s", LSG_GetBasePath(), path.c_str()) : path);
	#else
		return (!path.empty() && path[0] != '/' ? LSG_Text::Format("%s%s", LSG_GetBasePath(), path.c_str()) : path);
	#endif
}

LSG_ComponentsCompare LSG_Text::GetRowCompare(int column)
{
	auto compare = [column](LSG_Component* i1, LSG_Component* i2)
	{
		if ((column < 0) || (column >= (int)i1->GetChildCount()) || (column >= (int)i2->GetChildCount()))
			return false;

		return LSG_Text::GetXmlValueCompare(i1->GetChild(column), i2->GetChild(column));
	};

	return compare;
}

bool LSG_Text::GetXmlValueCompare(LSG_Component* i1, LSG_Component* i2)
{
	return std::lexicographical_compare(
		i1->text.begin(), i1->text.end(),
		i2->text.begin(), i2->text.end(),
		[](const char& c1, const char& c2) {
			return std::tolower(c1) < std::tolower(c2);
		}
	);
};

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

SDL_Texture* LSG_Text::getTexture(const std::string& text)
{
	if (text.empty())
		return nullptr;

	auto fontSize = this->getFontSize();
	auto font     = LSG_Text::GetFontArial(fontSize);

	TTF_SetFontStyle(font, this->fontStyle);

	#if defined _linux
		auto textUTF16 = (uint16_t*)SDL_iconv_string("UCS-2", "UTF-8", text.c_str(), SDL_strlen(text.c_str()) + 1);
	#else
		auto textUTF16 = SDL_iconv_utf8_ucs2(text.c_str());
	#endif

	if (!textUTF16)
		throw std::invalid_argument(LSG_Text::Format("Failed to convert UTF8 text: %s", SDL_GetError()));

	SDL_Surface* surface = nullptr;

	if (this->wrap)
		surface = TTF_RenderUNICODE_Blended_Wrapped(font, textUTF16, this->textColor, 0);
	else
		surface = TTF_RenderUNICODE_Blended(font, textUTF16, this->textColor);

	TTF_CloseFont(font);
	SDL_free(textUTF16);

	if (!surface)
		throw std::invalid_argument(LSG_Text::Format("Failed to create a Unicode surface: %s", TTF_GetError()));

	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	this->lastFontSize  = fontSize;
	this->lastTextColor = SDL_Color(this->textColor);

	return texture;
}

bool LSG_Text::hasChanged()
{
	bool isColorChanged    = !LSG_Graphics::IsColorEquals(this->textColor, this->lastTextColor);
	bool isFontSizeChanged = (this->getFontSize() != this->lastFontSize);

	return (isColorChanged || isFontSizeChanged);
}
