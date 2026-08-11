#include "LSG_Text.h"

std::mutex LSG_Text::surfaceLock;

LSG_Text::LSG_Text(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->ellipsisTexture = nullptr;
	this->textOverflow    = LSG_TEXT_OVERFLOW_NONE;

	auto xmlAttributes = LSG_XML::GetAttributes(xmlNode);

	this->wrap = (xmlAttributes.contains("wrap") && (xmlAttributes["wrap"] == "true"));

	if (xmlAttributes.contains("text-overflow"))
		this->textOverflow = (xmlAttributes["text-overflow"] == "ellipsis" ? LSG_TEXT_OVERFLOW_ELLIPSIS : LSG_TEXT_OVERFLOW_CLIP);
}

bool LSG_Text::FontSupportsText(TTF_Font* font, uint16_t* text)
{
	if (!font || !text)
		return false;

	for (size_t i = 0; text[i] != 0; i++)
	{
		if (!std::iswspace(text[i]) && !TTF_FontHasGlyph(font, text[i]))
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
		throw std::invalid_argument(std::format("Failed to open default font: {}", SDL_GetError()));

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

SDL_Surface* LSG_Text::getSurface(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap)
{
	if (text.empty())
		return nullptr;

	LSG_Text::surfaceLock.lock();

	auto textUTF16 = LSG_Text::ToUTF16(text);
	auto textUTF8  = LSG_Text::ToUTF8(textUTF16);

	auto font = LSG_Text::GetFont(fontSize, textUTF16);

	TTF_SetFontStyle(font, fontStyle);

	SDL_Surface* surface = nullptr;

	if (wrap)
		surface = TTF_RenderText_Blended_Wrapped(font, textUTF8.c_str(), textUTF8.size(), textColor, 0);
	else
		surface = TTF_RenderText_Blended(font, textUTF8.c_str(), textUTF8.size(), textColor);

	TTF_CloseFont(font);
	SDL_free(textUTF16);

	if (!surface)
		throw std::invalid_argument(std::format("Failed to create a Unicode surface for text '{}': {}", text, SDL_GetError()));

	LSG_Text::surfaceLock.unlock();

	return surface;
}

SDL_Texture* LSG_Text::getTexture(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap)
{
	if (text.empty())
		return nullptr;

	auto surface = LSG_Text::getSurface(text, fontSize, fontStyle, textColor, wrap);
	auto texture = LSG_Window::ToTexture(surface);

	SDL_DestroySurface(surface);

	return texture;
}

SDL_Surface* LSG_Text::getSurface(const std::string& text) const
{
	return this->getSurface(text, this->getFontSize(), this->getFontStyle(), this->textColor, this->wrap);
}

SDL_Texture* LSG_Text::getTexture(const std::string& text) const
{
	return this->getTexture(text, this->getFontSize(), this->getFontStyle(), this->textColor, this->wrap);
}

SDL_Surface* LSG_Text::GetSurface(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap)
{
	return LSG_Text::getSurface(text, fontSize, fontStyle, textColor, wrap);
}

SDL_Texture* LSG_Text::GetTexture(const std::string& text, int fontSize, int fontStyle, const SDL_Color& textColor, bool wrap)
{
	return LSG_Text::getTexture(text, fontSize, fontStyle, textColor, wrap);
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

void LSG_Text::renderTextOverflowClip(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& destination, int maxWidth) const
{
	SDL_Rect textClip = { 0, 0, maxWidth, destination.h };

	SDL_Rect textDestination = destination;

	textDestination.w = textClip.w;

	LSG_Graphics::RenderTexture(renderer, texture, &textClip, &textDestination);
}

void LSG_Text::renderTextOverflowEllipse(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& destination, int maxWidth) const
{
	auto ellipsisSize = LSG_Graphics::GetTextureSize(this->ellipsisTexture);

	SDL_Rect textClip = { 0, 0, (maxWidth - ellipsisSize.width), destination.h };

	SDL_Rect textDestination = destination;

	textDestination.w = textClip.w;

	LSG_Graphics::RenderTexture(renderer, texture, &textClip, &textDestination);

	SDL_Rect ellipsisDestination = {
		(destination.x + maxWidth - ellipsisSize.width),
		(destination.y + destination.h - ellipsisSize.height),
		ellipsisSize.width,
		ellipsisSize.height
	};

	LSG_Graphics::RenderTexture(renderer, this->ellipsisTexture, nullptr, &ellipsisDestination);
}

std::string LSG_Text::Replace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring)
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

std::string LSG_Text::ToLower(const std::string& text)
{
	auto lower = std::string(text);

	for (size_t i = 0; i < text.size(); i++)
		lower[i] = std::tolower(text[i]);

	return lower;
}

std::string LSG_Text::ToUpper(const std::string& text)
{
	auto upper = std::string(text);

	for (size_t i = 0; i < text.size(); i++)
		upper[i] = std::toupper(text[i]);

	return upper;
}

std::string LSG_Text::ToUTF8(const std::wstring& wide)
{
    auto buffer = SDL_iconv_wchar_utf8(wide.c_str());
    auto utf8   = std::string(buffer);

	SDL_free(buffer);

    return utf8;
}

std::string LSG_Text::ToUTF8(uint16_t* utf16)
{
	auto wide = std::wstring(reinterpret_cast<const wchar_t*>(utf16));

	return LSG_Text::ToUTF8(wide);
}

uint16_t* LSG_Text::ToUTF16(const std::string& text)
{
	auto formattedText = LSG_Text::Replace(text, "\\n", "\n");

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

std::string LSG_Text::Trim(const std::string& text)
{
	auto trimmed = std::string(text);

	while (!trimmed.empty() && (trimmed[0] > 0) && std::isspace(trimmed[0]))
		trimmed = trimmed.substr(1);

	while (!trimmed.empty() && (trimmed[trimmed.size() - 1] > 0) && std::isspace(trimmed[trimmed.size() - 1]))
		trimmed = trimmed.substr(0, trimmed.size() - 1);

	return trimmed;
}
