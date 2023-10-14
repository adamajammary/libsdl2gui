#include "LSG_Text.h"

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent), LSG_ScrollBar()
{
	this->enableScroll  = (LSG_XML::GetAttribute(this->xmlNode, "enable-scroll") != "false");
	this->lastFontSize  = 0;
	this->lastTextColor = {};
	this->text          = "";
	this->wrap          = (LSG_XML::GetAttribute(this->xmlNode, "wrap") == "true");
}

TTF_Font* LSG_Text::getFont(uint16_t* text, int fontSize)
{
	if ((fontSize < 1) || !text)
		return nullptr;

	TTF_Font* font = nullptr;

	if (this->IsMenu() || this->IsSubMenu() || this->IsMenuItem())
		font = LSG_Text::GetFontMonoSpace(fontSize);

	if (font)
	{
		for (int i = 0; text[i] > 0 && i < 1024; i++)
		{
			if ((text[i] == '\n') || TTF_GlyphIsProvided(font, text[i]))
				continue;

			TTF_CloseFont(font);
			font = nullptr;

			break;
		}
	}

	if (!font)
		font = LSG_Text::GetFontArial(fontSize);

	if (font)
		TTF_SetFontStyle(font, this->fontStyle);

	return font;
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
		auto FONT_PATH = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
	#elif defined  _macosx
		auto FONT_PATH = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
	#elif defined _windows
		auto FONT_PATH = "C:\\Windows\\Fonts\\ARIALUNI.TTF";
	#endif

	auto font = TTF_OpenFont(FONT_PATH, fontSize);

	if (!font)
		throw std::invalid_argument(LSG_Text::Format("Failed to open default font '%s': %s", FONT_PATH, TTF_GetError()));

	return font;
}

/**
 * @throws invalid_argument
 */
TTF_Font* LSG_Text::GetFontMonoSpace(int fontSize)
{
	#if defined _android
		auto FONT_PATH = "/system/fonts/DroidSansMono.ttf";
	#elif defined _ios
		auto fullPath  = LSG_Text::GetFullPath("ui/Courier New Bold.ttf");
		auto FONT_PATH = fullPath.c_str();
	#elif defined _linux
		auto FONT_PATH = "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf";
	#elif defined  _macosx
		auto FONT_PATH = "/System/Library/Fonts/Supplemental/Courier New Bold.ttf";
	#elif defined _windows
		auto FONT_PATH = "C:\\Windows\\Fonts\\courbd.ttf";
	#endif

	auto font = TTF_OpenFont(FONT_PATH, fontSize);

	if (!font)
		throw std::invalid_argument(LSG_Text::Format("Failed to open mono font '%s': %s", FONT_PATH, TTF_GetError()));

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

int LSG_Text::GetScrollX()
{
	return this->scrollOffsetX;
}

int LSG_Text::GetScrollY()
{
	return this->scrollOffsetY;
}

bool LSG_Text::GetStringCompare(const std::string& s1, const std::string& s2)
{
	return std::lexicographical_compare(
		s1.begin(), s1.end(),
		s2.begin(), s2.end(),
		[](const char& c1, const char& c2) {
			return std::tolower(c1) < std::tolower(c2);
		}
	);
};

LSG_StringsCompare LSG_Text::GetStringsCompare(int sortColumn)
{
	auto compare = [sortColumn](const LSG_Strings& s1, const LSG_Strings& s2)
	{
		if ((sortColumn < 0) || (int)(sortColumn >= s1.size()) || (int)(sortColumn >= s2.size()))
			return false;

		return LSG_Text::GetStringCompare(s1[sortColumn], s2[sortColumn]);
	};

	return compare;
}

//std::string LSG_TextLabel::getText(const std::string& text)
//{
//	if (!text.empty())
//		return text;
//
//	auto xmlText = LSG_XML::GetValue(this->xmlNode, this->xmlDoc);
//
//	//if (xmlText.empty())
//	//	xmlText = LSG_XML::GetAttribute(xmlNode, "text");
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

	#if defined _linux
		auto textUTF16 = (uint16_t*)SDL_iconv_string("UCS-2", "UTF-8", text.c_str(), SDL_strlen(text.c_str()) + 1);
	#else
		auto textUTF16 = SDL_iconv_utf8_ucs2(text.c_str());
	#endif

	if (!textUTF16)
		throw std::invalid_argument(LSG_Text::Format("Failed to convert UTF8 text: %s", text.c_str()));

	auto fontSize = this->getFontSize();
	auto font     = LSG_Text::getFont(textUTF16, fontSize);

	if (!font) {
		SDL_free(textUTF16);
		return nullptr;
	}

	SDL_Surface* surface = nullptr;

	if (this->wrap)
		surface = TTF_RenderUNICODE_Blended_Wrapped(font, textUTF16, this->textColor, 0);
	else
		surface = TTF_RenderUNICODE_Blended(font, textUTF16, this->textColor);

	TTF_CloseFont(font);
	SDL_free(textUTF16);

	if (!surface)
		return nullptr;

	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	this->lastFontSize  = fontSize;
	this->lastTextColor = SDL_Color(this->textColor);

	return texture;
}

bool LSG_Text::hasChanged()
{
	auto fontSize = this->getFontSize();

	return (!LSG_Graphics::IsColorEquals(this->textColor, this->lastTextColor) || (fontSize != this->lastFontSize));
}

void LSG_Text::renderTexture(SDL_Renderer* renderer, const SDL_Rect& backgroundArea)
{
	auto size = this->GetTextureSize();

	SDL_Rect background = backgroundArea;

	if (this->enableScroll)
	{
		bool showScrollY = (size.height > (backgroundArea.h + LSG_ConstScrollBar::Width));
		bool showScrollX = (size.width  > (backgroundArea.w + LSG_ConstScrollBar::Width));

		if (showScrollY)
			background.w -= LSG_ConstScrollBar::Width;

		if (showScrollX)
			background.h -= LSG_ConstScrollBar::Width;

		this->showScrollY = (size.height > background.h);
		this->showScrollX = (size.width  > background.w);

		if (this->showScrollY && !showScrollY)
			background.w -= LSG_ConstScrollBar::Width;

		if (this->showScrollX && !showScrollX)
			background.h -= LSG_ConstScrollBar::Width;
	}

	SDL_Rect clip = { 0, 0, std::min(size.width, background.w), std::min(size.height, background.h) };
	auto     dest = this->getRenderDestinationAligned(background, size);

	if (this->showScrollX)
	{
		auto maxScrollOffsetX = (size.width - clip.w);

		if (this->scrollOffsetX > maxScrollOffsetX)
			this->scrollOffsetX = maxScrollOffsetX;

		clip.x += this->scrollOffsetX;
	} else {
		this->scrollOffsetX = 0;
	}

	if (this->showScrollY)
	{
		auto maxScrollOffsetY = (size.height - clip.h);

		if (this->scrollOffsetY > maxScrollOffsetY)
			this->scrollOffsetY = maxScrollOffsetY;

		clip.y += this->scrollOffsetY;
	} else {
		this->scrollOffsetY = 0;
	}

	SDL_RenderCopy(renderer, this->texture, &clip, &dest);
}

void LSG_Text::SetText(const std::string &text)
{
	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->destroyTextures();

	this->text = text;

	if (!text.empty())
		this->texture = this->getTexture(this->text);
}

void LSG_Text::SetText()
{
	if (!this->text.empty() && this->texture && !this->hasChanged())
	{
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		return;
	}

	this->destroyTextures();

	if (this->text.empty())
		this->text = LSG_XML::GetValue(this->xmlNode, this->xmlDoc);

	if (!this->text.empty())
		this->texture = this->getTexture(this->text);
}
