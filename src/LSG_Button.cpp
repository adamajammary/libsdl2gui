#include "LSG_Button.h"

LSG_Button::LSG_Button(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->selected = false;
}

bool LSG_Button::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled)
		return false;

	this->sendEvent(LSG_EVENT_BUTTON_CLICKED);

	return true;
}

void LSG_Button::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

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
