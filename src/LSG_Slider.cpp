#include "LSG_Slider.h"

LSG_Slider::LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_ProgressBar(id, layer, xmlNode, xmlNodeName, parent)
{
	this->barWidth      = 0;
	this->fillProgress  = false;
	this->isSlideActive = false;
	this->onHoverCB     = nullptr;
	this->partSize      = LSG_Window::GetDPIScaled(LSG_Slider::DefaultPartSize);
	this->parts         = {};
	this->thumb         = {};

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("bar-width"))
		this->barWidth = LSG_Window::GetDPIScaled(std::atoi(attributes["bar-width"].c_str()));

	if (attributes.contains("fill-progress"))
		this->fillProgress = (attributes["fill-progress"] == "true");

	if (attributes.contains("thumb-border-width"))
		this->thumb.borderWidth = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-border-width"].c_str()));

	if (attributes.contains("thumb-size"))
		this->thumb.size = LSG_Window::GetDPIScaled(std::atoi(attributes["thumb-size"].c_str()));

	this->SetColors();
}

void LSG_Slider::AddPart(LibXml::xmlNode* node)
{
	auto xmlAttributes = LSG_XML::GetAttributes(node);

	LSG_SliderPart part = {
		.value   = (xmlAttributes.contains("value") ? std::atof(xmlAttributes["value"].c_str()) : 0.0),
		.tooltip = (xmlAttributes.contains("tooltip") ? xmlAttributes["tooltip"] : "")
	};

	this->parts.push_back(LSG_SliderPartItem(part));
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

LSG_SliderParts LSG_Slider::GetParts() const
{
	LSG_SliderParts parts;

	for (auto const& part : this->parts)
		parts.push_back({ .value = part.value, .tooltip = part.tooltip });

	return parts;
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

std::string LSG_Slider::getTooltip(const LSG_SliderTooltip& tooltip) const
{
	if (!tooltip.text.empty() && !tooltip.onHoverText.empty())
		return std::format("{} - {}", tooltip.text, tooltip.onHoverText);
	else if (!tooltip.text.empty())
		return tooltip.text;
	else if (!tooltip.onHoverText.empty())
		return tooltip.onHoverText;

	return "";
}

double LSG_Slider::GetValue(const SDL_Point& mousePosition) const
{
	auto bar = this->getBar();

	double value;

	if (this->IsVertical())
		value = (double)((double)(bar.y + bar.h - mousePosition.y) / (double)bar.h);
	else
		value = (double)((double)(mousePosition.x - bar.x) / (double)bar.w);

	return std::max(0.0, std::min(1.0, value));
}

bool LSG_Slider::isMouseOverPart(const SDL_Point& mousePosition, const SDL_Rect& bar, size_t index) const
{
	if (index >= this->parts.size())
		return false;

	bool isLastPart = (index == (this->parts.size() - 1));
	auto partDest   = SDL_Rect(this->parts[index].destination);

	if (this->IsVertical())
	{
		auto nextPartY = (!isLastPart ? this->parts[index + 1].destination.y : bar.y);

		partDest.h = (partDest.y - nextPartY);
		partDest.y = nextPartY;
	}
	else
	{
		auto nextPartX = (!isLastPart ? this->parts[index + 1].destination.x : (bar.x + bar.w));

		partDest.w = (nextPartX - partDest.x);
	}

	return SDL_PointInRect(&mousePosition, &partDest);
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

void LSG_Slider::renderParts(SDL_Renderer* renderer, const SDL_Rect& bar)
{
	if (this->parts.empty())
		return;

	bool isVertical     = this->IsVertical();
	auto backgroundSize = (isVertical ? bar.h : bar.w);

	for (auto& part : this->parts)
	{
		auto position    = (int)((double)backgroundSize * part.value);
		auto minPosition = (this->borderRadius + this->partSize);
		auto maxPosition = (backgroundSize - (minPosition + this->partSize));

		if (position > maxPosition)
			position = maxPosition;

		part.destination = SDL_Rect(bar);

		if (isVertical) {
			part.destination.y = (bar.y + bar.h - position - this->partSize);
			part.destination.h = this->partSize;
		} else {
			part.destination.x += position;
			part.destination.w  = this->partSize;
		}

		if (position >= minPosition)
			LSG_Graphics::RenderFill(renderer, 0, this->parent->backgroundColor, part.destination);
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

void LSG_Slider::RenderTooltip(SDL_Renderer* renderer) const
{
	if (!this->highlighted) {
		LSG_Graphics::DestroyTextures(std::format("{}_tooltip", this->id));
		return;
	}

	auto bar            = this->getBar();
	auto onHoverTooltip = (this->onHoverCB != nullptr ? this->onHoverCB() : "");
	auto mousePosition  = LSG_Window::GetMousePosition();

	for (size_t i = 0; i < this->parts.size(); i++)
	{
		if (!this->isMouseOverPart(mousePosition, bar, i))
			continue;

		LSG_SliderTooltip tooltip = {
			.id            = std::format("{}_tooltip_part{}", this->id, i),
			.mousePosition = mousePosition,
			.onHoverText   = onHoverTooltip,
			.text          = this->parts[i].tooltip
		};

		this->renderTooltip(renderer, tooltip);

		return;
	}

	LSG_SliderTooltip tooltip = {
		.id            = std::format("{}_tooltip", this->id),
		.mousePosition = mousePosition,
		.onHoverText   = onHoverTooltip,
		.text          = this->tooltip
	};

	this->renderTooltip(renderer, tooltip);
}

void LSG_Slider::renderTooltip(SDL_Renderer* renderer, const LSG_SliderTooltip& tooltip) const
{
	auto tooltipText = this->getTooltip(tooltip);

	if (tooltipText.empty())
		return;

	auto tooltipId = std::format("{}_{}", tooltip.id, std::hash<std::string>{}(tooltipText));

	LSG_Graphics::RenderTooltip(renderer, tooltipText, tooltip.mousePosition, tooltipId);
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

void LSG_Slider::SetOnHoverCallback(const LSG_OnHoverCallback& callback)
{
	this->onHoverCB = callback;
}

void LSG_Slider::SetParts(const LSG_SliderParts& parts)
{
	LSG_SliderPartItems partItems;

	for (auto const& part : parts)
		partItems.push_back(LSG_SliderPartItem(part));

	this->parts = partItems;
}

void LSG_Slider::setValue(const SDL_Point& mousePosition)
{
	auto value = this->GetValue(mousePosition);

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
