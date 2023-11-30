#include "LSG_TableRow.h"

LSG_TableRow::LSG_TableRow(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableRow::Render(SDL_Renderer* renderer)
{
}
