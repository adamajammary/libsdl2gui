#include "LSG_Events.h"

uint32_t       LSG_Events::lastClickTime  = 0;
uint32_t       LSG_Events::lastClickTime2 = 0;
LSG_Component* LSG_Events::lastComponent  = nullptr;
SDL_Event      LSG_Events::lastEvent      = {};
bool           LSG_Events::isMouseDown    = false;

SDL_Point LSG_Events::getMousePosition(const SDL_Event& event)
{
	SDL_Point position = {};

	if ((event.type == SDL_FINGERDOWN) || (event.type == SDL_FINGERUP) || (event.type == SDL_FINGERMOTION))
	{
		auto size = LSG_Window::GetSize();
		position  = { (int)(event.tfinger.x * (float)size.width), (int)(event.tfinger.y * (float)size.height) };

		return position;
	}

	#if defined _macosx
		if ((event.type == SDL_MOUSEBUTTONDOWN) || (event.type == SDL_MOUSEBUTTONUP))
			position = { LSG_Graphics::GetDPIScaled(event.button.x), LSG_Graphics::GetDPIScaled(event.button.y) };
		else if (event.type == SDL_MOUSEMOTION)
			position = { LSG_Graphics::GetDPIScaled(event.motion.x), LSG_Graphics::GetDPIScaled(event.motion.y) };
	#else
		if ((event.type == SDL_MOUSEBUTTONDOWN) || (event.type == SDL_MOUSEBUTTONUP))
			position = { event.button.x, event.button.y };
		else if (event.type == SDL_MOUSEMOTION)
			position = { event.motion.x, event.motion.y };
	#endif

	return position;
}

void LSG_Events::handleKeyDownEvent(const SDL_KeyboardEvent& event)
{
	auto component = LSG_UI::GetComponent(LSG_Window::GetMousePosition());

	if (!component || !component->enabled)
		return;

	if (component->IsTextLabel())
	{
		auto textLabel = static_cast<LSG_TextLabel*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     textLabel->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    textLabel->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     textLabel->OnScrollHome(); break;
			case SDLK_END:      textLabel->OnScrollEnd(); break;
			case SDLK_UP:       textLabel->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     textLabel->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   textLabel->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: textLabel->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}
	}
	else if (component->IsList())
	{
		auto list = static_cast<LSG_List*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     list->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    list->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     list->SelectFirstRow(); break;
			case SDLK_END:      list->SelectLastRow(); break;
			case SDLK_UP:       list->SelectRow(-1); break;
			case SDLK_DOWN:     list->SelectRow(1); break;
			case SDLK_PAGEUP:   list->SelectRow(-LSG_List::UnitPage); break;
			case SDLK_PAGEDOWN: list->SelectRow(LSG_List::UnitPage); break;
			case SDLK_RETURN: case SDLK_KP_ENTER: list->Activate(); break;
			default: break;
		}
	}
	else if (component->IsTable())
	{
		auto table = static_cast<LSG_Table*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     table->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    table->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     table->SelectFirstRow(); break;
			case SDLK_END:      table->SelectLastRow(); break;
			case SDLK_UP:       table->SelectRow(-1); break;
			case SDLK_DOWN:     table->SelectRow(1); break;
			case SDLK_PAGEUP:   table->SelectRow(-LSG_List::UnitPage); break;
			case SDLK_PAGEDOWN: table->SelectRow(LSG_List::UnitPage); break;
			case SDLK_RETURN: case SDLK_KP_ENTER: table->Activate(); break;
			default: break;
		}
	}

	LSG_Events::sendEvent(LSG_EVENT_COMPONENT_KEY_ENTERED, component->GetID());
}

void LSG_Events::handleMouseDownEvent(const SDL_Event& event)
{
	if (LSG_Events::isMouseDown)
		return;

	auto mousePosition = LSG_Events::getMousePosition(event);
	auto component     = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if ((event.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_RIGHT)) {
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_RIGHT_CLICKED, component->GetID());
		return;
	}

	LSG_Events::lastClickTime2 = LSG_Events::lastClickTime;
	LSG_Events::lastClickTime  = SDL_GetTicks();
	LSG_Events::lastComponent  = component;

	bool enableMouseDown = false;

	if (component->IsSlider())
		enableMouseDown = static_cast<LSG_Slider*>(component)->OnMouseClickThumb(mousePosition);
	else if (component->IsList())
		enableMouseDown = static_cast<LSG_List*>(component)->OnScrollMouseClick(mousePosition);
	else if (component->IsTable())
		enableMouseDown = static_cast<LSG_Table*>(component)->OnScrollMouseClick(mousePosition);
	else if (component->IsTextLabel())
		enableMouseDown = static_cast<LSG_TextLabel*>(component)->OnScrollMouseClick(mousePosition);

	if (enableMouseDown)
	{
		LSG_Events::isMouseDown = true;
		LSG_Events::lastEvent   = event;

		if (component->IsScrollable())
			LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, component->GetID());
	}
}

void LSG_Events::handleMouseLastDownEvent()
{
	if (!LSG_Events::isMouseDown || !LSG_Events::lastComponent)
		return;

	auto lastPosition  = LSG_Events::getMousePosition(LSG_Events::lastEvent);
	auto lastComponent = LSG_Events::lastComponent;

	if (lastComponent->IsList())
		static_cast<LSG_List*>(lastComponent)->OnScrollMouseDown(lastPosition);
	else if (lastComponent->IsTable())
		static_cast<LSG_Table*>(lastComponent)->OnScrollMouseDown(lastPosition);
	else if (lastComponent->IsTextLabel())
		static_cast<LSG_TextLabel*>(lastComponent)->OnScrollMouseDown(lastPosition);
}

void LSG_Events::handleMouseMoveEvent(const SDL_Event& event)
{
	auto mousePosition = LSG_Events::getMousePosition(event);

	if (!LSG_Events::isMouseDown) {
		LSG_UI::HighlightComponents(mousePosition);
		return;
	}

	if (!LSG_Events::lastComponent)
		return;

	auto lastPosition  = LSG_Events::getMousePosition(LSG_Events::lastEvent);
	auto lastComponent = LSG_Events::lastComponent;

	if (lastComponent->IsSlider())
		static_cast<LSG_Slider*>(lastComponent)->OnMouseMove(mousePosition);
	else if (lastComponent->IsList())
		static_cast<LSG_List*>(lastComponent)->OnScrollMouseMove(mousePosition, lastPosition);
	else if (lastComponent->IsTable())
		static_cast<LSG_Table*>(lastComponent)->OnScrollMouseMove(mousePosition, lastPosition);
	else if (lastComponent->IsTextLabel())
		static_cast<LSG_TextLabel*>(lastComponent)->OnScrollMouseMove(mousePosition, lastPosition);

	if (lastComponent->IsScrollable())
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, lastComponent->GetID());
}

void LSG_Events::handleMouseScrollEvent(const SDL_MouseWheelEvent& event)
{
	int       scroll        = -(event.y * LSG_ScrollBar::UnitWheel);
	SDL_Point mousePosition = { event.mouseX, event.mouseY };
	auto      component     = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (component->IsSlider())
		static_cast<LSG_Slider*>(component)->OnMouseScroll(scroll);
	else if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollVertical(scroll);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollVertical(scroll);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollVertical(scroll);

	LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, component->GetID());
}

void LSG_Events::handleMouseUp(const SDL_Event& event)
{
	if (!LSG_Events::isMouseDown && LSG_Events::lastComponent)
	{
		auto mousePosition = LSG_Events::getMousePosition(event);
		auto component     = LSG_UI::GetComponent(mousePosition);

		if (component && (component->GetID() == LSG_Events::lastComponent->GetID()))
		{
			auto timeSinceLastClick2 = (SDL_GetTicks() - LSG_Events::lastClickTime2);
			auto timeSinceLastClick  = (SDL_GetTicks() - LSG_Events::lastClickTime);
			bool isDoubleClick       = (timeSinceLastClick2 < LSG_ConstClickTime::DoubleClick);
			bool isRightClick        = (timeSinceLastClick >= LSG_ConstClickTime::RightClick);

			if ((event.type == SDL_FINGERUP) && isRightClick)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_RIGHT_CLICKED, component->GetID());
			else if (isDoubleClick && (component->IsList() || component->IsTable()))
				static_cast<LSG_List*>(component)->Activate();
			else if (isDoubleClick)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_DOUBLE_CLICKED, component->GetID());
			else if (component->IsButton())
				static_cast<LSG_Button*>(component)->OnMouseClick(mousePosition);
			else if (component->IsList())
				static_cast<LSG_List*>(component)->OnMouseClick(mousePosition);
			else if (component->IsMenu())
				static_cast<LSG_Menu*>(component)->OnMouseClick(mousePosition);
			else if (component->IsSlider())
				static_cast<LSG_Slider*>(component)->OnMouseClick(mousePosition);
			else if (component->IsTable())
				static_cast<LSG_Table*>(component)->OnMouseClick(mousePosition);
			else
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_CLICKED, component->GetID());
		}
	}

	if (LSG_Events::lastComponent)
	{
		if (LSG_Events::lastComponent->IsSlider())
			static_cast<LSG_Slider*>(LSG_Events::lastComponent)->OnMouseUp();
		else if (LSG_Events::lastComponent->IsList())
			static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTable())
			static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTextLabel())
			static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseUp();
	}

	LSG_Events::isMouseDown   = false;
	LSG_Events::lastComponent = nullptr;
	LSG_Events::lastEvent     = {};

	LSG_UI::UnhighlightComponents();
}

void LSG_Events::handleSysWMEvent(const SDL_SysWMEvent& event)
{
	if (!event.msg)
		return;

	#if defined _windows
	if (event.msg->subsystem != SDL_SYSWM_WINDOWS)
		return;

	switch (event.msg->msg.win.msg) {
		case WM_QUERYENDSESSION: case WM_ENDSESSION: LSG_Quit(); break;
		case WM_DPICHANGED: LSG_UI::Layout(); break;
		default: break;
	}
	#endif
}

void LSG_Events::handleWindowEvent(const SDL_WindowEvent& event)
{
	switch (event.event) {
		case SDL_WINDOWEVENT_CLOSE:        LSG_Quit(); break;
		case SDL_WINDOWEVENT_SIZE_CHANGED: LSG_UI::Layout(); break;
		default: break;
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
		case SDL_QUIT:
			LSG_Quit();
			break;
		case SDL_DISPLAYEVENT:
			LSG_UI::Layout();
			break;
		case SDL_KEYDOWN:
			LSG_Events::handleKeyDownEvent(event.key);
			break;
		case SDL_SYSWMEVENT:
			LSG_Events::handleSysWMEvent(event.syswm);
			break;
		case SDL_WINDOWEVENT:
			LSG_Events::handleWindowEvent(event.window);
			break;
		case SDL_MOUSEWHEEL:
			LSG_Events::handleMouseScrollEvent(event.wheel);
			break;
		case SDL_FINGERDOWN:
		case SDL_MOUSEBUTTONDOWN:
			LSG_Events::handleMouseDownEvent(event);
			break;
		case SDL_FINGERUP:
		case SDL_MOUSEBUTTONUP:
			LSG_Events::handleMouseUp(event);
			break;
		case SDL_FINGERMOTION:
		case SDL_MOUSEMOTION:
			LSG_Events::handleMouseMoveEvent(event);
			break;
		default:
			break;
		}

		events.push_back(SDL_Event(event));
	}

	if (LSG_Events::lastEvent.type == SDL_MOUSEBUTTONDOWN) {
		int x, y;
		LSG_Events::isMouseDown = (SDL_GetMouseState(&x, &y) == SDL_BUTTON_LEFT);
	}

	return events;
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
