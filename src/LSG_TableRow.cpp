#include "LSG_TableRow.h"

LSG_TableRow::LSG_TableRow(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableRow::Render(SDL_Renderer* renderer)
{
}
