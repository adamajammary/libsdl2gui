#include "main.h"

#ifndef LSG_MENU_SUB_H
#define LSG_MENU_SUB_H

class LSG_MenuSub : public LSG_Text, public LSG_IEvent
{
public:
	LSG_MenuSub(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_MenuSub() {}

private:
	LSG_Strings items;
	int         selectedSubMenu;

public:
	void         AddItem(const std::string& item, const std::string& itemId = "");
	void         Close();
	LSG_Strings  GetItems();
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetItems();

private:
	int          getRowHeight(LSG_Component* component);
	int          getSelectedSubMenu(const SDL_Point& mousePosition, LSG_Component* component);
	bool         open(const SDL_Point& mousePosition, int index);
	virtual void renderDisabledOverlay(SDL_Renderer* renderer) override;
	void         renderHighlight(SDL_Renderer* renderer);
	void         renderHighlightSelection(SDL_Renderer* renderer, int index, LSG_Component* component);
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
