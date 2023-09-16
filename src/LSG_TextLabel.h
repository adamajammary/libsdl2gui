#include "main.h"

#ifndef LSG_TEXT_LABEL_H
#define LSG_TEXT_LABEL_H

class LSG_TextLabel : public LSG_Text
{
public:
	LSG_TextLabel(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TextLabel() {}

public:
	std::string  GetText();
	virtual void Render(SDL_Renderer* renderer) override;

};

#endif
