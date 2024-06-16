#include "LSG_Slider.h"

LSG_Slider::LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->fillProgress      = false;
	this->isSlideActive     = false;
	this->orientation       = "";
	this->progressColor     = {};
	this->thumb             = {};
	this->thumbBorder       = 0;
	this->thumbBorderColor  = {};
	this->thumbColor        = {};
	this->thumbWidthDefault = LSG_Graphics::GetDPIScaled(LSG_Slider::DefaultThumbWidth);
	this->thumbWidth        = this->thumbWidthDefault;
	this->value             = 0.0;

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("fill-progress"))
		this->fillProgress = (attributes["fill-progress"] == "true");

	if (attributes.contains("orientation"))
		this->orientation = attributes["orientation"];

	if (attributes.contains("thumb-border"))
		this->thumbBorder = std::atoi(attributes["thumb-border"].c_str());

	if (attributes.contains("thumb-width"))
		this->thumbWidth = LSG_Graphics::GetDPIScaled(std::atoi(attributes["thumb-width"].c_str()));

	if (attributes.contains("value"))
		this->value = std::atof(attributes["value"].c_str());

	auto progressColor    = this->getXmlColor("progress-color");
	auto thumbColor       = this->getXmlColor("thumb-color");
	auto thumbBorderColor = this->getXmlColor("thumb-border-color");

	if (!progressColor.empty())
		this->progressColor = LSG_Graphics::ToSdlColor(progressColor);

	if (!thumbColor.empty())
		this->thumbColor = LSG_Graphics::ToSdlColor(thumbColor);

	if (!thumbBorderColor.empty())
		this->thumbBorderColor = LSG_Graphics::ToSdlColor(thumbBorderColor);
}

double LSG_Slider::GetValue() const
{
	return this->value;
}

bool LSG_Slider::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	if (!SDL_PointInRect(&mousePosition, &this->thumb)) {
		this->setValue(mousePosition);
		return true;
	}

	this->isSlideActive = false;

	return false;
}

bool LSG_Slider::OnMouseClickThumb(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	if (SDL_PointInRect(&mousePosition, &this->thumb)) {
		this->isSlideActive = true;
		return true;
	}

	this->isSlideActive = false;

	return false;
}

bool LSG_Slider::OnMouseMove(const SDL_Point& mousePosition)
{
	if (!LSG_Events::IsMouseDown() || !this->isSlideActive)
		return false;

	this->setValue(mousePosition);

	return true;
}

void LSG_Slider::OnMouseScroll(int offset)
{
	this->setValue(offset);
}

void LSG_Slider::OnMouseUp()
{
	this->isSlideActive = false;
}

void LSG_Slider::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	auto backgroundArea = SDL_Rect(this->background);
	bool isVertical     = this->IsVertical();
	auto thumbWidth     = std::max(this->thumbWidthDefault, this->thumbWidth);
	auto thumbWidthHalf = (thumbWidth / 2);

	if (isVertical) {
		backgroundArea.y += thumbWidthHalf;
		backgroundArea.h -= thumbWidth;
		backgroundArea.x += (this->thumbWidthDefault / 2);
		backgroundArea.w -= this->thumbWidthDefault;
	} else {
		backgroundArea.x += thumbWidthHalf;
		backgroundArea.w -= thumbWidth;
		backgroundArea.y += (this->thumbWidthDefault / 2);
		backgroundArea.h -= this->thumbWidthDefault;
	}

	auto fillArea = this->getFillArea(backgroundArea, this->border);

	this->renderFill(renderer,   this->border, this->backgroundColor, backgroundArea);
	this->renderBorder(renderer, this->border, this->borderColor,     backgroundArea);

	auto progressValue = (int)((double)(isVertical ? backgroundArea.h : backgroundArea.w) * this->value);

	if (this->fillProgress)
	{
		SDL_Rect progressArea = fillArea;

		if (isVertical) {
			progressArea.h  = progressValue;
			progressArea.y += (backgroundArea.h - progressValue);
		} else {
			progressArea.w = progressValue;
		}

		this->renderFill(renderer, 0, this->progressColor, progressArea);
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

	this->renderFill(renderer,   this->thumbBorder, this->thumbColor,       this->thumb);
	this->renderBorder(renderer, this->thumbBorder, this->thumbBorderColor, this->thumb);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_Slider::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	this->value = std::max(0.0, std::min(1.0, this->value));

	SDL_Event sliderEvent = {};

	sliderEvent.type       = SDL_RegisterEvents(1);
	sliderEvent.user.code  = (int)type;
	sliderEvent.user.data1 = (void*)strdup(this->id.c_str());
	sliderEvent.user.data2 = new double(this->value);

	SDL_PushEvent(&sliderEvent);
}

void LSG_Slider::SetColors()
{
	LSG_Component::SetColors();

	auto backgroundColor  = this->getXmlColor("background-color", false);
	auto progressColor    = this->getXmlColor("progress-color");
	auto thumbColor       = this->getXmlColor("thumb-color");
	auto thumbBorderColor = this->getXmlColor("thumb-border-color");

	this->backgroundColor  = (!backgroundColor.empty()  ? LSG_Graphics::ToSdlColor(backgroundColor)  : LSG_Slider::DefaultBackgroundColor);
	this->progressColor    = (!progressColor.empty()    ? LSG_Graphics::ToSdlColor(progressColor)    : LSG_Slider::DefaultProgressColor);
	this->thumbColor       = (!thumbColor.empty()       ? LSG_Graphics::ToSdlColor(thumbColor)       : LSG_Slider::DefaultThumbColor);
	this->thumbBorderColor = (!thumbBorderColor.empty() ? LSG_Graphics::ToSdlColor(thumbBorderColor) : LSG_ConstDefaultColor::Border);
}

void LSG_Slider::setValue(const SDL_Point& mousePosition)
{
	auto background = LSG_UI::GetScrolledBackground(this->background, this->parent);

	if (this->IsVertical())
		this->value = (double)((double)(background.y + background.h - mousePosition.y) / (double)background.h);
	else
		this->value = (double)((double)(mousePosition.x - background.x) / (double)background.w);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::setValue(int offset)
{
	if (this->IsVertical())
		this->value = (double)((double)((int)((double)this->background.h * this->value) - offset) / (double)this->background.h);
	else
		this->value = (double)((double)((int)((double)this->background.w * this->value) - offset) / (double)this->background.w);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::SetValue(double value)
{
	this->value = std::max(0.0, std::min(1.0, value));
}
