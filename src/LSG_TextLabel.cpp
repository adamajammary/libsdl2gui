#include "LSG_TextLabel.h"

LSG_TextLabel::LSG_TextLabel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
}

SDL_Size LSG_TextLabel::GetSize()
{
	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->getTextureSize();

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	textureSize.width  += (padding2x + border2x);
	textureSize.height += (padding2x + border2x);

	if (attributes.contains("width") && (this->background.w > 0))
		textureSize.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		textureSize.height = this->background.h;

	return textureSize;
}

SDL_Size LSG_TextLabel::GetTextureSize()
{
	return this->getTextureSize();
}

void LSG_TextLabel::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->getTextureSize();

	SDL_Size size = {};

	if (attributes.contains("width") && (this->background.w > 0))
		size.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		size.height = this->background.h;

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = (size.width  > 0 ? size.width  : (textureSize.width  + padding2x + border2x));
	this->background.h = (size.height > 0 ? size.height : (textureSize.height + padding2x + border2x));

	this->render(renderer);
}

void LSG_TextLabel::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_TextLabel::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto alignment       = this->getAlignment();
	auto fillArea        = this->getFillArea(this->background, this->border);
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	auto textureSize     = this->getTextureSize();

	if (!this->wrap) {
		this->renderTexture(renderer, fillArea, alignment, this->texture, textureSize);
		return;
	}

	this->renderScrollableTexture(renderer, fillArea, this->border, alignment, this->texture, textureSize);

	if (fillArea.h < scrollBarSize2x)
		return;

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, fillArea, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, fillArea, textureSize.height, this->backgroundColor, this->highlighted);
}

void LSG_TextLabel::SetText(const std::string &text)
{
	if (text == this->text)
		return;

	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->destroyTextures();

	this->text = text;

	if (!text.empty())
		this->texture = this->getTexture(text);
}

void LSG_TextLabel::SetText()
{
	if (this->texture && !this->hasChanged())
		return;

	this->destroyTextures();

	if (!this->text.empty())
		this->texture = this->getTexture(this->text);
}
