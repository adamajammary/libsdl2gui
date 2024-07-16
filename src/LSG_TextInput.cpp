#include "LSG_TextInput.h"

LSG_TextInput::LSG_TextInput(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->active                = false;
	this->cursorPosition        = 0;
	this->cursorTextWidth       = 0;
	this->highlightedIconClear  = false;
	this->highlightedTextLength = 0;
	this->highlightedTextSize   = {};
	this->lastCursorActive      = 0;
	this->maxIconSize           = 0;
	this->textSize              = {};

	auto attributes = this->GetXmlAttributes();

	this->placeholder = attributes["placeholder"];
	this->value       = attributes["value"];

	this->textures.resize(NR_OF_TEXT_INPUT_TEXTURES);
}

void LSG_TextInput::Clear()
{
	this->cursorPosition        = 0;
	this->highlightedTextLength = 0;
	this->value                 = "";

	this->setValue();
	this->Stop();

	this->sendEvent(LSG_EVENT_TEXT_INPUT_CLEARED);
}

void LSG_TextInput::Complete()
{
	this->Stop();

	this->sendEvent(LSG_EVENT_TEXT_INPUT_COMPLETED);
}

void LSG_TextInput::Copy()
{
	if (!this->active)
		return;

	if (this->highlightedTextLength == this->value.size())
		SDL_SetClipboardText(this->value.c_str());
	else if (this->highlightedTextLength > 0)
		SDL_SetClipboardText(this->value.substr(this->cursorPosition, this->highlightedTextLength).c_str());
	else if (this->highlightedTextLength < 0)
		SDL_SetClipboardText(this->value.substr((this->cursorPosition + this->highlightedTextLength), std::abs(this->highlightedTextLength)).c_str());
}

void LSG_TextInput::Delete()
{
	if (!this->active || this->value.empty())
		return;

	if (this->highlightedTextLength == 0)
	{
		if (this->cursorPosition >= this->value.size())
			return;

		auto   character      = this->value[this->cursorPosition];
		size_t characterWidth = (character < 0 ? 2 : 1);

		this->value.erase(this->cursorPosition, characterWidth);
	} else {
		this->value.erase(this->cursorPosition, this->highlightedTextLength);
	}

	this->highlightedTextLength = 0;

	this->setValue();
}

size_t LSG_TextInput::getCursorPosition(const SDL_Point& mousePosition)
{
	auto background = LSG_UI::GetScrolledBackground(this);
	auto iconSize   = LSG_Graphics::GetTextureSize(this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR]);

	auto border2x  = (this->border + this->border);
	auto padding3x = (this->padding * 3);

	auto fillWidth = (background.w - iconSize.width - padding3x - border2x);

	auto textWidth        = this->highlightedTextSize.width;
	auto directionalWidth = (this->highlightedTextLength < 0 ? -textWidth : textWidth);

	auto page      = ((this->cursorTextWidth + directionalWidth) / fillWidth);
	auto lastPage  = (this->textSize.width  / fillWidth);

	auto offsetX = (page * fillWidth);

	if (!this->value.empty() && (page > 0) && (page == lastPage))
		offsetX = (this->textSize.width - fillWidth);

	auto startX = (background.x + this->border + this->padding);
	auto clickX = (mousePosition.x + LSG_Cursor::IBeamOffset);

	auto position = (clickX - startX + offsetX);

	auto   font = LSG_Text::GetFontArial(this->getFontSize());
	size_t i;
	int    w, x = 0;

	for (i = 0; i < this->value.size(); i++)
	{
		if (this->value[i] < 0)
		{
			auto text16 = LSG_Text::ToUTF16(this->value.substr(i, 2));

			TTF_SizeUNICODE(font, text16, &w, nullptr);
			SDL_free(text16);
		} else {
			TTF_SizeUTF8(font, this->value.substr(i, 1).c_str(), &w, nullptr);
		}

		if ((x + w) > position)
			break;

		x += w;

		if (this->value[i] < 0)
			i++;
	}

	TTF_CloseFont(font);

	return i;
}

SDL_Rect LSG_TextInput::getIconClear(const SDL_Rect& fillArea)
{
	auto textureSize = LSG_Graphics::GetTextureSize(this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR]);

	SDL_Rect iconClear = {
		(fillArea.x + fillArea.w - textureSize.width),
		fillArea.y,
		textureSize.width,
		textureSize.height
	};

	return iconClear;
}

SDL_Size LSG_TextInput::GetSize()
{
	auto attributes  = this->GetXmlAttributes();
	auto showValue   = (this->active || !this->value.empty());
	auto texture     = this->textures[showValue ? LSG_TEXT_INPUT_TEXTURE_VALUE : LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER];
	auto textureSize = (showValue ? this->textSize : LSG_Graphics::GetTextureSize(texture));

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	textureSize.width  += (padding2x + border2x);
	textureSize.height += (padding2x + border2x);

	if (attributes.contains("width") && (this->background.w > 0))
		textureSize.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		textureSize.height = this->background.h;

	return textureSize;
}

std::string LSG_TextInput::GetValue()
{
	return this->value;
}

void LSG_TextInput::Highlight(const SDL_Point& mousePosition)
{
	this->highlightedIconClear = this->isMouseOverIconClear(mousePosition);
}

void LSG_TextInput::Input(const std::string& text)
{
	if (!this->active || !this->enabled || !this->visible)
		return;

	if (this->highlightedTextLength != 0)
		this->Delete();

	auto value1 = this->value.substr(0, this->cursorPosition);
	auto value2 = this->value.substr(this->cursorPosition);

	this->value = LSG_Text::Format("%s%s%s", value1.c_str(), text.c_str(), value2.c_str());

	this->cursorPosition       += text.size();
	this->highlightedTextLength = 0;

	this->setValue();
}

bool LSG_TextInput::IsHighlightedIconClear() const
{
	return this->highlightedIconClear;
}

bool LSG_TextInput::isMouseOverIconClear(const SDL_Point& mousePosition)
{
	if (!this->active || !this->enabled || !this->visible || this->value.empty())
		return false;

	auto background = LSG_UI::GetScrolledBackground(this);

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	SDL_Rect fillArea = {
		(background.x + this->border + this->padding),
		(background.y + this->border + this->padding),
		(background.w - border2x - padding2x),
		(background.h - border2x - padding2x)
	};

	auto iconClear = this->getIconClear(fillArea);

	return SDL_PointInRect(&mousePosition, &iconClear);
}

void LSG_TextInput::MoveCursorEnd()
{
	if (!this->active)
		return;

	this->cursorPosition        = this->value.size();
	this->highlightedTextLength = 0;

	this->setCursor();
}

void LSG_TextInput::MoveCursorHome()
{
	if (!this->active)
		return;

	this->cursorPosition        = 0;
	this->highlightedTextLength = 0;

	this->setCursor();
}

void LSG_TextInput::MoveCursorLeft()
{
	if (!this->active)
		return;

	if (this->cursorPosition == 0) {
		this->highlightedTextLength = 0;
		return;
	}

	if (this->highlightedTextLength == 0)
	{
		auto character      = this->value[this->cursorPosition - 1];
		int  characterWidth = (character < 0 ? 2 : 1);

		this->cursorPosition = (size_t)std::max(((int)this->cursorPosition - characterWidth), 0);
	} else if (this->highlightedTextLength < 0) {
		this->cursorPosition += this->highlightedTextLength;
	}

	this->highlightedTextLength = 0;

	this->setCursor();
}

void LSG_TextInput::MoveCursorRight()
{
	if (!this->active || (this->cursorPosition == this->value.size()))
		return;

	if (this->highlightedTextLength == 0)
	{
		auto   character      = this->value[this->cursorPosition];
		size_t characterWidth = (character < 0 ? 2 : 1);

		this->cursorPosition = std::min((this->cursorPosition + characterWidth), this->value.size());
	} else if (this->highlightedTextLength > 0) {
		this->cursorPosition += this->highlightedTextLength;
	}

	this->highlightedTextLength = 0;

	this->setCursor();
}

void LSG_TextInput::moveCursorTo(const SDL_Point& mousePosition)
{
	this->cursorPosition        = this->getCursorPosition(mousePosition);
	this->highlightedTextLength = 0;

	this->setCursor();
}

bool LSG_TextInput::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->visible)
		return false;

	if (this->isMouseOverIconClear(mousePosition)) {
		this->Clear();
		return false;
	} else if (this->active) {
		this->moveCursorTo(mousePosition);
		return true;
	}

	this->start();

	return false;
}

bool LSG_TextInput::OnMouseMove(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->visible || !this->active || !LSG_Events::IsMouseDown())
		return false;

	auto position = (int)this->getCursorPosition(mousePosition);

	this->highlightedTextLength = (position - (int)this->cursorPosition);

	this->setCursor();

	return true;
}

void LSG_TextInput::Paste()
{
	if (!this->active)
		return;

	auto text = SDL_GetClipboardText();

	this->Input(text);

	SDL_free(text);
}

void LSG_TextInput::Remove()
{
	if (!this->active || this->value.empty())
		return;

	if (this->highlightedTextLength == 0)
	{
		if (this->cursorPosition < 1)
			return;

		auto   character      = this->value[this->cursorPosition - 1];
		size_t characterWidth = (character < 0 ? 2 : 1);

		this->value.erase((this->cursorPosition - characterWidth), characterWidth);

		this->cursorPosition -= characterWidth;
	} else {
		this->value.erase(this->cursorPosition, this->highlightedTextLength);
	}

	this->highlightedTextLength = 0;

	this->setValue();
}

void LSG_TextInput::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_TextInput::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_TextInput::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	SDL_Rect background = {
		(this->background.x + this->border + this->padding),
		(this->background.y + this->border + this->padding),
		(this->background.w - border2x - padding2x),
		(this->background.h - border2x - padding2x)
	};

	SDL_Rect iconClear = {};

	if (this->active && !this->value.empty()) {
		iconClear     = this->getIconClear(background);
		background.w -= (iconClear.w + this->padding);
	}

	this->maxIconSize = background.h;

	this->renderIconClear(renderer,       iconClear);
	this->renderText(renderer,            background);
	this->renderCursor(renderer,          background);
	this->renderHighlightedText(renderer, background);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_TextInput::renderCursor(SDL_Renderer* renderer, const SDL_Rect& background)
{
	if (!this->active || ((SDL_GetTicks() - this->lastCursorActive) < 500) || (this->highlightedTextLength != 0))
		return;

	SDL_SetRenderDrawBlendMode(renderer, (this->textColor.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, this->textColor.r, this->textColor.g, this->textColor.b, this->textColor.a);

	auto page     = (this->cursorTextWidth / background.w);
	auto lastPage = (this->textSize.width  / background.w);
	auto offsetX  = (this->cursorTextWidth % background.w);

	if (!this->value.empty() && (page > 0) && (page == lastPage))
		offsetX += ((page * background.w) - (this->textSize.width - background.w));

	auto x = std::min((background.x + offsetX), (background.x + background.w));

	SDL_RenderDrawLine(renderer, x, background.y, x, (background.y + background.h));

	if ((SDL_GetTicks() - this->lastCursorActive) >= 1000)
		this->lastCursorActive = SDL_GetTicks();
}

void LSG_TextInput::renderHighlightedText(SDL_Renderer* renderer, const SDL_Rect& background)
{
	if (!this->active || (this->highlightedTextLength == 0))
		return;

	auto width      = this->highlightedTextSize.width;
	auto start      = (this->highlightedTextLength < 0 ? (this->cursorTextWidth - width) : this->cursorTextWidth);
	auto startWidth = (start + width);

	if (startWidth >= background.w)
	{
		auto pageStart      = (start / background.w);
		auto pageEnd        = (startWidth / background.w);
		auto lastPageOffset = ((pageEnd * background.w) - (this->textSize.width - background.w));
		auto lastPage       = (this->textSize.width / background.w);
		bool isLastPage     = ((pageEnd > 0) && (pageEnd == lastPage));

		if (pageStart != pageEnd)
		{
			if (isLastPage)
			{
				start %= background.w;

				if ((this->highlightedTextLength > 0) && (std::abs(pageEnd - pageStart) > 1)) {
					start = 0;
					width = ((startWidth % background.w) + lastPageOffset);
				} else if (this->highlightedTextLength > 0) {
					start = std::max((start - (this->textSize.width % background.w)), 0);
					width = ((startWidth % background.w) + lastPageOffset - start);
				} else if (this->highlightedTextLength < 0) {
					width = (background.w - start);
				}
			}
			else
			{
				if (this->highlightedTextLength > 0) {
					start = 0;
					width = (startWidth % background.w);
				} else {
					width = (background.w - (start % background.w));
					start = (background.w - width);
				}
			}
		}
		else 
		{
			start %= background.w;
			width %= background.w;

			if (isLastPage)
				start += lastPageOffset;
		}
	}

	SDL_Rect destination = {
		(background.x + start),
		(background.y + ((background.h - this->highlightedTextSize.height) / 2)),
		width,
		this->highlightedTextSize.height
	};

	this->renderHighlight(renderer, destination);
}

void LSG_TextInput::renderIconClear(SDL_Renderer* renderer, const SDL_Rect& icon)
{
	if (!this->active || this->value.empty())
		return;

	if (!this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR])
		this->setIconClear();

	SDL_RenderCopy(renderer, this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR], nullptr, &icon);

	if (this->enabled && this->highlightedIconClear)
		this->renderHighlight(renderer, icon);
}

void LSG_TextInput::renderText(SDL_Renderer* renderer, const SDL_Rect& background)
{
	auto showValue   = (this->active || !this->value.empty());
	auto texture     = this->textures[showValue ? LSG_TEXT_INPUT_TEXTURE_VALUE : LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER];
	auto textureSize = (showValue ? this->textSize : LSG_Graphics::GetTextureSize(texture));
	auto offsetX     = 0;

	if (showValue && (textureSize.width > background.w))
	{
		auto highlightedStart = (this->cursorTextWidth - this->highlightedTextSize.width);
		auto highlightedEnd   = (this->cursorTextWidth + this->highlightedTextSize.width);
		auto maxWidth         = (textureSize.width - background.w);

		if (this->highlightedTextLength > 0)
			offsetX = std::min(((highlightedEnd / background.w) * background.w), maxWidth);
		else if (this->highlightedTextLength < 0)
			offsetX = std::min(((highlightedStart / background.w) * background.w), maxWidth);
		else if (this->cursorPosition > 0)
			offsetX = std::min(((this->cursorTextWidth / background.w) * background.w), maxWidth);
	}

	SDL_Rect clip = {
		offsetX,
		0,
		std::min(textureSize.width,  background.w),
		std::min(textureSize.height, background.h)
	};

	SDL_Rect destination = {
		background.x,
		(background.y + ((background.h - clip.h) / 2)),
		clip.w,
		clip.h
	};

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_TextInput::SelectAll()
{
	if (!this->active)
		return;

	this->cursorPosition        = 0;
	this->highlightedTextLength = (int)this->value.size();

	this->setCursor();
}

void LSG_TextInput::SelectEnd()
{
	if (!this->active)
		return;

	this->highlightedTextLength = ((int)this->value.size() - (int)this->cursorPosition);

	this->setCursor();
}

void LSG_TextInput::SelectHome()
{
	if (!this->active)
		return;

	this->highlightedTextLength = -((int)this->cursorPosition);

	this->setCursor();
}

void LSG_TextInput::SelectLeft()
{
	auto highlightPosition = (this->cursorPosition + this->highlightedTextLength);

	if (!this->active || (highlightPosition == 0))
		return;

	auto character      = this->value[highlightPosition - 1];
	int  characterWidth = (character < 0 ? 2 : 1);

	this->highlightedTextLength -= characterWidth;

	this->setCursor();
}

void LSG_TextInput::SelectRight()
{
	auto highlightPosition = (size_t)(this->cursorPosition + this->highlightedTextLength);

	if (!this->active || (highlightPosition == this->value.size()))
		return;

	auto character      = this->value[highlightPosition];
	int  characterWidth = (character < 0 ? 2 : 1);

	this->highlightedTextLength += characterWidth;

	this->setCursor();
}

void LSG_TextInput::SelectWord(const SDL_Point& mousePosition)
{
	if (!this->active)
		return;

	size_t start, end, min = 0, max = this->value.size();

	for (start = this->cursorPosition; start > min && (this->value[start - 1] != ' '); start--) {}
	for (end   = this->cursorPosition; end   < max && (this->value[end]       != ' '); end++)   {}

	this->cursorPosition        = start;
	this->highlightedTextLength = (int)(end - start);

	this->setCursor();
}

void LSG_TextInput::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event textInputEvent = {};

	textInputEvent.type       = SDL_RegisterEvents(1);
	textInputEvent.user.code  = (int)type;
	textInputEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&textInputEvent);
}

void LSG_TextInput::setCursor()
{
	if (this->value.empty()) {
		this->cursorTextWidth = 0;
		this->textSize        = {};
		return;
	}

	auto font         = LSG_Text::GetFontArial(this->getFontSize());
	auto cursorText16 = LSG_Text::ToUTF16(this->value.substr(0, this->cursorPosition));

	TTF_SizeUNICODE(font, cursorText16, &this->cursorTextWidth, nullptr);
	SDL_free(cursorText16);

	auto text16 = LSG_Text::ToUTF16(this->value);

	TTF_SizeUNICODE(font, text16, &this->textSize.width, &this->textSize.height);
	SDL_free(text16);

	if (this->highlightedTextLength != 0)
	{
		auto start  = std::min((this->cursorPosition + this->highlightedTextLength), this->cursorPosition);
		auto length = std::abs(this->highlightedTextLength);

		auto highlightedText16 = LSG_Text::ToUTF16(this->value.substr(start, length));

		TTF_SizeUNICODE(font, highlightedText16, &this->highlightedTextSize.width, &this->highlightedTextSize.height);
		SDL_free(highlightedText16);
	} else {
		this->highlightedTextSize = {};
	}

	TTF_CloseFont(font);
}
	
void LSG_TextInput::setIconClear()
{
	if (this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR]) {
		SDL_DestroyTexture(this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR]);
		this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR] = nullptr;
	}

	SDL_Size maxSize = { this->maxIconSize, this->maxIconSize };

	if (this->maxIconSize > 0)
		this->textures[LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR] = LSG_Graphics::GetVectorClose(this->textColor, maxSize);
}

void LSG_TextInput::setPlaceholder()
{
	if (this->textures[LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER]) {
		SDL_DestroyTexture(this->textures[LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER]);
		this->textures[LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER] = nullptr;
	}

	if (this->placeholder.empty())
		return;

	auto textColor = SDL_Color(this->textColor);
	textColor.a    = (uint8_t)((double)textColor.a * 0.8);

	this->textures[LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER] = this->getTexture(this->placeholder, 0, &textColor);
}

void LSG_TextInput::SetText()
{
	this->setIconClear();
	this->setPlaceholder();
	this->setValue();
}

void LSG_TextInput::SetValue(const std::string& text)
{
	if (!this->visible)
		return;

	this->value                 = text;
	this->cursorPosition        = this->value.size();
	this->highlightedTextLength = 0;

	this->setValue();
}

void LSG_TextInput::setValue()
{
	if (this->textures[LSG_TEXT_INPUT_TEXTURE_VALUE]) {
		SDL_DestroyTexture(this->textures[LSG_TEXT_INPUT_TEXTURE_VALUE]);
		this->textures[LSG_TEXT_INPUT_TEXTURE_VALUE] = nullptr;
	}

	if (!this->value.empty())
		this->textures[LSG_TEXT_INPUT_TEXTURE_VALUE] = this->getTexture(this->value);

	this->setCursor();
}

void LSG_TextInput::start()
{
	if (this->active)
		return;

	this->active           = true;
	this->lastCursorActive = (SDL_GetTicks() - 500);

	SDL_SetTextInputRect(&this->background);
	SDL_StartTextInput();
}

void LSG_TextInput::Stop()
{
	if (!this->active)
		return;

	this->active = false;

	SDL_StopTextInput();
}
