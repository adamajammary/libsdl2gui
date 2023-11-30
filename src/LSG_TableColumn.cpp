#include "LSG_TableColumn.h"

LSG_TableColumn::LSG_TableColumn(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableColumn::Render(SDL_Renderer* renderer)
{
}
