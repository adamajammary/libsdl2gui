#include "main.h"

#ifndef LSG_BUTTON_H
#define LSG_BUTTON_H

class LSG_Button : public LSG_Component, public LSG_IEvent
{
public:
	LSG_Button(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Button() {}

public:
	static const int LayerOffset = 100000000;

private:
	bool selected;

public:
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetSelected(bool selected = true);

private:
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
