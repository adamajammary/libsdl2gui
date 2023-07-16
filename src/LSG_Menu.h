#include "main.h"

#ifndef LSG_MENU_H
#define LSG_MENU_H

class LSG_Menu : public LSG_Text, public LSG_IEvent
{
public:
	LSG_Menu(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Menu() {}

private:
	int selectedMenu;

public:
	void         Close();
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetItems();

private:
	int                   getSelectedMenu(const SDL_Point& mousePosition, const std::vector<SDL_Rect>& menuAreas);
	std::vector<SDL_Rect> getTextureDestinations();
	bool                  open(const SDL_Point& mousePosition, const std::vector<SDL_Rect>& menuAreas, int index);
	void                  renderHighlightSelection(SDL_Renderer* renderer, const std::vector<SDL_Rect>& menuAreas, int index);
	virtual void          sendEvent(LSG_EventType type) override;

};

#endif
