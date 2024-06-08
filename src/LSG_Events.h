#include "main.h"

#ifndef LSG_EVENTS_H
#define LSG_EVENTS_H

class LSG_Events
{
private:
	LSG_Events()  {}
	~LSG_Events() {}

private:
	static bool           isMouseDown;
	static uint32_t       lastClickTime;
	static uint32_t       lastClickTime2;
	static LSG_Component* lastComponent;
	static SDL_Event      lastEvent;

public:
	static bool                   IsMouseDown();
	static std::vector<SDL_Event> Handle();

private:
	static SDL_Point getMousePosition(const SDL_Event& event);
	static void      handleKeyDownEvent(const SDL_KeyboardEvent& event);
	static void      handleMouseDownEvent(const SDL_Event& event);
	static void      handleMouseLastDownEvent();
	static void      handleMouseMoveEvent(const SDL_Event& event);
	static void      handleMouseScrollEvent(const SDL_MouseWheelEvent& event);
	static void      handleMouseUp(const SDL_Event& event);
	static void      handleSysWMEvent(const SDL_SysWMEvent& event);
	static void      handleWindowEvent(const SDL_WindowEvent& event);
	static void      sendEvent(LSG_EventType type, const std::string& id, SDL_Keycode key = SDLK_UNKNOWN);

};

#endif
