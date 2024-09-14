#include "LSG_MenuSub.h"

LSG_MenuSub::LSG_MenuSub(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_MenuItem(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_MenuSub::AddItem(const std::string& item, const std::string& itemId)
{
	if (item.empty() || itemId.empty())
		return;

	auto itemNode = LSG_XML::AddChildNode(this->xmlNode, "menu-item");

	if (!itemNode)
		return;

	LSG_XML::SetAttribute(itemNode, "id", itemId);

	auto itemComponent = LSG_UI::AddXmlNode(itemNode, this);

	itemComponent->text = item;

	itemComponent->SetColors();
}

int LSG_MenuSub::getMaxHeightArrow()
{
	auto padding = LSG_Graphics::GetDPIScaled(LSG_MenuSub::PaddingArrow2x);

	return (this->background.h - padding);
}

void LSG_MenuSub::Render(SDL_Renderer* renderer)
{
	if (!this->visible || (this->textures.size() < NR_OF_SUB_MENU_TEXTURES))
		return;

	if (!this->enabled)
		this->renderDisabled(renderer);

	this->renderText(renderer, this->textures[LSG_SUB_MENU_TEXTURE_TEXT]);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer, this->background);

	this->renderArrow(renderer);
}

void LSG_MenuSub::renderArrow(SDL_Renderer* renderer)
{
	auto texture = this->textures[LSG_SUB_MENU_TEXTURE_ARROW];

	if (!texture)
		return;

	auto size = LSG_Graphics::GetTextureSize(texture);

	SDL_Rect destination = {
		(this->background.x + this->background.w - size.width),
		(this->background.y + ((this->background.h - size.height) / 2)),
		size.width,
		size.height
	};

	SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

void LSG_MenuSub::SetSubMenu(const SDL_Rect& background)
{
	this->background = background;

	this->destroyTextures();

	this->textures.resize(NR_OF_SUB_MENU_TEXTURES);

	auto xmlText = LSG_XML::GetAttribute(this->xmlNode, "title");

	if (!xmlText.empty())
		this->textures[LSG_SUB_MENU_TEXTURE_TEXT] = this->getTexture(xmlText);

	auto     maxSize = this->getMaxHeightArrow();
	SDL_Size size    = { maxSize, maxSize };

	this->textures[LSG_SUB_MENU_TEXTURE_ARROW] = LSG_Graphics::GetVectorNext(this->textColor, size);
}
