#include "main.h"

#ifndef LSG_TEXT_LABEL_H
#define LSG_TEXT_LABEL_H

class LSG_TextLabel : public LSG_ScrollBar, public LSG_Text
{
public:
	LSG_TextLabel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TextLabel() {}

public:
	SDL_Size     GetSize();
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	void         Set(const std::string& text);
	void         Set();
	void         setTexture();

private:
	void render(SDL_Renderer* renderer);
};

#endif
