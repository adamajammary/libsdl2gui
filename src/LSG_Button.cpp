#include "LSG_Button.h"

LSG_Button::LSG_Button(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

bool LSG_Button::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled)
		return false;

	this->sendEvent(LSG_EVENT_BUTTON_CLICKED);

	return true;
}

void LSG_Button::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		LSG_Component::Render(renderer);

	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
	else if (this->highlighted)
		this->renderHighlight(renderer);
}

void LSG_Button::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int32_t)type;
	clickEvent.user.data1 = (void*)strdup(this->GetID().c_str());

	SDL_PushEvent(&clickEvent);
}
