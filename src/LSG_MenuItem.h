#include "main.h"

#ifndef LSG_MENU_ITEM_H
#define LSG_MENU_ITEM_H

class LSG_MenuItem : public LSG_Component, public LSG_IEvent
{
public:
	LSG_MenuItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_MenuItem() {}

private:
	bool selected;

public:
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetSelected(bool selected = true);

private:
	virtual void renderDisabledOverlay(SDL_Renderer* renderer) override;
	void         renderHighlight(SDL_Renderer* renderer);
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
