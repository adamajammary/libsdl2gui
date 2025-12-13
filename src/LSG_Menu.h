#include "main.h"

#ifndef LSG_MENU_H
#define LSG_MENU_H

class LSG_Menu : public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_Menu(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Menu();

public:
	static const int LayerOffset = 100000000;

private:
	static const int DefaultWidth   = 300;
	static const int DefaultPadding = 10;

private:
	bool           highlightedIconClose;
	bool           highlightedNavBack;
	bool           isOpen;
	SDL_Texture*   renderTarget;
	LSG_Component* subMenu;

public:
	void         Close();
	void         Highlight(const SDL_Point& mousePosition);
	bool         IsHighlightedIconClose() const;
	bool         IsHighlightedNavBack() const;
	bool         IsMouseOverIconOpen(const SDL_Point& mousePosition) const;
	bool         IsOpen() const;
	void         Navigate(LSG_Component* component);
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	void         Open();
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override {};
	void         Render(SDL_Renderer* renderer);
	void         SetMenu();

private:
	SDL_Rect              getIconClose(const SDL_Rect& menu) const;
	SDL_Rect              getIconOpen() const;
	SDL_Rect              getMenu(const SDL_Rect& window) const;
	std::vector<SDL_Rect> getMenuItems() const;
	SDL_Rect              getNavBackHighlight(const SDL_Rect& menu) const;
	int                   getTextureHeight(const SDL_Rect& background) const;
	bool                  isMouseOverIconClose(const SDL_Point& mousePosition) const;
	bool                  isMouseOverMenu(const SDL_Point&      mousePosition) const;
	bool                  isMouseOverNavBack(const SDL_Point&   mousePosition) const;
	void                  renderHeaderLine(SDL_Renderer* renderer, const SDL_Rect& menu);
	void                  renderIconClose(SDL_Renderer*  renderer, const SDL_Rect& menu);
	void                  renderIconOpen(SDL_Renderer* renderer);
	void                  renderMenu(SDL_Renderer*     renderer);
	void                  renderMenuContentToTexture(SDL_Renderer* renderer, int offsetY, const SDL_Size& textureSize);
	void                  renderNavBack(SDL_Renderer* renderer, const SDL_Rect& menu);
	void                  renderTitle(SDL_Renderer*   renderer, const SDL_Rect& menu);
	virtual void          sendEvent(LSG_EventType type) const override {};
	void                  setMenuOpened();
	void                  setMenuClosed();
};

#endif
