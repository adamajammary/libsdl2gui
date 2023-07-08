#include "main.h"

#ifndef LSG_COMPONENT_H
#define LSG_COMPONENT_H

struct TexturSizes
{
	std::vector<SDL_Size> sizes     = {};
	SDL_Size              totalSize = {};
};

class LSG_Button;

class LSG_Component
{
public:
	LSG_Component(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Component();

public:
	SDL_Rect  background;
	SDL_Color backgroundColor;
	int       border;
	SDL_Color borderColor;
	bool      enabled;
	int       fontSize;
	bool      highlighted;
	int       margin;
	int       padding;
	SDL_Color textColor;
	bool      visible;

protected:
	LSG_Components            children;
	int                       fontStyle;
	std::string               id;
	int                       layer;
	std::string               orientation;
	LSG_Component*            parent;
	SDL_Texture*              texture;
	std::vector<SDL_Texture*> textures;
	LibXml::xmlDoc*           xmlDoc;
	LibXml::xmlNode*          xmlNode;
	std::string               xmlNodeName;

public:
	LSG_Button*      GetButton();
	LSG_Components   GetChildren();
	std::string      GetID();
	LSG_Component*   GetParent();
	SDL_Size         GetTextureSize();
	std::string      GetXmlAttribute(const std::string& attribute);
	LSG_UMapStrStr   GetXmlAttributes();
	LibXml::xmlNode* GetXmlNode();
	std::string      GetXmlValue();
	bool             HasCustomAlign();
	bool             IsImage();
	bool             IsList();
	bool             IsMenu();
	bool             IsMenuItem();
	bool             IsScrollable();
	bool             IsSlider();
	bool             IsSubMenu();
	bool             IsTable();
	bool             IsTableGroup();
	bool             IsTableHeader();
	bool             IsTextLabel();
	bool             IsVertical();
	virtual void     Render(SDL_Renderer* renderer);
	void             SetAlignmentHorizontal(LSG_HAlign alignment);
	void             SetAlignmentVertical(LSG_VAlign   alignment);
	virtual void     SetColors();
	void             SetHeight(int height);
	void             SetOrientation(LSG_Orientation orientation);
	void             SetPositionAlign(int x, int y);
	void             SetSizeBlank(int sizeX, int sizeY, int componentsX, int componentsY);
	void             SetSizeFixed();
	void             SetSizePercent(LSG_Component* parent);
	void             SetSpacing(int spacing);
	void             SetWidth(int width);

protected:
	void         destroyTextures();
	SDL_Rect     getFillArea(const SDL_Rect& area, int border);
	int          getFontSize();
	int          getFontStyle();
	SDL_Rect     getRenderDestinationAligned(const SDL_Rect& backgroundArea, const SDL_Size& size);
	SDL_Size     getTextureSize(SDL_Texture* texture);
	TexturSizes  getTextureSizes(int spacing = 0);
	std::string  getXmlColor(const std::string& xmlAttribute, bool recursive = true);
	bool         isButton();
	void         renderBorder(SDL_Renderer* renderer, int border, const SDL_Color& borderColor, const SDL_Rect& background);
	virtual void renderDisabledOverlay(SDL_Renderer* renderer);
	void         renderFill(SDL_Renderer* renderer, int border, const SDL_Color& backgroundColor, const SDL_Rect& background);
	void         renderHighlight(SDL_Renderer* renderer);
	void         setMinSizeSlider();

private:
	void renderBorder(SDL_Renderer* renderer);
	void renderFill(SDL_Renderer* renderer);

};

#endif
