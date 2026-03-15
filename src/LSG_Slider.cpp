#include "LSG_Slider.h"

LSG_Slider::LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_ProgressBar(id, layer, xmlNode, xmlNodeName, parent)
{
	this->fillProgress  = false;
	this->isSlideActive = false;
	this->thumb         = {};

	this->thumb.widthDefault = LSG_Window::GetDPIScaled(LSG_Slider::DefaultThumbWidth);

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("fill-progress"))
		this->fillProgress = (attributes["fill-progress"] == "true");

	if (attributes.contains("thumb-border-radius"))
		this->thumb.borderRadius = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-border-radius"].c_str()));

	if (attributes.contains("thumb-border-width"))
		this->thumb.borderWidth = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-border-width"].c_str()));

	if (attributes.contains("thumb-width"))
		this->thumb.width = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-width"].c_str()));
	else
		this->thumb.width = this->thumb.widthDefault;

	this->SetColors();
}

SDL_Rect LSG_Slider::getBackground() const
{
	auto background = SDL_Rect(this->background);
	bool isVertical = this->IsVertical();

	auto thumbWidth     = std::max(this->thumb.widthDefault, this->thumb.width);
	auto thumbWidthHalf = (thumbWidth / 2);

	if (isVertical) {
		background.y += thumbWidthHalf;
		background.h -= thumbWidth;
		background.x += (this->thumb.widthDefault / 2);
		background.w -= this->thumb.widthDefault;
	} else {
		background.x += thumbWidthHalf;
		background.w -= thumbWidth;
		background.y += (this->thumb.widthDefault / 2);
		background.h -= this->thumb.widthDefault;
	}

	return background;
}

int LSG_Slider::getProgressValue(const SDL_Rect& background) const
{
	return (int)((double)(this->IsVertical() ? background.h : background.w) * this->value);
}

SDL_Rect LSG_Slider::getThumb(const SDL_Rect& background, int progressValue) const
{
	SDL_Rect thumb = background;

	auto thumbWidth     = std::max(this->thumb.widthDefault, this->thumb.width);
	auto thumbWidthHalf = (thumbWidth / 2);

	if (this->IsVertical()) {
		thumb.y += (background.h - progressValue - thumbWidthHalf);
		thumb.h = thumbWidth;
		thumb.x = this->background.x;
		thumb.w = this->background.w;
	} else {
		thumb.x += (progressValue - thumbWidthHalf);
		thumb.w = thumbWidth;
		thumb.y = this->background.y;
		thumb.h = this->background.h;
	}

	return thumb;
}

void LSG_Slider::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return;

	auto background = this->getBackground();
	auto thumb      = this->getThumb(background, this->getProgressValue(background));

	if (!SDL_PointInRect(&mousePosition, &thumb)) {
		this->setValue(mousePosition);
		return;
	}

	this->isSlideActive = false;
}

bool LSG_Slider::OnMouseClickThumb(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	auto background = this->getBackground();
	auto thumb      = this->getThumb(background, this->getProgressValue(background));

	if (SDL_PointInRect(&mousePosition, &thumb)) {
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

void LSG_Slider::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_Slider::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Slider::render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	auto background = this->getBackground();

	this->renderBackground(renderer, background);

	auto progressValue = this->getProgressValue(background);

	if (this->fillProgress)
	{
		auto fillArea     = LSG_Graphics::GetFillArea(background, this->borderWidth);
		auto progressArea = SDL_Rect(fillArea);

		if (this->IsVertical()) {
			progressArea.h  = progressValue;
			progressArea.y += (background.h - progressValue);
		} else {
			progressArea.w = progressValue;
		}

		this->renderProgress(renderer, progressArea, progressValue, fillArea);
	}

	this->renderThumb(renderer, background, progressValue);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_Slider::renderBackground(SDL_Renderer* renderer, const SDL_Rect& background)
{
	if (this->borderRadius > 0)
	{
		LSG_Graphics::RenderFillWithRoundedBorder(
			renderer,
			this->backgroundColor,
			this->borderColor,
			this->borderRadius,
			this->borderWidth,
			background,
			std::format("{}_background", this->id)
		);
	} else {
		LSG_Graphics::RenderFill(renderer,   this->borderWidth, this->backgroundColor, background);
		LSG_Graphics::RenderBorder(renderer, this->borderWidth, this->borderColor,     background);
	}
}

void LSG_Slider::renderThumb(SDL_Renderer* renderer, const SDL_Rect& background, int progressValue)
{
	auto thumb = this->getThumb(background, progressValue);

	if (this->thumb.borderRadius > 0)
	{
		LSG_Graphics::RenderFillWithRoundedBorder(
			renderer,
			this->thumb.color,
			this->thumb.borderColor,
			this->thumb.borderRadius,
			this->thumb.borderWidth,
			thumb,
			std::format("{}_thumb", this->id)
		);
	} else {
		LSG_Graphics::RenderFill(renderer,   this->thumb.borderWidth, this->thumb.color,       thumb);
		LSG_Graphics::RenderBorder(renderer, this->thumb.borderWidth, this->thumb.borderColor, thumb);
	}
}

void LSG_Slider::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event sliderEvent = {};

	sliderEvent.type       = SDL_RegisterEvents(1);
	sliderEvent.user.code  = (int)type;
	sliderEvent.user.data1 = (void*)strdup(this->id.c_str());
	sliderEvent.user.data2 = new double(this->value);

	SDL_PushEvent(&sliderEvent);
}

void LSG_Slider::SetColors()
{
	LSG_ProgressBar::SetColors();

	auto thumbColor       = this->getXmlColor("thumb-color",        false);
	auto thumbBorderColor = this->getXmlColor("thumb-border-color", false);

	this->thumb.color       = (!thumbColor.empty()       ? LSG_Graphics::ToSdlColor(thumbColor)       : LSG_Slider::DefaultThumbColor);
	this->thumb.borderColor = (!thumbBorderColor.empty() ? LSG_Graphics::ToSdlColor(thumbBorderColor) : LSG_ConstDefaultColor::Border);
}

void LSG_Slider::setValue(const SDL_Point& mousePosition)
{
	double value;

	if (this->IsVertical())
		value = (double)((double)(this->background.y + this->background.h - mousePosition.y) / (double)this->background.h);
	else
		value = (double)((double)(mousePosition.x - this->background.x) / (double)this->background.w);

	this->SetValue(value);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::setValue(int offset)
{
	double value;

	if (this->IsVertical())
		value = (double)((double)((int)((double)this->background.h * this->value) - offset) / (double)this->background.h);
	else
		value = (double)((double)((int)((double)this->background.w * this->value) - offset) / (double)this->background.w);

	this->SetValue(value);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}
