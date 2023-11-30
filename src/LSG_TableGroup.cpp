#include "LSG_TableGroup.h"

LSG_TableGroup::LSG_TableGroup(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_TableGroup::Render(SDL_Renderer* renderer)
{
}

void LSG_TableGroup::Sort(LSG_ComponentsCompare compare, LSG_SortOrder sortOrder)
{
	if (sortOrder == LSG_SORT_ORDER_DESCENDING)
		std::sort(this->children.rbegin(), this->children.rend(), compare);
	else
		std::sort(this->children.begin(), this->children.end(), compare);
}
