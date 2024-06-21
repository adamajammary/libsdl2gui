#include "LSG_Panel.h"

LSG_Panel::LSG_Panel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->renderTarget = nullptr;
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

	auto border2x     = (this->border  + this->border);
	auto padding2x    = (this->padding + this->padding);
	auto totalSpacing = (spacing * (visibleChildren - 1));

	if (isVertical) {
		totalSize.height += (totalSpacing + padding2x + border2x);
		totalSize.width  += (padding2x + border2x);
	} else {
		totalSize.width  += (totalSpacing + padding2x + border2x);
		totalSize.height += (padding2x + border2x);
	}

	SDL_Size textureSize = {
		std::max(this->background.w, totalSize.width),
		std::max(this->background.h, totalSize.height)
	};

	return textureSize;
}

void LSG_Panel::Render(SDL_Renderer* renderer)
{
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

	if ((textureSize.width <= fillArea.w) && (textureSize.height <= fillArea.h)) {
		LSG_Component::Render(renderer);
		return;
	}

	this->showScrollX = true;
	this->showScrollY = true;

	this->renderContentToTexture(renderer, textureSize);

	this->renderContent(renderer, fillArea, textureSize);
	this->renderBorder(renderer,  this->border, this->borderColor, this->background);
	this->renderScroll(renderer,  fillArea, textureSize);
}

void LSG_Panel::renderContent(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& textureSize)
{
	auto scrollBarSize = LSG_ScrollBar::GetSize();

	SDL_Rect destination = {
		(background.x + this->padding),
		(background.y + this->padding),
		(background.w - scrollBarSize),
		(background.h - scrollBarSize)
	};

	SDL_Rect clip           = { 0, 0, destination.w, destination.h };
	auto     clipWithOffset = this->getClipWithOffset(clip, textureSize);

	SDL_RenderCopy(renderer, this->renderTarget, &clipWithOffset, &destination);
}

void LSG_Panel::renderContentToTexture(SDL_Renderer* renderer, const SDL_Size &textureSize)
{
	auto attributes  = this->GetXmlAttributes();
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);

	LSG_UmapStrSize sizes;

	for (auto child : this->children)
	{
		if (!child->visible)
			continue;

		SDL_Size size = { child->background.w, child->background.h };

		if (child->IsButton())
			static_cast<LSG_Button*>(child)->GetSize();
		else if (child->IsImage())
			static_cast<LSG_Image*>(child)->GetSize();
		else if (child->IsList())
			static_cast<LSG_List*>(child)->GetSize();
		else if (child->IsTable())
			static_cast<LSG_Table*>(child)->GetSize();
		else if (child->IsTextLabel())
			static_cast<LSG_TextLabel*>(child)->GetSize();

		sizes[child->GetID()] = size;
	}

	LSG_Window::InitRenderTarget(&this->renderTarget, textureSize);

	SDL_SetRenderTarget(renderer, this->renderTarget);

	SDL_Rect background  = { 0, 0, textureSize.width, textureSize.height };
	SDL_Size totalSize   = { background.w, background.h };

	this->renderFill(renderer, 0, this->backgroundColor, background);

	bool isVertical  = (orientation == "vertical");
	auto contentSize = (isVertical ? background.h : background.w);

	SDL_Point offsetPosition = {};

	for (auto child : this->children)
	{
		if (!sizes.contains(child->GetID()))
			continue;

		if (isVertical)
			offsetPosition.y += child->margin;
		else
			offsetPosition.x += child->margin;

		auto renderPosition = LSG_UI::GetAlignedPosition(offsetPosition, attributes, sizes, contentSize, totalSize, child, this);

		if (child->IsButton())
			static_cast<LSG_Button*>(child)->Render(renderer, renderPosition);
		else if (child->IsImage())
			static_cast<LSG_Image*>(child)->Render(renderer, renderPosition);
		else if (child->IsLine())
			static_cast<LSG_Line*>(child)->Render(renderer, renderPosition);
		else if (child->IsList())
			static_cast<LSG_List*>(child)->Render(renderer, renderPosition);
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

	SDL_SetRenderTarget(renderer, nullptr);
}

void LSG_Panel::renderScroll(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& textureSize)
{
	this->renderScrollBarHorizontal(renderer, background, textureSize.width,  this->backgroundColor, this->highlighted);
	this->renderScrollBarVertical(renderer,   background, textureSize.height, this->backgroundColor, this->highlighted);

	SDL_Rect bottomRight = {
		(this->scrollBarX.x + this->scrollBarX.w),
		(this->scrollBarY.y + this->scrollBarY.h),
		(background.w - this->scrollBarX.w),
		(background.h - this->scrollBarY.h)
	};

	this->renderFill(renderer, 0, this->backgroundColor, bottomRight);
}