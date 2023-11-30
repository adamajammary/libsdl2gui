#include "LSG_ListItem.h"

LSG_ListItem::LSG_ListItem(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_ListItem::Render(SDL_Renderer* renderer)
{
}
