#include "main.h"

#ifndef LSG_MENU_SUB_H
#define LSG_MENU_SUB_H

class LSG_MenuSub : public LSG_Menu
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
	int          getSelectedIndex();
	bool         open(const SDL_Point& mousePosition, int index);
	void         renderDisabledOverlay(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Color& backgroundColor);
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
