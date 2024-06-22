#include "main.h"

#ifndef LSG_PANEL_H
#define LSG_PANEL_H

class LSG_Panel : public LSG_ScrollBar, public LSG_Component
{
public:
	LSG_Panel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Panel();

private:
	SDL_Texture* renderTarget;
	bool         scrollable;

public:
	SDL_Size     GetSize();
	bool         IsScroll() const;
	virtual void Render(SDL_Renderer* renderer) override;

private:
	SDL_Size getSize();
	void     renderContent(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize);
	void     renderContentToTexture(SDL_Renderer* renderer, const SDL_Size& maxSize);
	void     renderScroll(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Size& maxSize);
};

#endif
