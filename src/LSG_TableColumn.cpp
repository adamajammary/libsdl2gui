#include "LSG_TableColumn.h"

LSG_TableColumn::LSG_TableColumn(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableColumn::Render(SDL_Renderer* renderer)
{
}
