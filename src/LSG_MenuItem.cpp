#include "LSG_MenuItem.h"

LSG_MenuItem::LSG_MenuItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->selected = false;
}

bool LSG_MenuItem::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || !this->visible || !this->parent)
		return false;

	auto mousePosition = SDL_Point(event.x, event.y);

	if (!SDL_PointInRect(&mousePosition, &this->background))
		return true;

	this->sendEvent(LSG_EVENT_MENU_ITEM_SELECTED);

	LSG_UI::CloseSubMenu();

	return true;
}

void LSG_MenuItem::Render(SDL_Renderer* renderer)
{
	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
	else if (this->highlighted )
		this->renderHighlight(renderer);

	if (!this->visible)
		return;

	if (this->selected)
		this->renderHighlight(renderer);
}

void LSG_MenuItem::renderDisabledOverlay(SDL_Renderer* renderer)
{
	auto backgroundArea = SDL_Rect(this->background);

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuItem::renderHighlight(SDL_Renderer* renderer)
{
	auto backgroundArea = SDL_Rect(this->background);

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuItem::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event menuEvent = {};

	menuEvent.type       = SDL_RegisterEvents(1);
	menuEvent.user.code  = (int32_t)type;
	menuEvent.user.data1 = (void*)strdup(this->GetID().c_str());

	SDL_PushEvent(&menuEvent);
}

void LSG_MenuItem::SetSelected(bool selected)
{
	if (!this->parent || (this->selected == selected))
		return;

	auto items = this->parent->GetChildren();

	for (auto item : items) {
		if (item->IsMenuItem())
			static_cast<LSG_MenuItem*>(item)->selected = false;
	}

	this->selected = selected;
}
