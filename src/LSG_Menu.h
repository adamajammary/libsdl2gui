#include "main.h"

#ifndef LSG_MENU_H
#define LSG_MENU_H

class LSG_Menu : public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_Menu(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Menu();

public:
	static const int LayerOffset = 1000000000;

private:
	static const int Padding = 10;
	static const int Width   = 300;

private:
	bool           highlightedIconClose;
	bool           highlightedNavBack;
	bool           isOpen;
	SDL_Texture*   renderTarget;
	LSG_Component* subMenu;

public:
	void         Close();
	void         Highlight(const SDL_Point& mousePosition);
	bool         IsMouseOverIconOpen(const SDL_Point& mousePosition);
	bool         IsOpen() const;
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetMenu();

private:
	SDL_Rect              getIconClose(const SDL_Rect& menu);
	SDL_Rect              getIconOpen();
	SDL_Rect              getMenu(const SDL_Rect& window);
	SDL_Rect              getMenuBorder(const SDL_Rect& menu) const;
	std::vector<SDL_Rect> getMenuItems();
	SDL_Rect              getNavBack(const SDL_Rect& menu);
	SDL_Rect              getNavBackHighlight(const SDL_Rect& menu);
	SDL_Rect              getNavTitle(const SDL_Rect& menu, const SDL_Rect& clip);
	SDL_Rect              getNavTitleClip(const SDL_Rect& menu);
	int                   getTextureHeight(const SDL_Rect& background);
	bool                  isMouseOverIconClose(const SDL_Point& mousePosition);
	bool                  isMouseOverMenu(const SDL_Point& mousePosition);
	bool                  isMouseOverNavBack(const SDL_Point& mousePosition);
	void                  navigate(LSG_Component* component);
	void                  open();
	void                  renderBorderLine(SDL_Renderer* renderer, const SDL_Rect& menu);
	void                  renderIconClose(SDL_Renderer* renderer, const SDL_Rect& menu);
	void                  renderIconOpen(SDL_Renderer* renderer);
	void                  renderMenu(SDL_Renderer* renderer);
	void                  renderMenuContentToTexture(SDL_Renderer* renderer, int offsetY, const SDL_Size& textureSize);
	void                  renderNavBack(SDL_Renderer* renderer, const SDL_Rect& menu);
	void                  renderTitle(SDL_Renderer* renderer, const SDL_Rect& menu);
	virtual void          sendEvent(LSG_EventType type) override;
	void                  setMenuOpened();
	void                  setMenuClosed();
};

#endif
