#include "LSG_Events.h"

bool           LSG_Events::isColumnResize = false;
bool           LSG_Events::isMouseDown    = false;
uint32_t       LSG_Events::lastClickTime  = 0;
uint32_t       LSG_Events::lastClickTime2 = 0;
LSG_Component* LSG_Events::lastComponent  = nullptr;
SDL_Event      LSG_Events::lastEvent      = {};
LSG_Component* LSG_Events::textInput      = nullptr;

SDL_Point LSG_Events::getMousePosition(const SDL_Event& event)
{
	SDL_Point position = {};

	if ((event.type == SDL_EVENT_FINGER_DOWN) || (event.type == SDL_EVENT_FINGER_UP) || (event.type == SDL_EVENT_FINGER_MOTION))
	{
		auto size = LSG_Window::GetSizeInPixels();
		position  = { (int)(event.tfinger.x * (float)size.width), (int)(event.tfinger.y * (float)size.height) };

		return position;
	}

	#if defined _macosx
		if ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) || (event.type == SDL_EVENT_MOUSE_BUTTON_UP))
			position = { LSG_Window::GetDPIScaled(event.button.x), LSG_Window::GetDPIScaled(event.button.y) };
		else if (event.type == SDL_EVENT_MOUSE_MOTION)
			position = { LSG_Window::GetDPIScaled(event.motion.x), LSG_Window::GetDPIScaled(event.motion.y) };
	#else
		if ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) || (event.type == SDL_EVENT_MOUSE_BUTTON_UP))
			position = { (int)event.button.x, (int)event.button.y };
		else if (event.type == SDL_EVENT_MOUSE_MOTION)
			position = { (int)event.motion.x, (int)event.motion.y };
	#endif

	return position;
}

void LSG_Events::handleKeyDownEvent(const SDL_KeyboardEvent& event)
{
	auto component = LSG_UI::GetComponent(LSG_Window::GetMousePosition());

	LSG_Events::HandleKeyDownEvent(event, component);
}

void LSG_Events::HandleKeyDownEvent(const SDL_KeyboardEvent& event, LSG_Component* component)
{
	if (LSG_Events::textInput) {
		LSG_Events::handleKeyDownEventTextInput(event);
		return;
	}

	if (!component || !component->enabled)
		return;

	if (component->IsModal()) {
		static_cast<LSG_Modal*>(component)->OnKeyDown(event);
		return;
	}

	if (component->IsMenu()) {
		LSG_Events::handleKeyDownEventMenu(event, static_cast<LSG_Menu*>(component));
		return;
	}
	
	if (component->IsSlider()) {
		LSG_Events::handleKeyDownEventSlider(event, static_cast<LSG_Slider*>(component));
		return;
	}
	
	if (component->IsList() || component->IsTable()) {
		LSG_Events::handleKeyDownEventList(event, static_cast<LSG_List*>(component));
		return;
	}
	
	if (component->IsCards()) {
		LSG_Events::handleKeyDownEventCards(event, static_cast<LSG_Cards*>(component));
		return;
	}
	
	if (component->IsTiles()) {
		LSG_Events::handleKeyDownEventTiles(event, static_cast<LSG_Tiles*>(component));
		return;
	}

	bool isHandled = false;

	if (component->IsTextLabel())
		isHandled = LSG_Events::handleKeyDownEventTextLabel(event, static_cast<LSG_TextLabel*>(component));

	if (isHandled)
		return;

	auto scrollableParent = component->GetScrollableParent();

	if (scrollableParent)
		isHandled = LSG_Events::handleKeyDownEventPanel(event, static_cast<LSG_Panel*>(component));

	if (!isHandled)
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_KEY_ENTERED, component->GetID(), event.key);
}

void LSG_Events::handleKeyDownEventCards(const SDL_KeyboardEvent& event, LSG_Cards* cards)
{
	if (!cards || !cards->enabled)
		return;

	if (event.mod & SDL_KMOD_CTRL)
	{
		switch (event.key) {
			case SDLK_A:        cards->SelectAll(); break;
			case SDLK_LEFT:     cards->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    cards->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     cards->OnScrollHome(); break;
			case SDLK_END:      cards->OnScrollEnd(); break;
			case SDLK_UP:       cards->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     cards->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   cards->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: cards->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}

		return;
	}

	if (event.mod & SDL_KMOD_SHIFT)
	{
		switch (event.key) {
			case SDLK_HOME:     cards->SelectFirst(true); break;
			case SDLK_END:      cards->SelectLast(true); break;
			case SDLK_UP:       cards->SelectRow(-1, true); break;
			case SDLK_DOWN:     cards->SelectRow(1,  true); break;
			case SDLK_PAGEUP:   cards->SelectRow(-LSG_Cards::DefaultCardPageRows, true); break;
			case SDLK_PAGEDOWN: cards->SelectRow(LSG_Cards::DefaultCardPageRows,  true); break;
			default: break;
		}

		return;
	}

	switch (event.key) {
		case SDLK_LEFT:     cards->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
		case SDLK_RIGHT:    cards->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
		case SDLK_HOME:     cards->SelectFirst(); break;
		case SDLK_END:      cards->SelectLast(); break;
		case SDLK_UP:       cards->SelectRow(-1); break;
		case SDLK_DOWN:     cards->SelectRow(1); break;
		case SDLK_PAGEUP:   cards->SelectRow(-LSG_Cards::DefaultCardPageRows); break;
		case SDLK_PAGEDOWN: cards->SelectRow(LSG_Cards::DefaultCardPageRows); break;
		case SDLK_RETURN: case SDLK_KP_ENTER: cards->Activate(); break;
		default: break;
	}
}

void LSG_Events::handleKeyDownEventList(const SDL_KeyboardEvent& event, LSG_List* list)
{
	if (!list || !list->enabled)
		return;

	if (event.mod & SDL_KMOD_CTRL)
	{
		switch (event.key) {
			case SDLK_A:        list->SelectAll(); break;
			case SDLK_LEFT:     list->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
			case SDLK_RIGHT:    list->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
			case SDLK_HOME:     list->OnScrollHome(); break;
			case SDLK_END:      list->OnScrollEnd(); break;
			case SDLK_UP:       list->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     list->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   list->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: list->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}

		return;
	}

	if (event.mod & SDL_KMOD_SHIFT)
	{
		switch (event.key) {
			case SDLK_HOME:     list->SelectFirstRowShift(); break;
			case SDLK_END:      list->SelectLastRowShift(); break;
			case SDLK_UP:       list->SelectRow(-1, true); break;
			case SDLK_DOWN:     list->SelectRow(1,  true); break;
			case SDLK_PAGEUP:   list->SelectRow(-LSG_List::DefaultPageRows, true); break;
			case SDLK_PAGEDOWN: list->SelectRow(LSG_List::DefaultPageRows,  true); break;
			default: break;
		}

		return;
	}

	switch (event.key) {
		case SDLK_LEFT:     list->OnScrollHorizontal(-LSG_ScrollBar::Unit); break;
		case SDLK_RIGHT:    list->OnScrollHorizontal(LSG_ScrollBar::Unit); break;
		case SDLK_HOME:     list->SelectFirstRow(); break;
		case SDLK_END:      list->SelectLastRow(); break;
		case SDLK_UP:       list->SelectRow(-1); break;
		case SDLK_DOWN:     list->SelectRow(1); break;
		case SDLK_PAGEUP:   list->SelectRow(-LSG_List::DefaultPageRows); break;
		case SDLK_PAGEDOWN: list->SelectRow(LSG_List::DefaultPageRows); break;
		case SDLK_RETURN: case SDLK_KP_ENTER: list->Activate(); break;
		default: break;
	}
}

void LSG_Events::handleKeyDownEventMenu(const SDL_KeyboardEvent& event, LSG_Menu* menu)
{
	if (!menu || !menu->enabled || !menu->IsOpen())
		return;

	switch (event.key) {
		case SDLK_ESCAPE:   menu->Close(); break;
		case SDLK_HOME:     menu->OnScrollHome(); break;
		case SDLK_END:      menu->OnScrollEnd(); break;
		case SDLK_UP:       menu->OnScrollVertical(-LSG_ScrollBar::Unit); break;
		case SDLK_DOWN:     menu->OnScrollVertical(LSG_ScrollBar::Unit); break;
		case SDLK_PAGEUP:   menu->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
		case SDLK_PAGEDOWN: menu->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
		default: break;
	}
}

bool LSG_Events::handleKeyDownEventPanel(const SDL_KeyboardEvent& event, LSG_Panel* panel)
{
	if (!panel || !panel->enabled)
		return false;

	switch (event.key) {
		case SDLK_LEFT:     return panel->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		case SDLK_RIGHT:    return panel->OnScrollHorizontal(LSG_ScrollBar::Unit);
		case SDLK_HOME:     return panel->OnScrollHome();
		case SDLK_END:      return panel->OnScrollEnd();
		case SDLK_UP:       return panel->OnScrollVertical(-LSG_ScrollBar::Unit);
		case SDLK_DOWN:     return panel->OnScrollVertical(LSG_ScrollBar::Unit);
		case SDLK_PAGEUP:   return panel->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		case SDLK_PAGEDOWN: return panel->OnScrollVertical(LSG_ScrollBar::UnitPage);
		default: break;
	}

	return false;
}

void LSG_Events::handleKeyDownEventSlider(const SDL_KeyboardEvent& event, LSG_Slider* slider)
{
	if (!slider || !slider->enabled)
		return;

	switch (event.key) {
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
}

bool LSG_Events::handleKeyDownEventTextLabel(const SDL_KeyboardEvent& event, LSG_TextLabel* textLabel)
{
	if (!textLabel || !textLabel->enabled)
		return false;

	switch (event.key) {
		case SDLK_LEFT:     return textLabel->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		case SDLK_RIGHT:    return textLabel->OnScrollHorizontal(LSG_ScrollBar::Unit);
		case SDLK_HOME:     return textLabel->OnScrollHome();
		case SDLK_END:      return textLabel->OnScrollEnd();
		case SDLK_UP:       return textLabel->OnScrollVertical(-LSG_ScrollBar::Unit);
		case SDLK_DOWN:     return textLabel->OnScrollVertical(LSG_ScrollBar::Unit);
		case SDLK_PAGEUP:   return textLabel->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		case SDLK_PAGEDOWN: return textLabel->OnScrollVertical(LSG_ScrollBar::UnitPage);
		default: break;
	}

	return false;
}

void LSG_Events::handleKeyDownEventTiles(const SDL_KeyboardEvent& event, LSG_Tiles* tiles)
{
	if (!tiles || !tiles->enabled)
		return;

	if (event.mod & SDL_KMOD_CTRL)
	{
		switch (event.key) {
			case SDLK_A:        tiles->SelectAll(); break;
			case SDLK_HOME:     tiles->OnScrollHome(); break;
			case SDLK_END:      tiles->OnScrollEnd(); break;
			case SDLK_UP:       tiles->OnScrollVertical(-LSG_ScrollBar::Unit); break;
			case SDLK_DOWN:     tiles->OnScrollVertical(LSG_ScrollBar::Unit); break;
			case SDLK_PAGEUP:   tiles->OnScrollVertical(-LSG_ScrollBar::UnitPage); break;
			case SDLK_PAGEDOWN: tiles->OnScrollVertical(LSG_ScrollBar::UnitPage); break;
			default: break;
		}

		return;
	}

	if (event.mod & SDL_KMOD_SHIFT)
	{
		switch (event.key) {
			case SDLK_HOME:     tiles->SelectFirst(true); break;
			case SDLK_END:      tiles->SelectLast(true); break;
			case SDLK_LEFT:     tiles->SelectPrevious(true); break;
			case SDLK_RIGHT:    tiles->SelectNext(true); break;
			case SDLK_UP:       tiles->SelectPreviousRow(true); break;
			case SDLK_DOWN:     tiles->SelectNextRow(true); break;
			case SDLK_PAGEUP:   tiles->SelectPreviousPage(true); break;
			case SDLK_PAGEDOWN: tiles->SelectNextPage(true); break;
			default: break;
		}

		return;
	}

	switch (event.key) {
		case SDLK_HOME:     tiles->SelectFirst(); break;
		case SDLK_END:      tiles->SelectLast(); break;
		case SDLK_LEFT:     tiles->SelectPrevious(); break;
		case SDLK_RIGHT:    tiles->SelectNext(); break;
		case SDLK_UP:       tiles->SelectPreviousRow(); break;
		case SDLK_DOWN:     tiles->SelectNextRow(); break;
		case SDLK_PAGEUP:   tiles->SelectPreviousPage(); break;
		case SDLK_PAGEDOWN: tiles->SelectNextPage(); break;
		case SDLK_RETURN: case SDLK_KP_ENTER: tiles->Activate(); break;
		default: break;
	}
}

void LSG_Events::handleKeyDownEventTextInput(const SDL_KeyboardEvent& event)
{
	auto textInput = static_cast<LSG_TextInput*>(LSG_Events::textInput);

	if (!textInput->enabled || !textInput->visible)
		return;

	if (event.mod & SDL_KMOD_CTRL)
	{
		switch (event.key) {
			case SDLK_A: textInput->SelectAll(); break;
			case SDLK_C: textInput->Copy(); break;
			case SDLK_V: textInput->Paste(); break;
			default: break;
		}

		return;
	}

	if (event.mod & SDL_KMOD_SHIFT)
	{
		switch (event.key) {
			case SDLK_HOME:  textInput->SelectHome(); break;
			case SDLK_END:   textInput->SelectEnd(); break;
			case SDLK_LEFT:  textInput->SelectLeft(); break;
			case SDLK_RIGHT: textInput->SelectRight(); break;
			default: break;
		}

		return;
	}

	switch (event.key) {
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
}

void LSG_Events::HandleMouseDownEvent(const SDL_Event& event, LSG_Component* component)
{
	if (LSG_Events::isMouseDown)
		return;

	auto mousePosition = LSG_Events::getMousePosition(event);

	if (!component)
		component = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) && (event.button.button == SDL_BUTTON_RIGHT)) {
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_RIGHT_CLICKED, component->GetID());
		return;
	}

	if (component->IsModal()) {
		static_cast<LSG_Modal*>(component)->OnMouseDown(event, mousePosition);
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
	else if (component->IsCards())
		enableMouseDown = static_cast<LSG_Cards*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsList())
		enableMouseDown = static_cast<LSG_List*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsMenu())
		enableMouseDown = static_cast<LSG_Menu*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsTiles())
		enableMouseDown = static_cast<LSG_Tiles*>(component)->OnScrollMouseClick(scrolledPosition);
	else if (component->IsTextInput() && !isDoubleClick)
		enableMouseDown = static_cast<LSG_TextInput*>(component)->OnMouseDown(mousePosition);
	else if (component->IsTextLabel())
		enableMouseDown = static_cast<LSG_TextLabel*>(component)->OnScrollMouseClick(scrolledPosition);

	if (!enableMouseDown && component->IsTable())
	{
		auto table = static_cast<LSG_Table*>(component);

		LSG_Events::isColumnResize = table->IsMouseOverColumnBorder(scrolledPosition);

		if (!LSG_Events::isColumnResize)
			enableMouseDown = table->OnScrollMouseClick(scrolledPosition);
		else
			enableMouseDown = true;
	}

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

	if (LSG_Events::isColumnResize)
		LSG_Events::sendEvent(LSG_EVENT_TABLE_COLUMN_RESIZED, component->GetID());
}

void LSG_Events::handleMouseLastDownEvent()
{
	if (!LSG_Events::isMouseDown || !LSG_Events::lastComponent)
		return;

	auto lastPosition     = LSG_Events::getMousePosition(LSG_Events::lastEvent);
	auto scrolledPosition = LSG_UI::GetScrolledPosition(lastPosition, LSG_Events::lastComponent);

	bool isHandled = false;

	if (LSG_Events::lastComponent->IsCards())
		isHandled = static_cast<LSG_Cards*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsList())
		isHandled = static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsMenu())
		isHandled = static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsTable())
		isHandled = static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);
	else if (LSG_Events::lastComponent->IsTiles())
		isHandled = static_cast<LSG_Tiles*>(LSG_Events::lastComponent)->OnScrollMouseDown(scrolledPosition);

	if (isHandled)
		return;

	auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

	if (scrollableParent)
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
	else if (LSG_Events::lastComponent->IsTextInput())
		isHandled = static_cast<LSG_TextInput*>(LSG_Events::lastComponent)->OnMouseMove(mousePosition);
	else if (LSG_Events::lastComponent->IsCards())
		isHandled = static_cast<LSG_Cards*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsList())
		isHandled = static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsMenu())
		isHandled = static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	else if (LSG_Events::lastComponent->IsTiles())
		isHandled = static_cast<LSG_Tiles*>(LSG_Events::lastComponent)->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);

	if (!isHandled && LSG_Events::lastComponent->IsTable())
	{
		auto table = static_cast<LSG_Table*>(LSG_Events::lastComponent);

		if (LSG_Events::isColumnResize)
			isHandled = table->OnMouseMove(scrolledPosition, scrolledLastPosition);
		else
			isHandled = table->OnScrollMouseMove(scrolledPosition, scrolledLastPosition);
	}

	if (!isHandled)
	{
		auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

		if (scrollableParent)
			static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseMove(mousePosition, lastPosition);
	}

	if (LSG_Events::isColumnResize)
		LSG_Events::sendEvent(LSG_EVENT_TABLE_COLUMN_RESIZED, LSG_Events::lastComponent->GetID());
	else if (LSG_Events::lastComponent->IsScrollable())
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, LSG_Events::lastComponent->GetID());
}

void LSG_Events::HandleMouseScrollEvent(const SDL_MouseWheelEvent& event, LSG_Component* component)
{
	int       scrollOffset  = -(event.y * LSG_ScrollBar::UnitWheel);
	SDL_Point mousePosition = { (int)event.mouse_x, (int)event.mouse_y };

	if (!component)
		component = LSG_UI::GetComponent(mousePosition);

	if (!component || !component->enabled)
		return;

	if (component->IsScrollable())
		LSG_Events::sendEvent(LSG_EVENT_COMPONENT_SCROLLED, component->GetID());

	if (component->IsMenu()) {
		static_cast<LSG_Menu*>(component)->OnScrollVertical(scrollOffset);
		return;
	}

	if (component->IsModal()) {
		static_cast<LSG_Modal*>(component)->OnMouseScroll(event, mousePosition);
		return;
	}

	if (component->IsSlider()) {
		static_cast<LSG_Slider*>(component)->OnMouseScroll(scrollOffset);
		return;
	}

	bool isHandled = false;

	if (component->IsList())
		isHandled = static_cast<LSG_List*>(component)->OnScrollVertical(scrollOffset);
	else if (component->IsTable())
		isHandled = static_cast<LSG_Table*>(component)->OnScrollVertical(scrollOffset);
	else if (component->IsTextLabel())
		isHandled = static_cast<LSG_TextLabel*>(component)->OnScrollVertical(scrollOffset);
	else if (component->IsCards())
		isHandled = static_cast<LSG_Cards*>(component)->OnScrollVertical(scrollOffset);
	else if (component->IsTiles())
		isHandled = static_cast<LSG_Tiles*>(component)->OnScrollVertical(scrollOffset);

	if (isHandled)
		return;

	auto scrollableParent = component->GetScrollableParent();

	if (scrollableParent)
		static_cast<LSG_Panel*>(scrollableParent)->OnScrollVertical(scrollOffset);
}

void LSG_Events::HandleMouseUpEvent(const SDL_Event& event, LSG_Component* component)
{
	auto timeSinceLastClick2 = (SDL_GetTicks() - LSG_Events::lastClickTime2);
	auto timeSinceLastClick  = (SDL_GetTicks() - LSG_Events::lastClickTime);

	bool isDoubleClick = (timeSinceLastClick2 < LSG_ConstClickTime::DoubleClick);
	bool isLongPress   = (timeSinceLastClick >= LSG_ConstClickTime::LongPress);

	if (!LSG_Events::isMouseDown && LSG_Events::lastComponent)
	{
		auto mousePosition = LSG_Events::getMousePosition(event);

		if (!component)
			component = LSG_UI::GetComponent(mousePosition);

		if (component && component->IsModal()) {
			static_cast<LSG_Modal*>(component)->OnMouseUp(event, mousePosition);
			return;
		}

		if (component && (component->GetID() == LSG_Events::lastComponent->GetID()))
		{
			auto scrolledPosition = LSG_UI::GetScrolledPosition(mousePosition, component);

			if (isDoubleClick)
			{
				if (component->IsList() || component->IsTable())
					static_cast<LSG_List*>(component)->Activate(scrolledPosition);
				else if (component->IsTextInput())
					static_cast<LSG_TextInput*>(component)->SelectWord(scrolledPosition);
				else if (component->IsCards())
					static_cast<LSG_Cards*>(component)->Activate(scrolledPosition);
				else if (component->IsTiles())
					static_cast<LSG_Tiles*>(component)->Activate(scrolledPosition);
			}
			else if (!isLongPress)
			{
				if (component->IsList())
					static_cast<LSG_List*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsMenu())
					static_cast<LSG_Menu*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsNavigation())
					static_cast<LSG_Navigation*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsSlider())
					static_cast<LSG_Slider*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsTable())
					static_cast<LSG_Table*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsCards())
					static_cast<LSG_Cards*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsTiles())
					static_cast<LSG_Tiles*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsToggle())
					static_cast<LSG_Toggle*>(component)->OnMouseClick(scrolledPosition);
				else if (component->IsButton())
					static_cast<LSG_Button*>(component)->OnMouseClick();
			}

			if (isDoubleClick)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_DOUBLE_CLICKED, component->GetID());
			else if (isLongPress && (event.type == SDL_EVENT_FINGER_UP))
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_LONG_PRESSED, component->GetID());
			else if (!isLongPress)
				LSG_Events::sendEvent(LSG_EVENT_COMPONENT_CLICKED, component->GetID());
		}
	}

	LSG_Component* textInput = nullptr;

	if (LSG_Events::lastComponent)
	{
		auto lastPosition     = LSG_Events::getMousePosition(LSG_Events::lastEvent);
		auto scrolledPosition = LSG_UI::GetScrolledPosition(lastPosition, LSG_Events::lastComponent);

		if (LSG_Events::lastComponent->IsSlider())
			static_cast<LSG_Slider*>(LSG_Events::lastComponent)->OnMouseUp();
		else if (LSG_Events::lastComponent->IsTable() && LSG_Events::isColumnResize)
			static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnMouseUp(isDoubleClick);
		else if (LSG_Events::lastComponent->IsTable())
			static_cast<LSG_Table*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsCards())
			static_cast<LSG_Cards*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsList())
			static_cast<LSG_List*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsMenu())
			static_cast<LSG_Menu*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTextLabel())
			static_cast<LSG_TextLabel*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTiles())
			static_cast<LSG_Tiles*>(LSG_Events::lastComponent)->OnScrollMouseUp();
		else if (LSG_Events::lastComponent->IsTextInput())
			textInput = LSG_Events::lastComponent;

		auto scrollableParent = LSG_Events::lastComponent->GetScrollableParent();

		if (scrollableParent)
			static_cast<LSG_Panel*>(scrollableParent)->OnScrollMouseUp();

		if (LSG_Events::isColumnResize && isDoubleClick)
			LSG_Events::sendEvent(LSG_EVENT_TABLE_COLUMN_RESIZED, LSG_Events::lastComponent->GetID());
	}

	if (!textInput && LSG_Events::textInput && !isDoubleClick)
		static_cast<LSG_TextInput*>(LSG_Events::textInput)->Stop();

	LSG_Events::isColumnResize = false;
	LSG_Events::isMouseDown    = false;
	LSG_Events::lastComponent  = nullptr;
	LSG_Events::lastEvent      = {};
	LSG_Events::textInput      = textInput;

	LSG_UI::UnhighlightComponents();
}

void LSG_Events::handleTextInputEvent(const SDL_TextInputEvent& event)
{
	if (LSG_Events::textInput)
		static_cast<LSG_TextInput*>(LSG_Events::textInput)->Input(event.text);
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
		case SDL_EVENT_QUIT:
		case SDL_EVENT_TERMINATING:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			LSG_Quit();
			break;
		case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
		case SDL_EVENT_DISPLAY_ORIENTATION:
		case SDL_EVENT_RENDER_DEVICE_RESET:
		case SDL_EVENT_RENDER_TARGETS_RESET:
		case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		case SDL_EVENT_WINDOW_RESTORED:
			LSG_UI::Layout();
			break;
		case SDL_EVENT_KEY_DOWN:
			LSG_Events::handleKeyDownEvent(event.key);
			break;
		case SDL_EVENT_TEXT_INPUT:
			LSG_Events::handleTextInputEvent(event.text);
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			LSG_Events::HandleMouseScrollEvent(event.wheel);
			break;
		case SDL_EVENT_FINGER_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			LSG_Events::HandleMouseDownEvent(event);
			break;
		case SDL_EVENT_FINGER_UP:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			LSG_Events::HandleMouseUpEvent(event);
			break;
		case SDL_EVENT_FINGER_MOTION:
		case SDL_EVENT_MOUSE_MOTION:
			LSG_Events::handleMouseMoveEvent(event);
			break;
		default:
			break;
		}

		events.push_back(SDL_Event(event));
	}

	if (LSG_Events::lastEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
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
