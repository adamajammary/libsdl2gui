#include "LSG_Menu.h"

LSG_Menu::LSG_Menu(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->enableScroll = false;
	this->orientation  = "";
	this->selectedMenu = -1;
	this->wrap         = false;
}

void LSG_Menu::Close()
{
	this->selectedMenu = -1;
}

int LSG_Menu::getSelectedMenu(const SDL_Point& mousePosition, const std::vector<SDL_Rect>& menuAreas)
{
	for (int i = 0; i < (int)menuAreas.size(); i++) {
		if (SDL_PointInRect(&mousePosition, &menuAreas[i]))
			return i;
	}

	return -1;
}

std::vector<SDL_Rect> LSG_Menu::getTextureDestinations()
{
	std::vector<SDL_Rect> destinations;

	auto menuBackground = this->getFillArea(this->background, this->border);
	auto textureSizes   = this->getTextureSizes(LSG_MENU_SPACING);

	textureSizes.totalSize.width += LSG_MENU_SPACING_2X;

	auto destination = this->getRenderDestinationAligned(menuBackground, textureSizes.totalSize);

	destination.x += LSG_MENU_SPACING;

	for (size_t i = 0; i < this->textures.size(); i++)
	{
		destination.w = textureSizes.sizes[i].width;

		destinations.push_back(SDL_Rect(destination));

		destination.x += (destination.w + LSG_MENU_SPACING);
	}

	return destinations;
}

bool LSG_Menu::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->visible)
		return false;

	auto mousePosition = SDL_Point(event.x, event.y);
	auto destinations  = this->getTextureDestinations();
	auto index         = this->getSelectedMenu(mousePosition, destinations);

	return this->open(mousePosition, destinations, index);
}

bool LSG_Menu::open(const SDL_Point& mousePosition, const std::vector<SDL_Rect>& menuAreas, int index)
{
	auto currentSelectedMenu = this->selectedMenu;

	LSG_UI::CloseSubMenu();

	if ((index < 0) || (index >= (int)menuAreas.size()) || (index == currentSelectedMenu))
		return false;

	this->selectedMenu = index;

	SDL_Rect menuArea = SDL_Rect(menuAreas[this->selectedMenu]);

	if (SDL_RectEmpty(&menuArea))
		return false;

	auto child = this->children[this->selectedMenu];

	if (!child || !child->enabled || !child->IsSubMenu())
		return false;

	auto subMenu = static_cast<LSG_MenuSub*>(child);

	subMenu->SetItems();

	auto subMenuItems = subMenu->GetItems();

	if (subMenuItems.empty())
		return false;

	auto textureSize = subMenu->GetTextureSize();

	menuArea.y += (menuArea.h + LSG_MENU_SPACING_HALF);
	menuArea.w  = (textureSize.width  + LSG_MENU_SUB_PADDING_X_2X);
	menuArea.h  = (textureSize.height + LSG_MENU_SUB_PADDING_Y_2X);

	auto rowHeight = (textureSize.height / (int)subMenuItems.size());
	int  offsetY   = LSG_MENU_SUB_PADDING_Y;

	auto subMenuChildren = subMenu->GetChildren();

	for (auto child : subMenuChildren)
	{
		child->background    = SDL_Rect(menuArea);
		child->background.y += offsetY;
		child->background.h  = rowHeight;

		offsetY += rowHeight;
	}

	subMenu->background = menuArea;

	LSG_UI::SetSubMenuVisible(subMenu);

	return true;
}

void LSG_Menu::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (this->textures.empty())
		return;

	auto destinations = this->getTextureDestinations();

	for (size_t i = 0; i < this->textures.size(); i++)
		SDL_RenderCopy(renderer, this->textures[i], nullptr, &destinations[i]);

	if (!this->children.empty() && SDL_RectEmpty(&this->children[0]->background))
	{
		for (int i = 0; i < (int)this->children.size(); i++) {
			this->children[i]->background    = SDL_Rect(destinations[i]);
			this->children[i]->background.w += LSG_MENU_SPACING;
		}
	}

	SDL_Point mousePosition = {};
	SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	auto index = this->getSelectedMenu(mousePosition, destinations);

	if (this->selectedMenu >= 0)
		this->renderHighlightSelection(renderer, destinations, this->selectedMenu);

	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
	else if (this->highlighted)
		this->renderHighlightSelection(renderer, destinations, index);

	if ((this->selectedMenu >= 0) && (index >= 0) && (this->selectedMenu != index))
		this->open(mousePosition, destinations, index);
}

void LSG_Menu::renderHighlightSelection(SDL_Renderer* renderer, const std::vector<SDL_Rect>& menuAreas, int index)
{
	if ((index < 0) || (index >= (int)this->children.size()) || !this->children[index]->enabled)
		return;

	SDL_Rect destination = SDL_Rect(menuAreas[index]);

	destination.y  = this->background.y;
	destination.h  = this->background.h;
	destination.x -= LSG_MENU_SPACING_HALF;
	destination.w += LSG_MENU_SPACING;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &destination);
}

void LSG_Menu::sendEvent(LSG_EventType type)
{
}

void LSG_Menu::SetItems()
{
	this->Close();

	this->destroyTextures();

	for (auto child : this->children)
	{
		if (child->IsSubMenu())
			static_cast<LSG_MenuSub*>(child)->Close();

		auto label = child->GetXmlAttribute("label");

		SDL_Texture* texture = (!label.empty() ? this->getTexture(label) : nullptr);

		if (texture)
			this->textures.push_back(texture);
	}
}
