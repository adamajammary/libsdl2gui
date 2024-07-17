#include "LSG_Events.h"

bool           LSG_Events::isMouseDown    = false;
uint32_t       LSG_Events::lastClickTime  = 0;
uint32_t       LSG_Events::lastClickTime2 = 0;
LSG_Component* LSG_Events::lastComponent  = nullptr;
SDL_Event      LSG_Events::lastEvent      = {};
LSG_Component* LSG_Events::textInput      = nullptr;

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
	if (LSG_Events::textInput)
	{
		auto textInput = static_cast<LSG_TextInput*>(LSG_Events::textInput);

		if (!textInput->enabled || !textInput->visible)
			return;

		if (event.keysym.mod & KMOD_CTRL)
		{
			switch (event.keysym.sym) {
				case SDLK_a: textInput->SelectAll(); break;
				case SDLK_c: textInput->Copy(); break;
				case SDLK_v: textInput->Paste(); break;
				default: break;
			}

			return;
		}

		if (event.keysym.mod & KMOD_SHIFT)
		{
			switch (event.keysym.sym) {
				case SDLK_HOME:  textInput->SelectHome(); break;
				case SDLK_END:   textInput->SelectEnd(); break;
				case SDLK_LEFT:  textInput->SelectLeft(); break;
				case SDLK_RIGHT: textInput->SelectRight(); break;
				default: break;
			}

			return;
		}

		switch (event.keysym.sym) {
			case SDLK_HOME:      textInput->MoveCursorHome(); break;
			case SDLK_END:       textInput->MoveCursorEnd(); break;
			case SDLK_LEFT:      textInput->MoveCursorLeft(); break;
			case SDLK_RIGHT:     textInput->MoveCursorRight(); break;
			case SDLK_BACKSPACE: textInput->Remove(); break;
			case SDLK_DELETE:    textInput->Delete(); break;
			case SDLK_ESCAPE:    textInput->Stop(); break;
			case SDLK_RETURN: case SDLK_KP_ENTER: textInput->Complete(); break;
			default: break;
		}

		return;
	}

	auto component = LSG_UI::GetComponent(LSG_Window::GetMousePosition(), true);

	if (!component || !component->enabled)
		return;

	if (component->IsModal())
	{
		if (static_cast<LSG_Modal*>(component)->OnKeyDown(event))
			return;
	}
	else if (component->IsMenu())
	{
		auto menu = static_cast<LSG_Menu*>(component);

		if (!menu->IsOpen())
			return;

		switch (event.keysym.sym) {
			case SDLK_ESCAPE:   menu->Close(); break;
			case SDLK_HOME:     menu->OnScrollHome(); break;
			case SDLK_END:      menu->OnScrollEnd(); break;
			case SDLK_UP:       menu->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     menu->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   menu->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: menu->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}

		return;
	}
	else if (component->IsSlider())
	{
		auto slider = static_cast<LSG_Slider*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     slider->OnMouseScroll(LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    slider->OnMouseScroll(-LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     slider->SetValue(0.0); break;
			case SDLK_END:      slider->SetValue(1.0); break;
			case SDLK_UP:       slider->OnMouseScroll(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     slider->OnMouseScroll(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   slider->OnMouseScroll(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: slider->OnMouseScroll(LSG_ScrollBar::UnitPage); break;
			default: break;
		}

		return;
	}
	else if (component->IsList() || component->IsTable())
	{
		auto list = static_cast<LSG_List*>(component);

		if (event.keysym.mod & KMOD_CTRL)
		{
			switch (event.keysym.sym) {
				case SDLK_a:     list->SelectAll(); break;
				case SDLK_LEFT:  list->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
				case SDLK_RIGHT: list->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
				case SDLK_UP:    list->OnScrollVertical(-LSG_ScrollBar::Unit); break;
				case SDLK_DOWN:  list->OnScrollVertical(LSG_ScrollBar::Unit); break;
				default: break;
			}

			return;
		}

		if (event.keysym.mod & KMOD_SHIFT)
		{
			switch (event.keysym.sym) {
				case SDLK_HOME:     list->SelectFirstRowShift(); break;
				case SDLK_END:      list->SelectLastRowShift(); break;
				case SDLK_UP:       list->SelectRow(-1, true); break;
				case SDLK_DOWN:     list->SelectRow(1,  true); break;
				case SDLK_PAGEUP:   list->SelectRow(-LSG_List::UnitPage, true); break;
				case SDLK_PAGEDOWN: list->SelectRow(LSG_List::UnitPage,  true); break;
				default: break;
			}

			return;
		}

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

		return;
	}

	bool isHandled = false;

	if (component->IsTextLabel())
	{
		auto textLabel = static_cast<LSG_TextLabel*>(component);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     isHandled = textLabel->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    isHandled = textLabel->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     isHandled = textLabel->OnScrollHome(); break;
			case SDLK_END:      isHandled = textLabel->OnScrollEnd(); break;
			case SDLK_UP:       isHandled = textLabel->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     isHandled = textLabel->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   isHandled = textLabel->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: isHandled = textLabel->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}
	}

	if (isHandled)
		return;

	auto scrollableParent = component->GetScrollableParent();

	if (scrollableParent)
	{
		auto panel = static_cast<LSG_Panel*>(scrollableParent);

		switch (event.keysym.sym) {
			case SDLK_LEFT:     isHandled = panel->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    isHandled = panel->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     isHandled = panel->OnScrollHome(); break;
			case SDLK_END:      isHandled = panel->OnScrollEnd(); break;
			case SDLK_UP:       isHandled = panel->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     isHandled = panel->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   isHandled = panel->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: isHandled = panel->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}
	}

	if (!isHandled)
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_KEY_ENTERED, component->GetID(), event.keysym.sym);
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

	auto scrolledPosition = LSG_UI::GetScrolledPosition(mousePosition, component);
	bool enableMouseDown  = false;

	auto timeSinceLastClick2 = (SDL_GetTicks() - LSG_Events::lastClickTime2);
	bool isDoubleClick       = (timeSinceLastClick2 < LSG_ConstClickTime::DoubleClick);

	if (LSG_Events::textInput && (LSG_Events::textInput->GetID() != component->GetID()))
		static_cast<LSG_TextInput*>(LSG_Events::textInput)->Stop();

	if (component->IsSlider())
		enableMouseDown = static_cast<LSG_Slider*>(component)->OnMouseClickThumb(scrolledPosition);
	else if (component->IsList())
		enableMouseDown = static_cast<LSG_List*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsMenu())
		enableMouseDown = static_cast<LSG_Menu*>(component)->OnScrollMouseClick(mousePosition);
	else if (component->IsTable())
		enableMouseDown = static_cast<LSG_Table*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsTextInput() && !isDoubleClick)
		enableMouseDown = static_cast<LSG_TextInput*>(component)->OnMouseClick(mousePosition);
	else if (component->IsTextLabel())
		enableMouseDown = static_cast<LSG_TextLabel*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsButton())
		enableMouseDown = true;

	if (!enableMouseDown)
	{
		auto scrollableParent = component->GetScrollableParent();

		if (scrollableParent)
			enableMouseDown = static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseClick(mousePosition);
	}

	if (!enableMouseDown)
		return;

	LSG_Events::isMouseDown = true;
	LSG_Events::lastEvent   = event;

	if (component->IsScrollable())
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, component->GetID());
	else if (component->IsButton())
		LSG_Events::sendEvent(LSG_EVENT_BUTTON_PRESSED, component->GetID());
}

void LSG_Events::handleMouseLastDownEvent()
{
	if (!LSG_Events::isMouseDown || !LSG_Events::lastComponent)
		return;

	if (LSG_Events::lastComponent->IsButton()) {
		LSG_Events::sendEvent(LSG_EVENT_BUTTON_PRESSED, LSG_Events::lastComponent->GetID());
		return;
	}

	auto lastPosition     = LSG_Events::getMousePosition(LSG_Events::lastEvent);
	auto scrolledPosition = LSG_UI::GetScrolledPosition(lastPosition, LSG_Events::lastComponent);

	bool isHandled = false;

	if (LSG_Events::lastComponent->IsList())
		isHandled = static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsMenu())
		isHandled = static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseDown(lastPosition);
	else if (LSG_Events::lastComponent->IsTable())
		isHandled = static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);

	auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

	if (!isHandled && scrollableParent)
		static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseDown(lastPosition);
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

	auto scrolledPosition = LSG_UI::GetScrolledPosition(mousePosition, LSG_Events::lastComponent);

	auto lastPosition         = LSG_Events::getMousePosition(LSG_Events::lastEvent);
	auto scrolledLastPosition = LSG_UI::GetScrolledPosition(lastPosition, LSG_Events::lastComponent);

	bool isHandled = false;

	if (LSG_Events::lastComponent->IsSlider())
		isHandled = static_cast<LSG_Slider*>(LSG_Events::lastComponent)->OnMouseMove(scrolledPosition);
	else if (LSG_Events::lastComponent->IsList())
		isHandled = static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsMenu())
		isHandled = static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseMove(mousePosition, lastPosition);
	else if (LSG_Events::lastComponent->IsTable())
		isHandled = static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsTextInput())
		isHandled = static_cast<LSG_TextInput*>(LSG_Events::lastComponent)->OnMouseMove(mousePosition);
	else if (LSG_Events::lastComponent->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);

	auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

	if (!isHandled && scrollableParent)
		static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseMove(mousePosition, lastPosition);

	if (LSG_Events::lastComponent->IsScrollable())
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, LSG_Events::lastComponent->GetID());
}

void LSG_Events::handleMouseScrollEvent(const SDL_MouseWheelEvent& event)
{
	int       scroll        = -(event.y * LSG_ScrollBar::UnitWheel);
	SDL_Point mousePosition = { event.mouseX, event.mouseY };
	auto      component     = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (component->IsMenu()) {
		static_cast<LSG_Menu*>(component)->OnScrollVertical(scroll);
		return;
	}

	if (component->IsSlider()) {
		static_cast<LSG_Slider*>(component)->OnMouseScroll(scroll);
		return;
	}

	bool isHandled = false;

	if (component->IsList())
		isHandled = static_cast<LSG_List*>(component)->OnScrollVertical(scroll);
	else if (component->IsTable())
		isHandled = static_cast<LSG_Table*>(component)->OnScrollVertical(scroll);
	else if (component->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(component)->OnScrollVertical(scroll);

	if (isHandled)
		return;

	auto scrollableParent = component->GetScrollableParent();

	if (scrollableParent)
		isHandled = static_cast<LSG_Panel*>(scrollableParent)->OnScrollVertical(scroll);

	if (!isHandled)
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, component->GetID());
}

void LSG_Events::handleMouseUp(const SDL_Event& event)
{
	auto timeSinceLastClick2 = (SDL_GetTicks() - LSG_Events::lastClickTime2);
	auto timeSinceLastClick  = (SDL_GetTicks() - LSG_Events::lastClickTime);

	bool isDoubleClick = (timeSinceLastClick2 < LSG_ConstClickTime::DoubleClick);
	bool isRightClick  = (timeSinceLastClick >= LSG_ConstClickTime::RightClick);

	LSG_Component* textInput = nullptr;

	if (!LSG_Events::isMouseDown && LSG_Events::lastComponent)
	{
		auto mousePosition = LSG_Events::getMousePosition(event);
		auto component     = LSG_UI::GetComponent(mousePosition);

		if (component && (component->GetID() == LSG_Events::lastComponent->GetID()))
		{
			auto scrolledPosition = LSG_UI::GetScrolledPosition(mousePosition, component);

			if ((event.type == SDL_FINGERUP) && isRightClick)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_RIGHT_CLICKED, component->GetID());
			else if (isDoubleClick && (component->IsList() || component->IsTable()))
				static_cast<LSG_List*>(component)->Activate(scrolledPosition);
			else if (isDoubleClick && component->IsTextInput())
				static_cast<LSG_TextInput*>(component)->SelectWord(scrolledPosition);
			else if (isDoubleClick)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_DOUBLE_CLICKED, component->GetID());
			else if (component->IsList())
				static_cast<LSG_List*>(component)->OnMouseClick(scrolledPosition);
			else if (component->IsMenu())
				static_cast<LSG_Menu*>(component)->OnMouseClick(mousePosition);
			else if (component->IsSlider())
				static_cast<LSG_Slider*>(component)->OnMouseClick(scrolledPosition);
			else if (component->IsTable())
				static_cast<LSG_Table*>(component)->OnMouseClick(scrolledPosition);
			else
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_CLICKED, component->GetID());
		}
	}

	if (LSG_Events::lastComponent)
	{
		auto lastPosition     = LSG_Events::getMousePosition(LSG_Events::lastEvent);
		auto scrolledPosition = LSG_UI::GetScrolledPosition(lastPosition, LSG_Events::lastComponent);

		if (LSG_Events::lastComponent->IsSlider())
			static_cast<LSG_Slider*>(LSG_Events::lastComponent)->OnMouseUp();
		else if (LSG_Events::lastComponent->IsList())
			static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsMenu())
			static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTable())
			static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTextLabel())
			static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTextInput())
			textInput = LSG_Events::lastComponent;
		else if (LSG_Events::lastComponent->IsButton())
			static_cast<LSG_Button*>(LSG_Events::lastComponent)->OnMouseClick(scrolledPosition);

		auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

		if (scrollableParent)
			static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseUp();
	}

	if (!textInput && LSG_Events::textInput && !isDoubleClick)
		static_cast<LSG_TextInput*>(LSG_Events::textInput)->Stop();

	LSG_Events::isMouseDown   = false;
	LSG_Events::lastComponent = nullptr;
	LSG_Events::lastEvent     = {};
	LSG_Events::textInput     = textInput;

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

void LSG_Events::handleTextInput(const SDL_TextInputEvent& event)
{
	if (LSG_Events::textInput)
		static_cast<LSG_TextInput*>(LSG_Events::textInput)->Input(event.text);
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
			case SDL_QUIT:         LSG_Quit(); break;
			case SDL_DISPLAYEVENT: LSG_UI::Layout(); break;
			case SDL_KEYDOWN:      LSG_Events::handleKeyDownEvent(event.key); break;
			case SDL_TEXTINPUT:    LSG_Events::handleTextInput(event.text); break;
			case SDL_SYSWMEVENT:   LSG_Events::handleSysWMEvent(event.syswm); break;
			case SDL_WINDOWEVENT:  LSG_Events::handleWindowEvent(event.window); break;
			case SDL_MOUSEWHEEL:   LSG_Events::handleMouseScrollEvent(event.wheel); break;
			case SDL_FINGERDOWN:   case SDL_MOUSEBUTTONDOWN: LSG_Events::handleMouseDownEvent(event); break;
			case SDL_FINGERUP:     case SDL_MOUSEBUTTONUP:   LSG_Events::handleMouseUp(event); break;
			case SDL_FINGERMOTION: case SDL_MOUSEMOTION:     LSG_Events::handleMouseMoveEvent(event); break;
			default: break;
		}

		events.push_back(SDL_Event(event));
	}

	if (LSG_Events::lastEvent.type == SDL_MOUSEBUTTONDOWN)
		LSG_Events::isMouseDown = (SDL_GetMouseState(nullptr, nullptr) == SDL_BUTTON_LEFT);

	return events;
}

bool LSG_Events::IsMouseDown()
{
	return LSG_Events::isMouseDown;
}

void LSG_Events::sendEvent(LSG_EventType type, const std::string& id, SDL_Keycode key)
{
	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int)type;
	clickEvent.user.data1 = (void*)strdup(id.c_str());

	if (key != SDLK_UNKNOWN)
		clickEvent.user.data2 = new SDL_Keycode(key);

	SDL_PushEvent(&clickEvent);
}
