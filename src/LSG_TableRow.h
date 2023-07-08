#include "main.h"

#ifndef LSG_TABLE_ROW_H
#define LSG_TABLE_ROW_H

class LSG_TableRow : public LSG_Component
{
public:
	LSG_TableRow(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TableRow() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
