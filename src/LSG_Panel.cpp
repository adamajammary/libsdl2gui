#include "LSG_Panel.h"

LSG_Panel::LSG_Panel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

SDL_Size LSG_Panel::GetTextureSize(const SDL_Rect& background)
{
	auto attributes  = this->GetXmlAttributes();
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);

	SDL_Size size = {};

	for (auto child : this->children) {
		if (orientation == "vertical")
			size.height += child->background.h;
		else
			size.width += child->background.w;
	}

	auto spacingAndPadding = (this->padding + (spacing * ((int)this->children.size() - 1)) + this->padding);

	if (orientation == "vertical")
		size.height += spacingAndPadding;
	else
		size.width += spacingAndPadding;

	SDL_Size textureSize = {
		std::max(background.w, size.width),
		std::max(background.h, size.height)
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

	if (fillArea.h < LSG_ScrollBar::GetSize2x()) {
		LSG_Component::Render(renderer);
		return;
	}

	auto textureSize = this->GetTextureSize(fillArea);

	if ((textureSize.width <= fillArea.w) && (textureSize.height <= fillArea.h)) {
		LSG_Component::Render(renderer);
		return;
	}

	this->showScrollX = true;
	this->showScrollY = true;

	SDL_Point offset = {
		this->children[0]->background.x,
		this->children[0]->background.y
	};

	textureSize.width  += offset.x;
	textureSize.height += offset.y;

	SDL_Size maxSize = {
		(textureSize.width  - offset.x),
		(textureSize.height - offset.y)
	};

	auto texture = this->renderContentToTexture(renderer, offset, textureSize);

	this->renderContent(renderer, texture, fillArea, offset, maxSize);
	this->renderBorder(renderer, this->border, this->borderColor, this->background);
	this->renderScroll(renderer, fillArea, maxSize);

	SDL_DestroyTexture(texture);
}

void LSG_Panel::renderContent(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& background, const SDL_Point& offset, const SDL_Size& maxSize)
{
	auto scrollBarSize = LSG_ScrollBar::GetSize();

	SDL_Rect destination = {
		(background.x + this->padding),
		(background.y + this->padding),
		(background.w - scrollBarSize),
		(background.h - scrollBarSize)
	};

	SDL_Rect clip = {
		offset.x,
		offset.y,
		destination.w,
		destination.h
	};

	auto clipWithOffset = this->getClipWithOffset(clip, maxSize);

	SDL_RenderCopy(renderer, texture, &clipWithOffset, &destination);
}

SDL_Texture* LSG_Panel::renderContentToTexture(SDL_Renderer* renderer, const SDL_Point &offset, const SDL_Size &textureSize)
{
	auto format  = SDL_GetWindowPixelFormat(SDL_RenderGetWindow(renderer));
	auto texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, textureSize.width, textureSize.height);

	SDL_SetRenderTarget(renderer, texture);

	SDL_Rect background = {
		offset.x,
		offset.y,
		textureSize.width,
		textureSize.height
	};

	this->renderFill(renderer, 0, this->backgroundColor, background);

	for (auto child : this->children)
		child->Render(renderer);

	SDL_SetRenderTarget(renderer, NULL);

	return texture;
}

void LSG_Panel::renderScroll(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize)
{
	this->renderScrollBarHorizontal(renderer, background, maxSize.width,  this->backgroundColor, this->highlighted);
	this->renderScrollBarVertical(renderer,   background, maxSize.height, this->backgroundColor, this->highlighted);

	auto scrollBarSize = LSG_ScrollBar::GetSize();

	SDL_Rect bottomRight = {
		(this->scrollBarX.x + this->scrollBarX.w),
		(this->scrollBarY.y + this->scrollBarY.h),
		scrollBarSize,
		scrollBarSize
	};

	this->renderFill(renderer, 0, this->backgroundColor, bottomRight);
}