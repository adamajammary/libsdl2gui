#include "main.h"

#ifndef LSG_TEXT_LABEL_H
#define LSG_TEXT_LABEL_H

class LSG_TextLabel : public LSG_ScrollBar, public LSG_Text
{
public:
	LSG_TextLabel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TextLabel() {}

public:
	SDL_Size     GetSize() const;
	SDL_Size     GetTextSize(const SDL_Size& maxSize) const;
	SDL_Size     GetTextureSize() const;
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	void         Render(SDL_Renderer* renderer);
	void         SetText(const std::string& text);
	void         SetText();

private:
	void render(SDL_Renderer* renderer);
};

#endif
