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
	static const int PaddingIcon   = 5;
	static const int PaddingIcon2x = 10;

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
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetMenuItem(const SDL_Rect& background);
	void         SetSelected(bool selected = true);

private:
	SDL_Texture* getIcon(const std::string& imageFile);
	int          getMaxHeightIcon() const;
	void         renderIcon(SDL_Renderer*     renderer);
	void         renderSelected(SDL_Renderer* renderer);
	void         renderText(SDL_Renderer*     renderer);
	virtual void sendEvent(LSG_EventType type) override;

};

#endif
