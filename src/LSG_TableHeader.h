#include "main.h"

#ifndef LSG_TABLE_HEADER_H
#define LSG_TABLE_HEADER_H

class LSG_TableHeader : public LSG_Component
{
public:
	LSG_TableHeader(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TableHeader() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
