#include "LSG_Events.h"

uint32_t       LSG_Events::lastClickTime = 0;
LSG_Component* LSG_Events::lastComponent = nullptr;
SDL_Event      LSG_Events::lastEvent     = {};
bool           LSG_Events::isMouseDown   = false;

void LSG_Events::handleKeyDownEvent(const SDL_KeyboardEvent& event)
{
	SDL_Point mousePosition = {};
	SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	auto component = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (component->IsTextLabel())
	{
		auto textLabel = static_cast<LSG_TextLabel*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     textLabel->ScrollHorizontal(-LSG_SCROLL_UNIT); break;
			case SDLK_RIGHT:    textLabel->ScrollHorizontal(LSG_SCROLL_UNIT); break;
			case SDLK_HOME:     textLabel->ScrollHome(); break;
			case SDLK_END:      textLabel->ScrollEnd(); break;
			case SDLK_UP:       textLabel->ScrollVertical(-LSG_SCROLL_UNIT); break;
			case SDLK_DOWN:     textLabel->ScrollVertical(LSG_SCROLL_UNIT); break;
			case SDLK_PAGEUP:   textLabel->ScrollVertical(-LSG_SCROLL_UNIT_PAGE); break;
			case SDLK_PAGEDOWN: textLabel->ScrollVertical(LSG_SCROLL_UNIT_PAGE); break;
			default: break;
		}

		return;
	}

	if (component->IsList() || component->IsTable())
	{
		auto list = static_cast<LSG_List*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     list->ScrollHorizontal(-LSG_SCROLL_UNIT); break;
			case SDLK_RIGHT:    list->ScrollHorizontal(LSG_SCROLL_UNIT); break;
			case SDLK_HOME:     list->SelectFirstRow(); break;
			case SDLK_END:      list->SelectLastRow(); break;
			case SDLK_UP:       list->SelectRow(-1); break;
			case SDLK_DOWN:     list->SelectRow(1); break;
			case SDLK_PAGEUP:   list->SelectRow(-LSG_LIST_UNIT_PAGE); break;
			case SDLK_PAGEDOWN: list->SelectRow(LSG_LIST_UNIT_PAGE); break;
			case SDLK_RETURN: case SDLK_KP_ENTER: list->Activate(); break;
			default: break;
		}

		return;
	}
}

void LSG_Events::handleMouseDownEvent(const SDL_Event& event)
{
	if (LSG_Events::isMouseDown || (event.type != SDL_MOUSEBUTTONDOWN))
		return;

	SDL_Point mousePosition = { event.button.x, event.button.y };
	auto      component     = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (event.button.button == SDL_BUTTON_RIGHT) {
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_RIGHT_CLICKED, component->GetID());
		return;
	}

	bool enableMouseDown = false;

	if (component->IsSlider())
		enableMouseDown = static_cast<LSG_Slider*>(component)->MouseClick(event.button);
	else if (component->IsScrollable())
		enableMouseDown = static_cast<LSG_Text*>(component)->ScrollMouseClick(mousePosition);

	if (enableMouseDown)
	{
		LSG_Events::isMouseDown   = true;
		LSG_Events::lastClickTime = SDL_GetTicks();
		LSG_Events::lastComponent = component;
		LSG_Events::lastEvent     = event;

		return;
	}

	bool isClicked = false;

	if (component->IsMenu())
		isClicked = static_cast<LSG_Menu*>(component)->MouseClick(event.button);
	else if (component->IsSubMenu())
		isClicked = static_cast<LSG_MenuSub*>(component)->MouseClick(event.button);
	else if (component->IsMenuItem())
		isClicked = static_cast<LSG_MenuItem*>(component)->MouseClick(event.button);

	if (isClicked) {
		LSG_Events::lastClickTime = SDL_GetTicks();
		return;
	}

	LSG_UI::CloseSubMenu();

	if (component->IsList())
		isClicked = static_cast<LSG_List*>(component)->MouseClick(event.button);
	else if (component->IsTable())
		isClicked = static_cast<LSG_Table*>(component)->MouseClick(event.button);

	if (isClicked) {
		LSG_Events::lastClickTime = SDL_GetTicks();
		return;
	}

	auto button = LSG_UI::GetButton(mousePosition);

	if (button)
		isClicked = button->MouseClick(event.button);

	if (isClicked) {
		LSG_Events::lastClickTime = SDL_GetTicks();
		return;
	}

	if (LSG_Events::IsDoubleClick(event.button)) {
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_DOUBLE_CLICKED, component->GetID());
	} else {
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_CLICKED, component->GetID());
		LSG_Events::lastClickTime = SDL_GetTicks();
	}
}

void LSG_Events::handleMouseLastDownEvent()
{
	if (!LSG_Events::isMouseDown || !LSG_Events::lastComponent)
		return;

	SDL_Point lastPosition  = { LSG_Events::lastEvent.button.x, LSG_Events::lastEvent.button.y };
	auto      lastComponent = LSG_Events::lastComponent;

	if (lastComponent->IsScrollable())
		static_cast<LSG_Text*>(lastComponent)->ScrollMouseDown(lastPosition);
}

void LSG_Events::handleMouseMoveEvent(const SDL_MouseMotionEvent& event)
{
	SDL_Point mousePosition = { event.x, event.y };

	if (!LSG_Events::isMouseDown) {
		LSG_UI::HighlightComponents(mousePosition);
		return;
	}

	if (!LSG_Events::lastComponent)
		return;

	SDL_Point lastPosition  = { LSG_Events::lastEvent.button.x, LSG_Events::lastEvent.button.y };
	auto      lastComponent = LSG_Events::lastComponent;

	if (lastComponent->IsSlider())
		static_cast<LSG_Slider*>(lastComponent)->MouseMove(mousePosition);
	else if (lastComponent->IsScrollable())
		static_cast<LSG_Text*>(lastComponent)->ScrollMouseMove(mousePosition, lastPosition);
}

void LSG_Events::handleMouseScrollEvent(const SDL_MouseWheelEvent& event)
{
	int scroll = -(event.y * LSG_SCROLL_UNIT_WHEEL);

	if (event.direction == SDL_MOUSEWHEEL_FLIPPED)
		scroll *= -1;

	auto mousePosition = SDL_Point(event.mouseX, event.mouseY);
	auto component     = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (component->IsSlider())
		static_cast<LSG_Slider*>(component)->MouseScroll(scroll);
	else if (component->IsScrollable())
		static_cast<LSG_Text*>(component)->ScrollVertical(scroll);
}

void LSG_Events::handleMouseUp()
{
	if (LSG_Events::lastComponent)
	{
		if (LSG_Events::lastComponent->IsSlider())
			static_cast<LSG_Slider*>(LSG_Events::lastComponent)->MouseUp();
		else if (LSG_Events::lastComponent->IsScrollable())
			static_cast<LSG_Text*>(LSG_Events::lastComponent)->ScrollMouseUp();
	}

	LSG_Events::isMouseDown   = false;
	LSG_Events::lastComponent = nullptr;
	LSG_Events::lastEvent     = {};
}

void LSG_Events::handleWindowEvent(const SDL_WindowEvent& event)
{
	switch (event.event) {
	case SDL_WINDOWEVENT_CLOSE:
        LSG_Quit();
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		LSG_UI::Layout();
		break;
	default:
		break;
	}
}

std::vector<SDL_Event> LSG_Events::Handle()
{
	LSG_Events::handleMouseLastDownEvent();

	SDL_Event              event  = {};
	std::vector<SDL_Event> events = {};

	SDL_PumpEvents();

	while (SDL_PollEvent(&event))
	{
		switch (event.type) {
			case SDL_KEYDOWN:         LSG_Events::handleKeyDownEvent(event.key); break;
			case SDL_MOUSEBUTTONDOWN: LSG_Events::handleMouseDownEvent(event); break;
			case SDL_MOUSEBUTTONUP:   LSG_Events::handleMouseUp(); break;
			case SDL_MOUSEMOTION:     LSG_Events::handleMouseMoveEvent(event.motion); break;
			case SDL_MOUSEWHEEL:      LSG_Events::handleMouseScrollEvent(event.wheel); break;
			case SDL_WINDOWEVENT:     LSG_Events::handleWindowEvent(event.window); break;
			default: break;
		}

		events.push_back(SDL_Event(event));
	}

	int x, y;
	LSG_Events::isMouseDown = (SDL_GetMouseState(&x, &y) == SDL_BUTTON_LEFT);

	return events;
}

bool LSG_Events::IsDoubleClick(const SDL_MouseButtonEvent& event)
{
	if (event.clicks < 2)
		return false;

	auto timeSinceLastClick = (SDL_GetTicks() - LSG_Events::lastClickTime);
	bool isDoubleClick      = ((timeSinceLastClick > 0) && (timeSinceLastClick < LSG_DOUBLE_CLICK_TIME_LIMIT));

	return isDoubleClick;
}

bool LSG_Events::IsMouseDown()
{
	return LSG_Events::isMouseDown;
}

void LSG_Events::sendEvent(LSG_EventType type, const std::string& id)
{
	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int)type;
	clickEvent.user.data1 = (void*)strdup(id.c_str());

	SDL_PushEvent(&clickEvent);
}
