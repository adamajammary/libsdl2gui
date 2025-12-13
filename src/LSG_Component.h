#include "main.h"

#ifndef LSG_COMPONENT_H
#define LSG_COMPONENT_H

class LSG_Button;

class LSG_Component : public LSG_IRenderable
{
public:
	LSG_Component(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Component();

public:
	static const int LayerOffset = 1000000;

public:
	SDL_Rect    background;
	SDL_Color   backgroundColor;
	int         border;
	SDL_Color   borderColor;
	bool        enabled;
	bool        highlighted;
	int         margin;
	int         padding;
	std::string text;
	SDL_Color   textColor;
	bool        visible;

protected:
	LSG_Components            children;
	std::string               id;
	int                       layer;
	std::string               orientation;
	LSG_Component*            parent;
	SDL_Texture*              texture;
	std::vector<SDL_Texture*> textures;
	LibXml::xmlNode*          xmlNode;
	std::string               xmlNodeName;

public:
	size_t           GetChildCount() const;
	LSG_Component*   GetChild(int index) const;
	LSG_Components   GetChildren();
	int              GetFontStyle() const;
	std::string      GetID() const;
	int              GetLayer() const;
	LSG_Component*   GetParent() const;
	LSG_Component*   GetScrollableParent();
	int              GetSpacing() const;
	std::string      GetXmlAttribute(const std::string& attribute) const;
	LSG_UMapStrStr   GetXmlAttributes() const;
	LibXml::xmlNode* GetXmlNode() const;
	bool             IsButton() const;
	bool             IsCards() const;
	bool             IsImage() const;
	bool             IsLine() const;
	bool             IsList() const;
	bool             IsMenu() const;
	bool             IsMenuItem() const;
	bool             IsModal() const;
	bool             IsNavigation() const;
	bool             IsPanel() const;
	bool             IsProgressBar() const;
	bool             IsScrollable() const;
	bool             IsScrollablePanel(bool includeParents = false) const;
	bool             IsSlider() const;
	bool             IsSubMenu() const;
	bool             IsTable() const;
	bool             IsTextInput() const;
	bool             IsTextLabel() const;
	bool             IsTiles() const;
	bool             IsToggle() const;
	bool             IsVertical() const;
	void             RemoveChild(LSG_Component* child);
	virtual void     Render(SDL_Renderer* renderer) const override;
	void             SetAlignmentHorizontal(LSG_HAlign alignment);
	void             SetAlignmentVertical(LSG_VAlign alignment);
	void             SetBackgroundColor(const SDL_Color& color);
	void             SetBorder(int border);
	void             SetBorderColor(const SDL_Color& color);
	virtual void     SetColors();
	void             SetMargin(int margin);
	void             SetOrientation(LSG_Orientation orientation);
	void             SetPadding(int padding);
	void             SetPositionAlign(int x, int y);
	void             SetSizeBlank(int sizeX, int sizeY, int componentsX, int componentsY);
	void             SetSizeFixed();
	void             SetSizePercent(LSG_Component* parent);
	void             SetSpacing(int spacing);
	void             SetVisible(bool visible);

protected:
	virtual void  destroyTextures();
	LSG_Alignment getAlignment() const;
	int           getFontSize() const;
	int           getFontStyle(bool checkParent = true) const;
	LSG_Alignment getParentAlignment() const;
	int           getTextureHeight() const;
	SDL_Size      getTextureSize() const;
	std::string   getXmlColor(const std::string& xmlAttribute, bool recursive = true) const;
	void          renderDisabled(SDL_Renderer*  renderer) const;
	void          renderHighlight(SDL_Renderer* renderer) const;
	void          renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background) const;

private:
	void renderBorder(SDL_Renderer* renderer) const;
	void renderFill(SDL_Renderer* renderer) const;
	void setSizePercent(const SDL_Rect& parentBackground);
};

#endif
