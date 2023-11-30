#include "main.h"

#ifndef LSG_MENU_SUB_H
#define LSG_MENU_SUB_H

class LSG_MenuSub : public LSG_MenuItem
{
public:
	LSG_MenuSub(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_MenuSub() {}

public:
	static const int PaddingArrow   = 8;
	static const int PaddingArrow2x = 16;

public:
	void         AddItem(const std::string& item, const std::string& itemId);
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetSubMenu(const SDL_Rect& background);

private:
	int  getMaxHeightArrow();
	void renderArrow(SDL_Renderer* renderer);

};

#endif
