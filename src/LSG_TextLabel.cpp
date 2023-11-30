#include "LSG_TextLabel.h"

LSG_TextLabel::LSG_TextLabel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_TextLabel::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto background      = this->getFillArea(this->background, this->border);
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	auto textureSize     = this->getTextureSize();

	if (!this->wrap) {
		this->renderTexture(renderer, background, this->getAlignment(), this->texture, textureSize);
		return;
	}

	if (background.h < scrollBarSize2x)
		return;

	this->renderScrollableTexture(renderer, background, this->getAlignment(), this->texture, textureSize);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, background, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, background, textureSize.height, this->backgroundColor, this->highlighted);
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
