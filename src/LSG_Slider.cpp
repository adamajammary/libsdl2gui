#include "LSG_Slider.h"

LSG_Slider::LSG_Slider(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->fillProgress     = false;
	this->isSlideActive    = false;
	this->orientation      = "";
	this->progressColor    = {};
	this->thumb            = {};
	this->thumbBorder      = 0;
	this->thumbBorderColor = {};
	this->thumbColor       = {};
	this->thumbWidth       = LSG_SLIDER_THUMB_WIDTH;
	this->value            = 0.0;

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("fill-progress"))
		this->fillProgress = (attributes["fill-progress"] == "true");

	if (attributes.contains("orientation"))
		this->orientation = attributes["orientation"];

	if (attributes.contains("thumb-border"))
		this->thumbBorder = std::atoi(attributes["thumb-border"].c_str());

	if (attributes.contains("thumb-width"))
		this->thumbWidth = std::atoi(attributes["thumb-width"].c_str());

	if (attributes.contains("value"))
		this->value = std::atof(attributes["value"].c_str());

	auto progressColor    = this->getXmlColor("progress-color");
	auto thumbColor       = this->getXmlColor("thumb-color");
	auto thumbBorderColor = this->getXmlColor("thumb-border-color");

	if (!progressColor.empty())
		this->progressColor = LSG_UI::ToSdlColor(progressColor);

	if (!thumbColor.empty())
		this->thumbColor = LSG_UI::ToSdlColor(thumbColor);

	if (!thumbBorderColor.empty())
		this->thumbBorderColor = LSG_UI::ToSdlColor(thumbBorderColor);
}

bool LSG_Slider::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	auto mousePosition = SDL_Point(event.x, event.y);

	this->isSlideActive = false;

	if (!SDL_PointInRect(&mousePosition, &this->thumb))
		this->setValue(mousePosition);
	else
		this->isSlideActive = true;

	return true;
}

void LSG_Slider::MouseMove(const SDL_Point& mousePosition)
{
	if (LSG_Events::IsMouseDown() && this->isSlideActive)
		this->setValue(mousePosition);
}

void LSG_Slider::MouseScroll(int offset)
{
	this->setValue(offset);
}

void LSG_Slider::MouseUp()
{
	this->isSlideActive = false;
}

void LSG_Slider::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	auto backgroundArea = SDL_Rect(this->background);
	bool isVertical     = (this->orientation == LSG_VERTICAL);
	auto thumbWidth     = max(LSG_SLIDER_THUMB_WIDTH, this->thumbWidth);
	auto thumbWidthHalf = (thumbWidth / 2);

	if (isVertical) {
		backgroundArea.y += thumbWidthHalf;
		backgroundArea.h -= thumbWidth;
		backgroundArea.x += (LSG_SLIDER_THUMB_WIDTH / 2);
		backgroundArea.w -= LSG_SLIDER_THUMB_WIDTH;
	} else {
		backgroundArea.x += thumbWidthHalf;
		backgroundArea.w -= thumbWidth;
		backgroundArea.y += (LSG_SLIDER_THUMB_WIDTH / 2);
		backgroundArea.h -= LSG_SLIDER_THUMB_WIDTH;
	}

	auto fillArea = this->getFillArea(backgroundArea, this->border);

	SDL_SetRenderDrawBlendMode(renderer, this->backgroundColor.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, this->backgroundColor.r, this->backgroundColor.g, this->backgroundColor.b, this->backgroundColor.a);

	SDL_RenderFillRect(renderer, &fillArea);

	this->renderBorder(renderer, this->border, this->borderColor, backgroundArea);

	auto progressValue = (int)((double)(isVertical ? backgroundArea.h : backgroundArea.w) * this->value);

	if (this->fillProgress)
	{
		SDL_Rect progressArea = SDL_Rect(fillArea);

		if (isVertical) {
			progressArea.h  = progressValue;
			progressArea.y += (backgroundArea.h - progressValue);
		} else {
			progressArea.w = progressValue;
		}

		SDL_SetRenderDrawColor(renderer, this->progressColor.r, this->progressColor.g, this->progressColor.b, this->progressColor.a);
		SDL_RenderFillRect(renderer, &progressArea);
	}

	this->thumb = SDL_Rect(backgroundArea);

	if (isVertical) {
		this->thumb.y += (backgroundArea.h - progressValue - thumbWidthHalf);
		this->thumb.h  = thumbWidth;
		this->thumb.x  = this->background.x;
		this->thumb.w  = this->background.w;
	} else {
		this->thumb.x += (progressValue - thumbWidthHalf);
		this->thumb.w  = thumbWidth;
		this->thumb.y  = this->background.y;
		this->thumb.h  = this->background.h;
	}

	SDL_SetRenderDrawColor(renderer, this->thumbColor.r, this->thumbColor.g, this->thumbColor.b, this->thumbColor.a);
	SDL_RenderFillRect(renderer, &this->thumb);

	this->renderBorder(renderer, this->thumbBorder, this->thumbBorderColor, this->thumb);

	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
}

void LSG_Slider::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	this->value = max(0.0, min(1.0, this->value));

	SDL_Event sliderEvent = {};

	sliderEvent.type       = SDL_RegisterEvents(1);
	sliderEvent.user.code  = (int)type;
	sliderEvent.user.data1 = (void*)strdup(this->GetID().c_str());
	sliderEvent.user.data2 = (void*)&this->value;

	SDL_PushEvent(&sliderEvent);
}

void LSG_Slider::SetColors()
{
	LSG_Component::SetColors();

	auto backgroundColor  = this->getXmlColor("background-color", false);
	auto progressColor    = this->getXmlColor("progress-color");
	auto thumbColor       = this->getXmlColor("thumb-color");
	auto thumbBorderColor = this->getXmlColor("thumb-border-color");

	this->backgroundColor  = (!backgroundColor.empty()  ? LSG_UI::ToSdlColor(backgroundColor)  : LSG_SLIDER_BACK_COLOR);
	this->progressColor    = (!progressColor.empty()    ? LSG_UI::ToSdlColor(progressColor)    : LSG_SLIDER_PROGRESS_COLOR);
	this->thumbColor       = (!thumbColor.empty()       ? LSG_UI::ToSdlColor(thumbColor)       : LSG_SLIDER_THUMB_COLOR);
	this->thumbBorderColor = (!thumbBorderColor.empty() ? LSG_UI::ToSdlColor(thumbBorderColor) : LSG_DEFAULT_BORDER_COLOR);
}

void LSG_Slider::setValue(const SDL_Point& mousePosition)
{
	if (this->orientation == LSG_VERTICAL)
		this->value = (double)((double)(this->background.y + this->background.h - mousePosition.y) / (double)this->background.h);
	else
		this->value = (double)((double)(mousePosition.x - this->background.x) / (double)this->background.w);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::setValue(int offset)
{
	if (this->orientation == LSG_VERTICAL)
		this->value = (double)((double)((int)((double)this->background.h * this->value) - offset) / (double)this->background.h);
	else
		this->value = (double)((double)((int)((double)this->background.w * this->value) - offset) / (double)this->background.w);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::SetValue(double value)
{
	this->value = max(0.0, min(1.0, value));

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}
