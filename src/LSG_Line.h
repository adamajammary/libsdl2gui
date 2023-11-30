#include "main.h"

#ifndef LSG_LINE_H
#define LSG_LINE_H

class LSG_Line : public LSG_Component
{
public:
	LSG_Line(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Line() {}

public:
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
