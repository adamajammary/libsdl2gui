#include "main.h"

#ifndef LSG_TABLE_GROUP_H
#define LSG_TABLE_GROUP_H

class LSG_TableGroup : public LSG_Component
{
public:
	LSG_TableGroup(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TableGroup() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
