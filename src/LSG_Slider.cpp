#include "LSG_Slider.h"

LSG_Slider::LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_ProgressBar(id, layer, xmlNode, xmlNodeName, parent)
{
	this->barWidth      = 0;
	this->fillProgress  = false;
	this->isSlideActive = false;
	this->partSize      = LSG_Window::GetDPIScaled(LSG_Slider::DefaultPartSize);
	this->parts         = {};
	this->thumb         = {};

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("bar-width"))
		this->barWidth = LSG_Window::GetDPIScaled(std::atoi(attributes["bar-width"].c_str()));

	if (attributes.contains("fill-progress"))
		this->fillProgress = (attributes["fill-progress"] == "true");

	if (attributes.contains("parts"))
	{
		auto xmlParts = LSG_Text::Split(attributes["parts"], ',');

		for (const auto& part : xmlParts)
			this->parts.push_back(std::atof(part.c_str()));
	}

	if (attributes.contains("thumb-border-width"))
		this->thumb.borderWidth = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-border-width"].c_str()));

	if (attributes.contains("thumb-size"))
		this->thumb.size = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-size"].c_str()));

	this->SetColors();
}

SDL_Rect LSG_Slider::getBar() const
{
	bool isVertical = this->IsVertical();
	auto barWidth   = this->getBarWidth(isVertical);
	auto thumbSize  = this->getThumbSize(isVertical);

	auto bar = SDL_Rect(this->background);

	if (isVertical)
	{
		bar.x += ((this->background.w - barWidth) / 2);
		bar.w  = barWidth;

		bar.y += (thumbSize / 2);
		bar.h -= thumbSize;
	}
	else
	{
		bar.y += ((this->background.h - barWidth) / 2);
		bar.h  = barWidth;

		bar.x += (thumbSize / 2);
		bar.w -= thumbSize;
	}

	return bar;
}

int LSG_Slider::getBarWidth(bool isVertical) const
{
	int barWidth;

	if (isVertical)
		barWidth = (this->barWidth > 0 ? std::min(this->barWidth, this->background.w) : this->background.w);
	else
		barWidth = (this->barWidth > 0 ? std::min(this->barWidth, this->background.h) : this->background.h);

	return barWidth;
}

std::vector<double> LSG_Slider::GetParts() const
{
	return this->parts;
}

int LSG_Slider::getProgressWidth() const
{
	auto bar = this->getBar();

	return (int)((double)(this->IsVertical() ? bar.h : bar.w) * this->value);
}

SDL_Rect LSG_Slider::getThumb() const
{
	bool isVertical = this->IsVertical();

	auto progress  = this->getProgressWidth();
	auto thumbSize = this->getThumbSize(isVertical);

	auto thumb = SDL_Rect(this->background);

	if (isVertical)
	{
		thumb.y += (this->background.h - progress - thumbSize);
		thumb.x += ((this->background.w - thumbSize) / 2);

		thumb.w = thumbSize;
		thumb.h = thumbSize;
	}
	else
	{
		thumb.x += progress;
		thumb.y += ((this->background.h - thumbSize) / 2);

		thumb.w = thumbSize;
		thumb.h = thumbSize;
	}

	return thumb;
}

int LSG_Slider::getThumbSize(bool isVertical) const
{
	int thumbSize;

	if (isVertical)
		thumbSize = (this->thumb.size > 0 ? std::min(this->thumb.size, this->background.w) : this->background.w);
	else
		thumbSize = (this->thumb.size > 0 ? std::min(this->thumb.size, this->background.h) : this->background.h);

	return thumbSize;
}

void LSG_Slider::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return;

	auto thumb = this->getThumb();

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

	auto thumb = this->getThumb();

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

	auto bar = this->getBar();

	this->renderBar(renderer, bar);

	if (this->fillProgress)
	{
		auto fillArea      = LSG_Graphics::GetFillArea(bar, this->borderWidth);
		auto progressArea  = SDL_Rect(fillArea);
		auto progressWidth = this->getProgressWidth();

		if (this->IsVertical()) {
			progressArea.h  = progressWidth;
			progressArea.y += (bar.h - progressWidth);
		} else {
			progressArea.w = progressWidth;
		}

		this->renderProgress(renderer, progressArea, progressWidth, fillArea);
	}

	if (!this->parts.empty())
		this->renderParts(renderer, bar);

	this->renderThumb(renderer);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_Slider::renderBar(SDL_Renderer* renderer, const SDL_Rect& bar) const
{
	if (this->borderRadius > 0)
	{
		LSG_Graphics::RenderFillWithRoundedBorder(
			renderer,
			this->backgroundColor,
			this->borderColor,
			this->borderRadius,
			this->borderWidth,
			bar,
			std::format("{}_bar", this->id)
		);
	} else {
		LSG_Graphics::RenderFill(renderer,   this->borderWidth, this->backgroundColor, bar);
		LSG_Graphics::RenderBorder(renderer, this->borderWidth, this->borderColor,     bar);
	}
}

void LSG_Slider::renderParts(SDL_Renderer* renderer, const SDL_Rect& bar) const
{
	if (this->parts.empty())
		return;

	bool isVertical     = this->IsVertical();
	auto backgroundSize = (isVertical ? bar.h : bar.w);

	for (auto percent : this->parts)
	{
		auto position    = (int)((double)backgroundSize * percent);
		auto maxPosition = (backgroundSize - this->borderRadius - this->partSize);

		if ((position < this->borderRadius) || (position > maxPosition))
			continue;

		SDL_Rect partArea = bar;

		if (isVertical) {
			partArea.y += (bar.h - position - this->partSize);
			partArea.h  = this->partSize;
		} else {
			partArea.x += position;
			partArea.w  = this->partSize;
		}

		LSG_Graphics::RenderFill(renderer, 0, this->parent->backgroundColor, partArea);
	}
}

void LSG_Slider::renderThumb(SDL_Renderer* renderer) const
{
	auto id    = std::format("{}_thumb", this->id);
	auto thumb = this->getThumb();

	if (this->thumb.borderWidth <= 0) {
		LSG_Graphics::RenderFillCircle(renderer, this->thumb.color, thumb, id);
		return;
	}

	LSG_Graphics::RenderFillCircleWithBorder(
		renderer,
		this->thumb.color,
		thumb,
		this->thumb.borderColor,
		this->thumb.borderWidth,
		id
	);
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

void LSG_Slider::SetParts(const std::vector<double>& parts)
{
	this->parts = parts;
}

void LSG_Slider::setValue(const SDL_Point& mousePosition)
{
	auto bar = this->getBar();

	double value;

	if (this->IsVertical())
		value = (double)((double)(bar.y + bar.h - mousePosition.y) / (double)bar.h);
	else
		value = (double)((double)(mousePosition.x - bar.x) / (double)bar.w);

	this->SetValue(value);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}

void LSG_Slider::setValue(int offset)
{
	auto bar = this->getBar();

	double value;

	if (this->IsVertical())
		value = (double)((double)((int)((double)bar.h * this->value) - offset) / (double)bar.h);
	else
		value = (double)((double)((int)((double)bar.w * this->value) - offset) / (double)bar.w);

	this->SetValue(value);

	this->sendEvent(LSG_EVENT_SLIDER_VALUE_CHANGED);
}
