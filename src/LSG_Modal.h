#include "main.h"

#ifndef LSG_MODAL_H
#define LSG_MODAL_H

class LSG_Modal : public LSG_Text
{
public:
	LSG_Modal(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Modal() {}

public:
	static const int Height         = 52;
	static const int LayerOffsetMax = 2000000000;
	static const int LayerOffsetMin = 1500000000;

private:
	static const int CloseIconSize = 32;
	static const int Padding       = 10;
	static const int TitleFontSize = 18;

private:
	bool      hideCloseIcon;
	SDL_Point mousePosition;

public:
	static bool IsModalChild(LSG_Component* component);

public:
	bool         CloseOnMouseClick(const SDL_Point& mousePosition);
	void         Highlight(const SDL_Point& mousePosition);
	void         Layout();
	bool         OnKeyDown(const SDL_KeyboardEvent& event);
	virtual void Render(SDL_Renderer* renderer) override;
	void         Update();

private:
	SDL_Rect getCloseIcon();
	void     highlight(LSG_Component* component, const SDL_Point& mousePosition);
	void     renderHeader(SDL_Renderer* renderer);
	void     renderHeaderCloseIcon(SDL_Renderer* renderer);
	void     renderHeaderLine(SDL_Renderer* renderer);
	void     renderHeaderTitle(SDL_Renderer* renderer);
	void     setBorder(const LSG_UMapStrStr& attributes);
	void     setHideCloseIcon(const LSG_UMapStrStr& attributes);
	void     setMargin(const LSG_UMapStrStr& attributes);
	void     setPadding(const LSG_UMapStrStr& attributes);
	void     setPosition(const LSG_UMapStrStr& attributes, const SDL_Rect& parentBackground);
	void     setSize(const LSG_UMapStrStr& attributes, const SDL_Rect& parentBackground);
	void     setTextures(const std::string& title);
};

#endif
