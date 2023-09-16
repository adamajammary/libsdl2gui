#include "LSG_MenuItem.h"

LSG_MenuItem::LSG_MenuItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Menu(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->selected = false;
}

bool LSG_MenuItem::IsSelected()
{
	return this->selected;
}

bool LSG_MenuItem::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || !this->visible || !this->parent)
		return false;

	SDL_Point mousePosition = { event.x, event.y };

	if (!SDL_PointInRect(&mousePosition, &this->background))
		return true;

	this->sendEvent(LSG_EVENT_MENU_ITEM_SELECTED);

	LSG_UI::CloseSubMenu();

	return true;
}

void LSG_MenuItem::Render(SDL_Renderer* renderer)
{
}

void LSG_MenuItem::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event menuEvent = {};

	menuEvent.type       = SDL_RegisterEvents(1);
	menuEvent.user.code  = (int)type;
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

void LSG_MenuItem::SetValue(const std::string& value)
{
	LSG_XML::SetValue(this->xmlNode, value);
}
