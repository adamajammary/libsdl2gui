#include "LSG_TextLabel.h"

LSG_TextLabel::LSG_TextLabel(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_TextLabel::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto backgroundArea = this->getFillArea(this->background, this->border);
	auto size           = this->GetTextureSize();

	this->renderTexture(renderer, backgroundArea);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, backgroundArea, size.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, backgroundArea, size.height, this->backgroundColor, this->highlighted);
}
