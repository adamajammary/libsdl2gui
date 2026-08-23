#include "main.h"

#ifndef LSG_EVENTS_H
#define LSG_EVENTS_H

class LSG_Events
{
private:
	LSG_Events()  {}
	~LSG_Events() {}

private:
	static bool           isColumnResize;
	static bool           isMouseDown;
	static uint64_t       lastClickTime;
	static uint64_t       lastClickTime2;
	static LSG_Component* lastComponent;
	static SDL_Event      lastEvent;
	static LSG_Component* textInput;

public:
	static std::vector<SDL_Event> Handle();
	static void                   HandleKeyDownEvent(const SDL_KeyboardEvent& event, LSG_Component* component);
	static void                   HandleMouseDownEvent(const SDL_Event& event, LSG_Component* component = nullptr);
	static void                   HandleMouseScrollEvent(const SDL_MouseWheelEvent& event, LSG_Component* component = nullptr);
	static void                   HandleMouseUpEvent(const SDL_Event& event, LSG_Component* component = nullptr);
	static bool                   IsMouseDown();

private:
	static SDL_Point getMousePosition(const SDL_Event& event);
	static void      handleKeyDownEvent(const SDL_KeyboardEvent& event);
	static void      handleKeyDownEventCards(const SDL_KeyboardEvent& event, LSG_Cards* cards);
	static void      handleKeyDownEventList(const SDL_KeyboardEvent& event, LSG_List* list);
	static void      handleKeyDownEventMenu(const SDL_KeyboardEvent& event, LSG_Menu* menu);
	static bool      handleKeyDownEventPanel(const SDL_KeyboardEvent& event, LSG_Panel* panel);
	static void      handleKeyDownEventSlider(const SDL_KeyboardEvent& event, LSG_Slider* slider);
	static bool      handleKeyDownEventTextLabel(const SDL_KeyboardEvent& event, LSG_TextLabel* textLabel);
	static void      handleKeyDownEventTiles(const SDL_KeyboardEvent& event, LSG_Tiles* tiles);
	static void      handleKeyDownEventTextInput(const SDL_KeyboardEvent& event);
	static void      handleMouseLastDownEvent();
	static void      handleMouseMoveEvent(const SDL_Event& event);
	static void      handleTextInputEvent(const SDL_TextInputEvent& event);
	static void      sendEvent(LSG_EventType type, const std::string& id, SDL_Keycode key = SDLK_UNKNOWN);
};

#endif
