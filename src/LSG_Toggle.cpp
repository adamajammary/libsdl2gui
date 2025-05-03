#include "LSG_Toggle.h"

LSG_Toggle::LSG_Toggle(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->on = (LSG_XML::GetAttribute(this->xmlNode, "on") == "true");
}

SDL_Rect LSG_Toggle::getDestination() const
{
	auto fillArea    = this->getArea(this->background);
	auto size        = this->getMaxSize(fillArea);
	auto destination = LSG_Graphics::GetDestinationAligned(fillArea, size, this->getAlignment());

	return destination;
}

SDL_Size LSG_Toggle::getMaxSize(const SDL_Rect& background) const
{
	auto     size    = std::min(background.w, background.h);
	SDL_Size maxSize = { size, size };

	return maxSize;
}

bool LSG_Toggle::IsMouseOver(const SDL_Point& mousePosition) const
{
	auto destination = this->getDestination();

	return SDL_PointInRect(&mousePosition, &destination);
}

bool LSG_Toggle::IsOn() const
{
	return this->on;
}

bool LSG_Toggle::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	if (this->IsMouseOver(mousePosition))
		this->toggle();

	return true;
}

void LSG_Toggle::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_Toggle::Render(SDL_Renderer* renderer) const
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Toggle::render(SDL_Renderer* renderer) const
{
	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto destination = this->getDestination();

	SDL_RenderCopy(renderer, this->texture, nullptr, &destination);
}

void LSG_Toggle::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event listEvent = {};

	listEvent.type       = SDL_RegisterEvents(1);
	listEvent.user.code  = (int)type;
	listEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&listEvent);
}

void LSG_Toggle::Set(bool on)
{
	this->on = on;

	this->Set();
}

void LSG_Toggle::Set()
{
	this->destroyTextures();

	auto fillArea = this->getArea(this->background);
	auto icon     = (this->on ? LSG_VECTOR_ICON_TOGGLE_OFF : LSG_VECTOR_ICON_TOGGLE_ON);

	this->texture = LSG_Graphics::GetVector(icon, this->textColor, this->getMaxSize(fillArea));
}

void LSG_Toggle::toggle()
{
	this->on = !this->on;

	this->Set();

	this->sendEvent(this->on ? LSG_EVENT_TOGGLED_ON : LSG_EVENT_TOGGLED_OFF);
}
