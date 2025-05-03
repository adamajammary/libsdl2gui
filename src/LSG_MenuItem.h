#include "main.h"

#ifndef LSG_MENU_ITEM_H
#define LSG_MENU_ITEM_H

class LSG_MenuItem : public LSG_Text, public LSG_IEvent
{
public:
	LSG_MenuItem(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_MenuItem() {}

public:
	static const int Height = 32;

private:
	static const int PaddingIcon         = 14;
	static const int PaddingIconSelected = 5;

private:
	bool                 closed;
	LSG_ImageOrientation iconOrientation;
	bool                 selected;

public:
	void         Close();
	bool         IsClosed() const;
	bool         IsSelected() const;
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	void         Open();
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override {};
	virtual void Render(SDL_Renderer* renderer) const override;
	void         SetMenuItem(const SDL_Rect& background);
	void         SetSelected(bool selected = true);

protected:
	void renderText(SDL_Renderer* renderer, SDL_Texture* texture) const;

private:
	SDL_Texture* getIcon(const std::string& imageFile) const;
	int          getMaxHeightIcon() const;
	void         renderIcon(SDL_Renderer* renderer) const;
	void         renderKey(SDL_Renderer* renderer) const;
	void         renderSelected(SDL_Renderer* renderer) const;
	virtual void sendEvent(LSG_EventType type) const override;
};

#endif
