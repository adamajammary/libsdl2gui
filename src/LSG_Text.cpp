#include "LSG_Text.h"

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent), LSG_ScrollBar()
{
	this->enableScroll  = true;
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
		font = this->getFontMonoSpace(fontSize);

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
		font = this->getFontArial(fontSize);

	if (font)
		TTF_SetFontStyle(font, this->fontStyle);

	return font;
}

TTF_Font* LSG_Text::getFontArial(int fontSize)
{
	TTF_Font* font = nullptr;

	#if defined _android
		font = TTF_OpenFont("/system/fonts/DroidSans.ttf", fontSize);
	#elif defined _ios
		font = TTF_OpenFont("/System/Library/Fonts/Cache/arialuni.ttf", fontSize);
	#elif defined _linux
		font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", fontSize);
	#elif defined  _macosx
		font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial Unicode.ttf", fontSize);
	#elif defined _windows
		font = TTF_OpenFont("C:/Windows/Fonts/ARIALUNI.TTF", fontSize);
	#endif

	return font;
}

TTF_Font* LSG_Text::getFontMonoSpace(int fontSize)
{
	TTF_Font* font = nullptr;

	#if defined _android
		font = TTF_OpenFont("/system/fonts/DroidSansMono.ttf", fontSize);
	#elif defined _ios
		font = TTF_OpenFont("/System/Library/Fonts/Cache/CourierNewBold.ttf", fontSize);
	#elif defined _linux
		font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf", fontSize);
	#elif defined  _macosx
		font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", fontSize);
	#elif defined _windows
		font = TTF_OpenFont("C:/Windows/Fonts/courbd.ttf", fontSize);
	#endif

	return font;
}

int LSG_Text::GetScrollX()
{
	return this->scrollOffsetX;
}

int LSG_Text::GetScrollY()
{
	return this->scrollOffsetY;
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

	auto textUTF16 = SDL_iconv_utf8_ucs2(text.c_str());

	if (!textUTF16)
		return nullptr;

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

	return (!SDL_ColorEquals(this->textColor, this->lastTextColor) || (fontSize != this->lastFontSize));
}

void LSG_Text::renderTexture(SDL_Renderer* renderer, const SDL_Rect& backgroundArea)
{
	auto size = this->GetTextureSize();

	SDL_Rect background = backgroundArea;

	if (this->enableScroll)
	{
		bool showScrollY = (size.height > (backgroundArea.h + LSG_SCROLL_WIDTH));
		bool showScrollX = (size.width  > (backgroundArea.w + LSG_SCROLL_WIDTH));

		if (showScrollY)
			background.w -= LSG_SCROLL_WIDTH;

		if (showScrollX)
			background.h -= LSG_SCROLL_WIDTH;

		this->showScrollY = (size.height > (background.h + LSG_SCROLL_WIDTH));
		this->showScrollX = (size.width  > (background.w + LSG_SCROLL_WIDTH));

		if (this->showScrollY && !showScrollY)
			background.w -= LSG_SCROLL_WIDTH;

		if (this->showScrollX && !showScrollX)
			background.h -= LSG_SCROLL_WIDTH;
	}

	SDL_Rect clip = { 0, 0, min(size.width, background.w), min(size.height, background.h) };
	auto     dest = this->getRenderDestinationAligned(background, size);

	if (this->showScrollX)
	{
		auto maxScrollOffsetX = (size.width - clip.w);

		if (this->scrollOffsetX > maxScrollOffsetX)
			this->scrollOffsetX = maxScrollOffsetX;

		clip.x += this->scrollOffsetX;
	}

	if (this->showScrollY)
	{
		auto maxScrollOffsetY = (size.height - clip.h);

		if (this->scrollOffsetY > maxScrollOffsetY)
			this->scrollOffsetY = maxScrollOffsetY;

		clip.y += this->scrollOffsetY;
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
