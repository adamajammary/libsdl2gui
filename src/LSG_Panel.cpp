#include "LSG_Panel.h"

LSG_Panel::LSG_Panel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->renderTarget = nullptr;
	this->scrollable   = false;
}

LSG_Panel::~LSG_Panel()
{
	if (this->renderTarget)
		SDL_DestroyTexture(this->renderTarget);
}

SDL_Size LSG_Panel::GetSize()
{
	auto attributes  = this->GetXmlAttributes();
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");
	bool isVertical  = (orientation == "vertical");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);

	SDL_Size totalSize       = {};
	int      visibleChildren = 0;

	for (auto child : this->children)
	{
		if (!child->visible)
			continue;

		auto     childMargin2x = (child->margin + child->margin);
		SDL_Size childSize     = { child->background.w, child->background.h };

		if (child->IsButton())
			childSize = static_cast<LSG_Button*>(child)->GetSize();
		else if (child->IsImage())
			childSize = static_cast<LSG_Image*>(child)->GetSize();
		else if (child->IsList())
			childSize = static_cast<LSG_List*>(child)->GetSize();
		else if (child->IsPanel())
			childSize = static_cast<LSG_Panel*>(child)->GetSize();
		else if (child->IsTable())
			childSize = static_cast<LSG_Table*>(child)->GetSize();
		else if (child->IsTextLabel())
			childSize = static_cast<LSG_TextLabel*>(child)->GetSize();

		if (isVertical) {
			totalSize.height += (childSize.height + childMargin2x);
			totalSize.width   = std::max((childSize.width + childMargin2x), totalSize.width);
		} else {
			totalSize.width += (childSize.width + childMargin2x);
			totalSize.height = std::max((childSize.height + childMargin2x), totalSize.height);
		}

		visibleChildren++;
	}

	auto padding2x    = (this->padding + this->padding);
	auto totalSpacing = (spacing * (visibleChildren - 1));

	if (isVertical) {
		totalSize.height += (totalSpacing + padding2x);
		totalSize.width  += padding2x;
	} else {
		totalSize.width  += (totalSpacing + padding2x);
		totalSize.height += padding2x;
	}

	auto scrollBarSize = LSG_ScrollBar::GetSize();

	if (this->showScrollY)
		totalSize.width += scrollBarSize;

	if (this->showScrollX)
		totalSize.height += scrollBarSize;

	return totalSize;
}

bool LSG_Panel::IsScroll() const
{
	return this->scrollable;
}

void LSG_Panel::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto textureSize = this->GetSize();

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = textureSize.width;
	this->background.h = textureSize.height;

	LSG_Component::Render(renderer);

	SDL_Point offset = {
		(position.x + this->border + this->padding),
		(position.y + this->border + this->padding)
	};

	SDL_Size maxSize = { this->background.w, this->background.h };

	this->renderChildren(renderer, offset, maxSize);
}

void LSG_Panel::Render(SDL_Renderer* renderer)
{
	this->scrollable  = false;
	this->showScrollX = false;
	this->showScrollY = false;

	if (!this->visible)
		return;

	auto scrollable = LSG_XML::GetAttribute(this->xmlNode, "scrollable");

	if ((scrollable != "true") || this->children.empty()) {
		LSG_Component::Render(renderer);
		return;
	}

	auto fillArea = this->getFillArea(this->background, this->border);

	if (fillArea.h < LSG_ScrollBar::GetSize2x())
		return;

	auto textureSize = this->GetSize();

	if ((textureSize.width <= this->background.w) && (textureSize.height <= this->background.h)) {
		LSG_Component::Render(renderer);
		return;
	}

	auto scrollBarSize = LSG_ScrollBar::GetSize();

	this->scrollable  = true;
	this->showScrollX = ((textureSize.width  + scrollBarSize) > this->background.w);
	this->showScrollY = ((textureSize.height + scrollBarSize) > this->background.h);

	SDL_Size maxSize = {
		std::max(this->background.w, textureSize.width),
		std::max(this->background.h, textureSize.height)
	};

	this->renderContentToTexture(renderer, maxSize);

	this->renderContent(renderer, fillArea, maxSize);
	this->renderBorder(renderer,  this->border, this->borderColor, this->background);
	this->renderScroll(renderer,  fillArea, maxSize);
}

void LSG_Panel::renderChildren(SDL_Renderer* renderer, const SDL_Point& offset, const SDL_Size& maxSize)
{
	auto attributes  = this->GetXmlAttributes();
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);

	LSG_UmapStrSize sizes;

	for (auto child : this->children) {
		if (child->visible)
			sizes[child->GetID()] = { child->background.w, child->background.h };
	}

	bool isVertical  = (orientation == "vertical");
	auto contentSize = (isVertical ? maxSize.height : maxSize.width);

	SDL_Point offsetPosition = offset;

	for (auto child : this->children)
	{
		if (!sizes.contains(child->GetID()))
			continue;

		if (isVertical)
			offsetPosition.y += child->margin;
		else
			offsetPosition.x += child->margin;

		auto renderPosition = LSG_UI::GetAlignedPosition(offsetPosition, attributes, sizes, contentSize, maxSize, child, this);

		if (child->IsButton())
			static_cast<LSG_Button*>(child)->Render(renderer, renderPosition);
		else if (child->IsImage())
			static_cast<LSG_Image*>(child)->Render(renderer, renderPosition);
		else if (child->IsLine())
			static_cast<LSG_Line*>(child)->Render(renderer, renderPosition);
		else if (child->IsList())
			static_cast<LSG_List*>(child)->Render(renderer, renderPosition);
		else if (child->IsPanel())
			static_cast<LSG_Panel*>(child)->Render(renderer, renderPosition);
		else if (child->IsProgressBar())
			static_cast<LSG_ProgressBar*>(child)->Render(renderer, renderPosition);
		else if (child->IsSlider())
			static_cast<LSG_Slider*>(child)->Render(renderer, renderPosition);
		else if (child->IsTable())
			static_cast<LSG_Table*>(child)->Render(renderer, renderPosition);
		else if (child->IsTextLabel())
			static_cast<LSG_TextLabel*>(child)->Render(renderer, renderPosition);

		if (isVertical)
			offsetPosition.y += (child->background.h + child->margin + spacing);
		else
			offsetPosition.x += (child->background.w + child->margin + spacing);
	}
}

void LSG_Panel::renderContent(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize)
{
	auto scrollBarSize = LSG_ScrollBar::GetSize();

	SDL_Rect destination = {
		background.x,
		background.y,
		(background.w - (this->showScrollY ? scrollBarSize : 0)),
		(background.h - (this->showScrollX ? scrollBarSize : 0))
	};

	SDL_Rect clip           = { 0, 0, destination.w, destination.h };
	auto     clipWithOffset = this->getClipWithOffset(clip, maxSize);

	SDL_RenderCopy(renderer, this->renderTarget, &clipWithOffset, &destination);
}

void LSG_Panel::renderContentToTexture(SDL_Renderer* renderer, const SDL_Size& maxSize)
{
	LSG_Window::InitRenderTarget(&this->renderTarget, maxSize);

	SDL_SetRenderTarget(renderer, this->renderTarget);

	SDL_Rect background = { 0, 0, maxSize.width, maxSize.height };

	this->renderFill(renderer, 0, this->backgroundColor, background);

	SDL_Point offsetPosition = { this->padding, this->padding };

	this->renderChildren(renderer, offsetPosition, maxSize);

	SDL_SetRenderTarget(renderer, nullptr);
}

void LSG_Panel::renderScroll(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize)
{
	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, background, maxSize.width,  this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer,   background, maxSize.height, this->backgroundColor, this->highlighted);

	if (!this->showScrollX || !this->showScrollY)
		return;

	SDL_Rect bottomRight = {
		(this->scrollBarX.x + this->scrollBarX.w),
		(this->scrollBarY.y + this->scrollBarY.h),
		(background.w - this->scrollBarX.w),
		(background.h - this->scrollBarY.h)
	};

	this->renderFill(renderer, 0, this->backgroundColor, bottomRight);
}
