#include "LSG_Button.h"

LSG_Button::LSG_Button(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->selected = false;
}

SDL_Size LSG_Button::GetSize()
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

		if (child->IsImage())
			childSize = static_cast<LSG_Image*>(child)->GetSize();
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
		(!attributes.contains("width")  ? totalSize.width  : this->background.w),
		(!attributes.contains("height") ? totalSize.height : this->background.h)
	};

	return textureSize;
}

bool LSG_Button::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled)
		return false;

	this->sendEvent(LSG_EVENT_BUTTON_CLICKED);

	return true;
}

void LSG_Button::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->GetSize();

	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = textureSize.width;
	this->background.h = textureSize.height;

	LSG_Component::Render(renderer);

	LSG_UmapStrSize sizes;

	int  contentSize = 0;
	bool isVertical  = (orientation == "vertical");

	for (auto child : this->children)
	{
		if (!child->visible)
			continue;

		SDL_Size size = {};

		if (child->IsImage())
			size = static_cast<LSG_Image*>(child)->GetTextureSize();
		else if (child->IsTextLabel())
			size = static_cast<LSG_TextLabel*>(child)->GetTextureSize();

		sizes[child->GetID()] = size;

		if (isVertical)
			contentSize += size.height;
		else
			contentSize += size.width;
	}

	SDL_Point offsetPosition = position;
	SDL_Size  totalSize      = { this->background.w, this->background.h };

	offsetPosition.x += (this->border + this->padding);
	offsetPosition.y += (this->border + this->padding);

	for (auto child : this->children)
	{
		if (!sizes.contains(child->GetID()))
			continue;

		if (isVertical)
			offsetPosition.y += child->margin;
		else
			offsetPosition.x += child->margin;

		auto renderPosition = LSG_UI::GetAlignedPosition(offsetPosition, attributes, sizes, contentSize, totalSize, child, this);

		if (child->IsImage())
			static_cast<LSG_Image*>(child)->Render(renderer, renderPosition);
		else if (child->IsTextLabel())
			static_cast<LSG_TextLabel*>(child)->Render(renderer, renderPosition);

		if (isVertical)
			offsetPosition.y += (child->background.h + child->margin + spacing);
		else
			offsetPosition.x += (child->background.w + child->margin + spacing);
	}

	this->render(renderer);
}

void LSG_Button::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	this->render(renderer);
}

void LSG_Button::render(SDL_Renderer* renderer)
{
	if (this->selected)
		this->renderHighlight(renderer);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer);
}

void LSG_Button::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int)type;
	clickEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&clickEvent);
}

void LSG_Button::SetSelected(bool selected)
{
	if (!this->parent || (this->selected == selected))
		return;

	auto parentChildren = this->parent->GetChildren();

	for (auto child : parentChildren) {
		if (child->IsButton())
			static_cast<LSG_Button*>(child)->selected = false;
	}

	this->selected = selected;
}
