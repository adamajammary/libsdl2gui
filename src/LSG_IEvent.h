#include "main.h"

#ifndef LSG_IEVENT_H
#define LSG_IEVENT_H

class LSG_IEvent
{
public:
	LSG_IEvent() {}
	virtual ~LSG_IEvent() {}

public:
	virtual bool OnMouseClick(const SDL_Point& mousePosition) = 0;

protected:
	virtual void sendEvent(LSG_EventType type) = 0;

};

#endif
