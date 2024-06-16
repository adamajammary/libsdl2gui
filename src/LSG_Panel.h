#include "main.h"

#ifndef LSG_PANEL_H
#define LSG_PANEL_H

class LSG_Panel : public LSG_ScrollBar, public LSG_Component
{
public:
	LSG_Panel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Panel() {}

public:
	SDL_Size     GetTextureSize(const SDL_Rect& background);
	virtual void Render(SDL_Renderer* renderer) override;

private:
	void         renderContent(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& background, const SDL_Point& offset, const SDL_Size& maxSize);
	SDL_Texture* renderContentToTexture(SDL_Renderer* renderer, const SDL_Point& offset, const SDL_Size& size);
	void         renderScroll(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize);
};

#endif
