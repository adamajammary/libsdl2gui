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
	int                       fontStyle;
	std::string               id;
	int                       layer;
	std::string               orientation;
	LSG_Component*            parent;
	SDL_Texture*              texture;
	std::vector<SDL_Texture*> textures;
	LibXml::xmlNode*          xmlNode;
	std::string               xmlNodeName;

public:
	size_t           GetChildCount();
	LSG_Component*   GetChild(int index);
	LSG_Components   GetChildren();
	std::string      GetID();
	int              GetLayer() const;
	LSG_Component*   GetParent();
	LSG_Component*   GetScrollableParent();
	int              GetSpacing();
	std::string      GetXmlAttribute(const std::string& attribute);
	LSG_UMapStrStr   GetXmlAttributes();
	LibXml::xmlNode* GetXmlNode();
	bool             IsButton() const;
	bool             IsImage() const;
	bool             IsLine() const;
	bool             IsList() const;
	bool             IsMenu() const;
	bool             IsMenuItem() const;
	bool             IsModal() const;
	bool             IsPanel() const;
	bool             IsProgressBar() const;
	bool             IsScrollable() const;
	bool             IsSlider() const;
	bool             IsSubMenu() const;
	bool             IsTable() const;
	bool             IsTextInput() const;
	bool             IsTextLabel() const;
	bool             IsVertical() const;
	void             RemoveChild(LSG_Component* child);
	virtual void     Render(SDL_Renderer* renderer) override;
	void             SetAlignmentHorizontal(LSG_HAlign alignment);
	void             SetAlignmentVertical(LSG_VAlign   alignment);
	virtual void     SetColors();
	void             SetOrientation(LSG_Orientation orientation);
	void             SetPositionAlign(int x, int y);
	void             SetSizeBlank(int sizeX, int sizeY, int componentsX, int componentsY);
	void             SetSizeFixed();
	void             SetSizePercent(LSG_Component* parent);
	void             SetSpacing(int spacing);
	void             SetVisible(bool visible);

protected:
	virtual void  destroyTextures();
	LSG_Alignment getAlignment();
	SDL_Rect      getArea(const SDL_Rect& background);
	SDL_Rect      getFillArea(const SDL_Rect& background, int border);
	int           getFontSize();
	int           getFontStyle();
	int           getTextureHeight();
	SDL_Size      getTextureSize();
	std::string   getXmlColor(const std::string& xmlAttribute, bool recursive = true);
	void          renderBorder(SDL_Renderer*    renderer, int border, const SDL_Color& borderColor, const SDL_Rect& background);
	void          renderDisabled(SDL_Renderer*  renderer) const;
	void          renderFill(SDL_Renderer*      renderer, int border, const SDL_Color& backgroundColor, const SDL_Rect& background);
	void          renderHighlight(SDL_Renderer* renderer);
	void          renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background);
	void          renderTexture(SDL_Renderer*   renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size);

private:
	void renderBorder(SDL_Renderer* renderer);
	void renderFill(SDL_Renderer* renderer);
	void setSizePercent(const SDL_Rect& parentBackground);
};

#endif
