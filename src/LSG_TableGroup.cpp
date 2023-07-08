#include "LSG_TableGroup.h"

LSG_TableGroup::LSG_TableGroup(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableGroup::Render(SDL_Renderer* renderer)
{
}
