#include "LSG_Navigation.h"

LSG_Navigation::LSG_Navigation(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->arrow        = {};
	this->borderRadius = 0;
	this->borderWidth  = 0;
	this->position     = 0;
	this->textSize     = {};

	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto xmlItemsPerNavigation = (xmlAttributes.contains("items-per-navigation") ? xmlAttributes["items-per-navigation"] : "");
	auto xmlItemsTotal         = (xmlAttributes.contains("items-total")          ? xmlAttributes["items-total"]          : "");

	this->items.perNavigation = (!xmlItemsPerNavigation.empty() ? std::atoll(xmlItemsPerNavigation.c_str()) : 1);
	this->items.total         = (!xmlItemsTotal.empty()         ? std::atoll(xmlItemsTotal.c_str())         : 0);

	this->canNavigate = this->getCanNavigate();

	this->text = this->getText();
}

LSG_Navigation::~LSG_Navigation()
{
	this->destroyTextures();
}

void LSG_Navigation::destroyTexture(SDL_Texture* &texture)
{
	if (texture) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}

void LSG_Navigation::destroyTextures()
{
	LSG_Component::destroyTextures();

	this->destroyTexture(this->arrow.back);
	this->destroyTexture(this->arrow.end);
	this->destroyTexture(this->arrow.home);
	this->destroyTexture(this->arrow.next);
}

SDL_Rect LSG_Navigation::getArrow(const SDL_Rect& destination, int padding) const
{
	SDL_Rect arrow       = destination;
	auto     paddingHalf = (padding / 2);

	arrow.x -= paddingHalf;
	arrow.y -= paddingHalf;
	arrow.w += padding;
	arrow.h += padding;

	return arrow;
}

SDL_Rect LSG_Navigation::getArrowDestination(const SDL_Rect& fillArea, int padding) const
{
	SDL_Rect destination = {
		(fillArea.x + padding),
		(fillArea.y + ((fillArea.h - this->arrow.size) / 2)),
		this->arrow.size,
		this->arrow.size
	};

	return destination;
}

LSG_CanNavigate LSG_Navigation::getCanNavigate() const
{
	bool hasEnoughItems = (this->items.perNavigation < (int)this->items.total);

	LSG_CanNavigate canNavigate = {
		.back = (hasEnoughItems && (this->position >= this->items.perNavigation)),
		.next = (hasEnoughItems && (this->position < ((int)this->items.total - this->items.perNavigation)))
	};

	return canNavigate;
}

size_t LSG_Navigation::GetItemsTotal() const
{
	return this->items.total;
}

int LSG_Navigation::GetPosition() const
{
	return this->position;
}

std::string LSG_Navigation::getText() const
{
	if (!this->items.total)
		return "0";

	auto start = (this->position + 1);
	auto end   = std::min((this->position + (int)this->items.perNavigation), (int)this->items.total);

	if (end > start)
		return std::format("{} - {} / {}", start, end, this->items.total);

	return std::format("{} / {}", start, this->items.total);
}

bool LSG_Navigation::IsMouseOverArrow(const SDL_Point& mousePosition) const
{
	auto fillArea = this->getFillArea();
	auto padding  = LSG_Window::GetDPIScaled(LSG_Navigation::ArrowPadding);

	auto destination = this->getArrowDestination(fillArea, padding);
	auto arrow       = this->getArrow(destination, padding);

	if (this->canNavigate.back && SDL_PointInRect(&mousePosition, &arrow))
		return true;

	destination.x += (this->arrow.size + padding);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.back && SDL_PointInRect(&mousePosition, &arrow))
		return true;

	destination.x = (fillArea.x + fillArea.w - padding - this->arrow.size);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.next && SDL_PointInRect(&mousePosition, &arrow))
		return true;

	destination.x -= (this->arrow.size + padding);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.next && SDL_PointInRect(&mousePosition, &arrow))
		return true;

	return false;
}

void LSG_Navigation::NavigateBack(const std::string& text)
{
	auto position = (this->position - (int)this->items.perNavigation);

	this->navigate(position, text);
}

void LSG_Navigation::NavigateEnd(const std::string& text)
{
	auto remainder = (int)(this->items.total % this->items.perNavigation);
	auto position  = (int)(this->items.total - (remainder == 0 ? this->items.perNavigation : remainder));

	this->navigate(position, text);
}

void LSG_Navigation::NavigateHome(const std::string& text)
{
	this->navigate(0, text);
}

void LSG_Navigation::NavigateNext(const std::string& text)
{
	auto position = (this->position + (int)this->items.perNavigation);

	this->navigate(position, text);
}

void LSG_Navigation::NavigateTo(int position, const std::string& text)
{
	this->navigate(position, text);
}

void LSG_Navigation::navigate(int position, const std::string& text)
{
	this->position = std::max(std::min(position, ((int)this->items.total - 1)), 0);

	this->canNavigate = this->getCanNavigate();

	if (!text.empty())
		this->text = text;
	else
		this->text = this->getText();

	this->destroyTextures();

	this->set();
}

void LSG_Navigation::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || !this->items.total)
		return;

	auto fillArea = this->getFillArea();
	auto padding  = LSG_Window::GetDPIScaled(LSG_Navigation::ArrowPadding);

	auto destination = this->getArrowDestination(fillArea, padding);
	auto arrow       = this->getArrow(destination, padding);

	if (this->canNavigate.back && SDL_PointInRect(&mousePosition, &arrow))
		this->sendEvent(LSG_EVENT_NAVIGATE_HOME);

	destination.x += (this->arrow.size + padding);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.back && SDL_PointInRect(&mousePosition, &arrow))
		this->sendEvent(LSG_EVENT_NAVIGATE_BACK);

	destination.x = (fillArea.x + fillArea.w - padding - this->arrow.size);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.next && SDL_PointInRect(&mousePosition, &arrow))
		this->sendEvent(LSG_EVENT_NAVIGATE_END);

	destination.x -= (this->arrow.size + padding);

	arrow = this->getArrow(destination, padding);

	if (this->canNavigate.next && SDL_PointInRect(&mousePosition, &arrow))
		this->sendEvent(LSG_EVENT_NAVIGATE_NEXT);
}

void LSG_Navigation::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_Navigation::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Navigation::render(SDL_Renderer* renderer) const
{
	this->renderFill(renderer);

	if (!this->arrow.back || !this->arrow.end || !this->arrow.home || !this->arrow.next || !this->texture)
		return;

	auto fillArea = this->getFillArea();
	auto padding  = LSG_Window::GetDPIScaled(LSG_Navigation::ArrowPadding);

	LSG_Navigation::renderArrows(renderer, fillArea, padding);
	LSG_Navigation::renderText(renderer,   fillArea, padding);
}

void LSG_Navigation::renderArrows(SDL_Renderer* renderer, const SDL_Rect& fillArea, int padding) const
{
	auto destination = this->getArrowDestination(fillArea, padding);

	SDL_RenderCopy(renderer, this->arrow.home, nullptr, &destination);

	destination.x += (this->arrow.size + padding);

	SDL_RenderCopy(renderer, this->arrow.back, nullptr, &destination);

	destination.x = (fillArea.x + fillArea.w - padding - this->arrow.size);

	SDL_RenderCopy(renderer, this->arrow.end, nullptr, &destination);

	destination.x -= (this->arrow.size + padding);

	SDL_RenderCopy(renderer, this->arrow.next, nullptr, &destination);
}

void LSG_Navigation::renderText(SDL_Renderer* renderer, const SDL_Rect& fillArea, int padding) const
{
	LSG_Alignment alignment = {
		LSG_HALIGN_CENTER,
		LSG_VALIGN_MIDDLE
	};

	SDL_Rect background = {
		(fillArea.x + padding + ((this->arrow.size + padding) * 2)),
		fillArea.y,
		(fillArea.w - ((this->arrow.size + padding) * 4)),
		fillArea.h
	};

	SDL_Rect clip = {
		0, 0,
		std::min(this->textSize.width,  background.w),
		std::min(this->textSize.height, background.h)
	};

	auto destination = LSG_Graphics::GetDestinationAligned(background, { clip.w, clip.h }, alignment);

	SDL_RenderCopy(renderer, this->texture, &clip, &destination);
}

void LSG_Navigation::RenderTooltip(SDL_Renderer* renderer) const
{
	if (!this->highlighted)
	{
		LSG_Graphics::DestroyTexture(std::format("{}_tooltip_background", this->id));
		LSG_Graphics::DestroyTexture(std::format("{}_tooltip_text",       this->id));

		LSG_Graphics::DestroyTexture(std::format("{}_back_tooltip_background", this->id));
		LSG_Graphics::DestroyTexture(std::format("{}_back_tooltip_text",       this->id));

		LSG_Graphics::DestroyTexture(std::format("{}_end_tooltip_background", this->id));
		LSG_Graphics::DestroyTexture(std::format("{}_end_tooltip_text",       this->id));

		LSG_Graphics::DestroyTexture(std::format("{}_home_tooltip_background", this->id));
		LSG_Graphics::DestroyTexture(std::format("{}_home_tooltip_text",       this->id));

		LSG_Graphics::DestroyTexture(std::format("{}_next_tooltip_background", this->id));
		LSG_Graphics::DestroyTexture(std::format("{}_next_tooltip_text",       this->id));

		return;
	}

	auto fillArea = this->getFillArea();
	auto padding  = LSG_Window::GetDPIScaled(LSG_Navigation::ArrowPadding);

	auto destination   = this->getArrowDestination(fillArea, padding);
	auto mousePosition = LSG_Window::GetMousePosition();

	std::string id      = "";
	std::string tooltip = "";

	if (SDL_PointInRect(&mousePosition, &destination)) {
		id      = std::format("{}_home", this->id);
		tooltip = "Home";
	}
	
	destination.x += (this->arrow.size + padding);

	if (SDL_PointInRect(&mousePosition, &destination)) {
		id      = std::format("{}_back", this->id);
		tooltip = "Back";
	}

	destination.x = (fillArea.x + fillArea.w - padding - this->arrow.size);

	if (SDL_PointInRect(&mousePosition, &destination)) {
		id      = std::format("{}_end", this->id);
		tooltip = "End";
	}

	destination.x -= (this->arrow.size + padding);

	if (SDL_PointInRect(&mousePosition, &destination)) {
		id      = std::format("{}_next", this->id);
		tooltip = "Next";
	}

	if (!id.empty() && !tooltip.empty())
		LSG_Graphics::RenderTooltip(renderer, tooltip, mousePosition, id);
	else if (!this->tooltip.empty())
		LSG_Graphics::RenderTooltip(renderer, this->tooltip, mousePosition, this->id);
}

void LSG_Navigation::set()
{
	auto colorPrev = (this->canNavigate.back ? this->textColor : LSG_ScrollBar::DefaultThumbColor);
	auto colorNext = (this->canNavigate.next ? this->textColor : LSG_ScrollBar::DefaultThumbColor);

	this->arrow.size = LSG_Window::GetDPIScaled(this->getFontSize());

	SDL_Size size = { this->arrow.size, this->arrow.size };

	if (!this->arrow.home)
		this->arrow.home = LSG_Graphics::GetVector(LSG_VECTOR_PAGE_START, colorPrev, size);

	if (!this->arrow.back)
		this->arrow.back = LSG_Graphics::GetVector(LSG_VECTOR_PAGE_BACK, colorPrev, size);

	if (!this->arrow.next)
		this->arrow.next = LSG_Graphics::GetVector(LSG_VECTOR_PAGE_NEXT, colorNext, size);

	if (!this->arrow.end)
		this->arrow.end = LSG_Graphics::GetVector(LSG_VECTOR_PAGE_END, colorNext, size);

	if (!this->texture)
	{
		this->texture  = this->getTexture(this->text);
		this->textSize = this->getTextureSize();
	}
}

void LSG_Navigation::Set(size_t itemsTotal, size_t itemsPerNavigation)
{
	this->position = 0;

	this->items.perNavigation = itemsPerNavigation;
	this->items.total         = itemsTotal;

	this->canNavigate = this->getCanNavigate();

	this->text = this->getText();

	this->destroyTextures();

	this->set();
}

void LSG_Navigation::Set()
{
	this->destroyTextures();

	this->set();
}
