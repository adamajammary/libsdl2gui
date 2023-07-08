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
	static LSG_Component* lastComponent;
	static SDL_Event      lastEvent;

public:
	static bool                   IsDoubleClick(const SDL_MouseButtonEvent& event);
	static bool                   IsMouseDown();
	static std::vector<SDL_Event> Handle();

private:
	static void handleKeyDownEvent(const SDL_KeyboardEvent& event);
	static void handleMouseDownEvent(const SDL_Event& event);
	static void handleMouseLastDownEvent();
	static void handleMouseMoveEvent(const SDL_MouseMotionEvent& event);
	static void handleMouseScrollEvent(const SDL_MouseWheelEvent& event);
	static void handleMouseUp();
	static void handleWindowEvent(const SDL_WindowEvent& event);

};

#endif
