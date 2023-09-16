#include "main.h"

#ifndef LSG_MENU_ITEM_H
#define LSG_MENU_ITEM_H

class LSG_MenuItem : public LSG_Menu
{
public:
	LSG_MenuItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_MenuItem() {}

private:
	bool selected;

public:
	bool         IsSelected();
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetSelected(bool selected = true);
	void         SetValue(const std::string& value);

private:
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
