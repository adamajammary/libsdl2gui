#include "LSG_ListItem.h"

LSG_ListItem::LSG_ListItem(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_ListItem::Render(SDL_Renderer* renderer)
{
}
