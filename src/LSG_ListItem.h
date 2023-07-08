#include "main.h"

#ifndef LSG_LIST_ITEM_H
#define LSG_LIST_ITEM_H

class LSG_ListItem : public LSG_Component
{
public:
	LSG_ListItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_ListItem() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
