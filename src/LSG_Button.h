#include "main.h"

#ifndef LSG_BUTTON_H
#define LSG_BUTTON_H

class LSG_Button : public LSG_Component, public LSG_IEvent
{
public:
	LSG_Button(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Button() {}

public:
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	virtual void Render(SDL_Renderer* renderer) override;

private:
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
