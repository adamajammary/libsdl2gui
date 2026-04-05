#include "main.h"

#ifndef LSG_MODAL_H
#define LSG_MODAL_H

class LSG_Modal : public LSG_Text
{
public:
	LSG_Modal(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Modal() {}

public:
	static const int HeaderHeight = 52;
	static const int LayerOffset  = 1000000000;

private:
	static const int CloseIconSize = 32;
	static const int Padding       = 10;
	static const int TitleFontSize = 18;

private:
	LSG_UMapStrComponent components;
	LSG_MapIntComponent  componentsByLayer;
	bool                 hideCloseIcon;

public:
	void           Close();
	LSG_Component* GetComponent(const std::string& id);
	SDL_Cursor*    Highlight(const SDL_Point& mousePosition);
	void           OnKeyDown(const SDL_KeyboardEvent& event);
	void           OnMouseDown(const SDL_Event& event, const SDL_Point& mousePosition);
	void           OnMouseScroll(const SDL_MouseWheelEvent& event, const SDL_Point& mousePosition) const;
	void           OnMouseUp(const SDL_Event& event, const SDL_Point& mousePosition) const;
	void           Open();
	virtual void   Render(SDL_Renderer* renderer) override;
	void           Set();
	void           SetBackground();

private:
	LSG_Component* addNode(LibXml::xmlNode* node, LSG_Component* parent);
	void           addNodes(LibXml::xmlNode* parentNode, LSG_Component* parent);
	SDL_Rect       getCloseIcon() const;
	LSG_Component* getComponent(const SDL_Point& mousePosition) const;
	LSG_Component* getComponentInScrollablePanel(const SDL_Point& mousePosition, LSG_Component* component) const;
	int            getSizeFromXmlAttribute(const std::string& size, int maxSize) const;
	int            getSizeFromXmlAttribute(const std::string& maxSize, const std::string& minSize, int size) const;
	bool           isMouseOverCloseIcon(const SDL_Point& mousePosition) const;
	bool           isMouseOverHeader(const SDL_Point& mousePosition) const;
	void           renderBackdrop(SDL_Renderer* renderer) const;
	void           renderHeader(SDL_Renderer* renderer, int headerHeight) const;
	void           renderHeaderCloseIcon(SDL_Renderer* renderer) const;
	void           renderHeaderLine(SDL_Renderer* renderer, int headerHeight) const;
	void           renderHeaderTitle(SDL_Renderer* renderer, int headerHeight) const;
};

#endif
