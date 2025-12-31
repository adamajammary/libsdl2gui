#include "main.h"

#ifndef LSG_NAVIGATION_H
#define LSG_NAVIGATION_H

struct LSG_NavigationArrow
{
	SDL_Texture* back    = nullptr;
	SDL_Texture* end     = nullptr;
	SDL_Texture* forward = nullptr;
	SDL_Texture* home    = nullptr;
	int          size    = 0;
};

struct LSG_NavigationItems
{
	size_t perNavigation = 1;
	size_t total         = 0;
};

struct LSG_CanNavigate
{
	bool back    = false;
	bool forward = false;
};

class LSG_Navigation : public LSG_Text, public LSG_IEvent
{
public:
	LSG_Navigation(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Navigation();

private:
	static const int ArrowPadding = 10;

private:
	LSG_NavigationArrow arrow;
	LSG_CanNavigate     canNavigate;
	LSG_NavigationItems items;
	int                 position;
	SDL_Size            textSize;

public:
	size_t       GetItemsTotal() const;
	int          GetPosition() const;
	bool         IsMouseOverArrow(const SDL_Point& mousePosition) const;
	void         NavigateBack(const std::string& text);
	void         NavigateEnd(const std::string& text);
	void         NavigateForward(const std::string& text);
	void         NavigateHome(const std::string& text);
	void         NavigateTo(int position, const std::string& text);
	virtual void OnMouseClick(const SDL_Point& mousePosition) override;
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	virtual void Render(SDL_Renderer* renderer) const override;
	void         Set(size_t itemsTotal, size_t itemsPerNavigation);
	void         Set();

private:
	void            destroyTexture(SDL_Texture* &texture);
	virtual void    destroyTextures() override;
	SDL_Rect        getArrow(const SDL_Rect& destination, int padding) const;
	SDL_Rect        getArrowDestination(const SDL_Rect& fillArea, int padding) const;
	LSG_CanNavigate getCanNavigate() const;
	std::string     getText() const;
	void            navigate(int position, const std::string& text);
	void            render(SDL_Renderer* renderer) const;
	void            renderArrows(SDL_Renderer* renderer, const SDL_Rect& fillArea, int padding) const;
	void            renderText(SDL_Renderer* renderer, const SDL_Rect& fillArea, int padding) const;
	virtual void    sendEvent(LSG_EventType type) const override;
	void            set();
};

#endif
