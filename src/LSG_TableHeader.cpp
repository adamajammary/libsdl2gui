#include "LSG_TableHeader.h"

LSG_TableHeader::LSG_TableHeader(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableHeader::Render(SDL_Renderer* renderer)
{
}
