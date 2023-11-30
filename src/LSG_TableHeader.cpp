#include "LSG_TableHeader.h"

LSG_TableHeader::LSG_TableHeader(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableHeader::Render(SDL_Renderer* renderer)
{
}
