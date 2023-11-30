#include "main.h"

#ifndef LSG_TABLE_COLUMN_H
#define LSG_TABLE_COLUMN_H

class LSG_TableColumn : public LSG_Component
{
public:
	LSG_TableColumn(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TableColumn() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
