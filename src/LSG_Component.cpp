#include "LSG_Component.h"

LSG_Component::LSG_Component(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
{
	this->background      = {};
	this->backgroundColor = {};
	this->border          = 0;
	this->borderColor     = {};
	this->highlighted     = false;
	this->id              = id;
	this->layer           = layer;
	this->margin          = 0;
	this->padding         = 0;
	this->parent          = parent;
	this->textColor       = {};
	this->texture         = nullptr;
	this->visible         = true;
	this->xmlNode         = xmlNode;
	this->xmlNodeName     = xmlNodeName;

	this->text = LSG_XML::GetValue(this->xmlNode);

	auto attributes  = LSG_XML::GetAttributes(this->xmlNode);
	auto enabled     = (attributes.contains("enabled")     ? attributes["enabled"]     : "true");
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");

	this->enabled     = (enabled == "true");
	this->fontStyle   = this->getFontStyle();
	this->orientation = orientation;

	if (this->parent)
		this->parent->children.push_back(this);
}

LSG_Component::~LSG_Component()
{
	this->destroyTextures();

	for (auto child : this->children)
	{
		if (!child)
			continue;

		if (child->IsButton())
			delete static_cast<LSG_Button*>(child);
		else if (child->IsImage())
			delete static_cast<LSG_Image*>(child);
		else if (child->IsLine())
			delete static_cast<LSG_Line*>(child);
		else if (child->IsList())
			delete static_cast<LSG_List*>(child);
		else if (child->IsListItem())
			delete static_cast<LSG_ListItem*>(child);
		else if (child->IsMenu())
			delete static_cast<LSG_Menu*>(child);
		else if (child->IsMenuItem())
			delete static_cast<LSG_MenuItem*>(child);
		else if (child->IsSubMenu())
			delete static_cast<LSG_MenuSub*>(child);
		else if (child->IsSlider())
			delete static_cast<LSG_Slider*>(child);
		else if (child->IsTable())
			delete static_cast<LSG_Table*>(child);
		else if (child->IsTableColumn())
			delete static_cast<LSG_TableColumn*>(child);
		else if (child->IsTableGroup())
			delete static_cast<LSG_TableGroup*>(child);
		else if (child->IsTableHeader())
			delete static_cast<LSG_TableHeader*>(child);
		else if (child->IsTableRow())
			delete static_cast<LSG_TableRow*>(child);
		else if (child->IsTextLabel())
			delete static_cast<LSG_TextLabel*>(child);
		else
			delete child;
	}

	this->children.clear();
}

void LSG_Component::destroyTextures()
{
	if (this->texture) {
		SDL_DestroyTexture(this->texture);
		this->texture = nullptr;
	}

	for (auto texture : this->textures)
	{
		if (texture) {
			SDL_DestroyTexture(texture);
			this->texture = nullptr;
		}
	}

	this->textures.clear();
}

LSG_Alignment LSG_Component::getAlignment()
{
	auto parentAttributes = (this->parent ? this->parent->GetXmlAttributes() : LSG_UMapStrStr());
	auto attributes       = this->GetXmlAttributes();

	auto halign = (attributes.contains("halign") ? attributes["halign"] : "");
	auto valign = (attributes.contains("valign") ? attributes["valign"] : "");

	if (halign.empty())
		halign = (parentAttributes.contains("halign") ? parentAttributes["halign"] : "");

	if (valign.empty())
		valign = (parentAttributes.contains("valign") ? parentAttributes["valign"] : "");

	LSG_Alignment alignment = { LSG_HALIGN_LEFT, LSG_VALIGN_TOP };

	if (halign == "center")
		alignment.halign = LSG_HALIGN_CENTER;
	else if (halign == "right")
		alignment.halign = LSG_HALIGN_RIGHT;

	if (valign == "middle")
		alignment.valign = LSG_VALIGN_MIDDLE;
	else if (valign == "bottom")
		alignment.valign = LSG_VALIGN_BOTTOM;

	return alignment;
}

SDL_Rect LSG_Component::getArea()
{
	SDL_Rect area = this->background;

	if (this->border > 0)
	{
		auto border2x = (this->border * 2);

		area.x += this->border;
		area.y += this->border;
		area.w -= border2x;
		area.h -= border2x;
	}

	if (this->parent && (this->parent->padding > 0))
	{
		auto padding2x = (this->parent->padding * 2);

		area.x += this->parent->padding;
		area.y += this->parent->padding;
		area.w -= padding2x;
		area.h -= padding2x;
	}

	return area;
}

LSG_Component* LSG_Component::GetChild(int index)
{
	if (!this->children.empty() && (index >= 0) && (index < (int)this->children.size()))
		return this->children[index];

	return nullptr;
}

size_t LSG_Component::GetChildCount()
{
	return this->children.size();
}

LSG_Components LSG_Component::GetChildren()
{
	return this->children;
}

SDL_Rect LSG_Component::getFillArea(const SDL_Rect& area, int border)
{
	SDL_Rect fillArea = SDL_Rect(area);

	if (border > 0)
	{
		auto border2x = (border * 2);

		fillArea.x += border;
		fillArea.y += border;
		fillArea.w -= border2x;
		fillArea.h -= border2x;
	}

	return fillArea;
}

int LSG_Component::getFontSize()
{
	auto xmlFontSize = LSG_XML::GetAttribute(this->xmlNode, "font-size");

	if (!xmlFontSize.empty())
		return std::atoi(xmlFontSize.c_str());

	if (this->parent)
		return this->parent->getFontSize();

	return LSG_DEFAULT_FONT_SIZE;
}

int LSG_Component::getFontStyle()
{
	auto attributes    = LSG_XML::GetAttributes(this->xmlNode);
	auto bold          = (attributes.contains("bold") ? attributes["bold"] : "");
	auto italic        = (attributes.contains("italic") ? attributes["italic"] : "");
	auto strikeThrough = (attributes.contains("strike-through") ? attributes["strike-through"] : "");
	auto underline     = (attributes.contains("underline") ? attributes["underline"] : "");
	int  style         = TTF_STYLE_NORMAL;

	if (bold == "true")
		style |= TTF_STYLE_BOLD;

	if (italic == "true")
		style |= TTF_STYLE_ITALIC;

	if (strikeThrough == "true")
		style |= TTF_STYLE_STRIKETHROUGH;

	if (underline == "true")
		style |= TTF_STYLE_UNDERLINE;

	if (style > TTF_STYLE_NORMAL)
		return style;

	if (this->parent)
		return this->parent->getFontStyle();

	return TTF_STYLE_NORMAL;
}

std::string LSG_Component::GetID()
{
	return this->id;
}

int LSG_Component::GetLayer() const
{
	return this->layer;
}

LSG_Component* LSG_Component::GetParent()
{
	return this->parent;
}

int LSG_Component::GetSpacing()
{
	auto spacing = LSG_XML::GetAttribute(this->xmlNode, "spacing");

	return (!spacing.empty() ? std::atoi(spacing.c_str()) : 0);
}

int LSG_Component::getTextureHeight()
{
	if (this->texture)
		return LSG_Graphics::GetTextureSize(this->texture).height;

	if (!this->textures.empty())
		return LSG_Graphics::GetTextureSize(this->textures[0]).height;

	return 0;
}

SDL_Size LSG_Component::getTextureSize()
{
	if (!this->texture)
		return {};

	return LSG_Graphics::GetTextureSize(this->texture);
}

std::string LSG_Component::GetXmlAttribute(const std::string& attribute)
{
	return LSG_XML::GetAttribute(this->xmlNode, attribute);
}

LSG_UMapStrStr LSG_Component::GetXmlAttributes()
{
	return LSG_XML::GetAttributes(this->xmlNode);
}

std::string LSG_Component::getXmlColor(const std::string& xmlAttribute, bool recursive)
{
	auto color = LSG_UI::GetColorFromTheme(this->id, xmlAttribute);

	if (!color.empty())
		return color;

	auto xmlAttribColor = LSG_XML::GetAttribute(this->xmlNode, xmlAttribute.c_str());

	if (!xmlAttribColor.empty())
		return xmlAttribColor;

	if (recursive && this->parent)
		return this->parent->getXmlColor(xmlAttribute);

	return "";
}

LibXml::xmlNode* LSG_Component::GetXmlNode()
{
	return this->xmlNode;
}

bool LSG_Component::IsButton() const
{
	return (this->xmlNodeName == "button");
}

bool LSG_Component::IsImage() const
{
	return (this->xmlNodeName == "image");
}

bool LSG_Component::IsLine() const
{
	return (this->xmlNodeName == "line");
}

bool LSG_Component::IsList() const
{
	return (this->xmlNodeName == "list");
}

bool LSG_Component::IsListItem() const
{
	return (this->xmlNodeName == "list-item");
}

bool LSG_Component::IsMenu() const
{
	return (this->xmlNodeName == "menu");
}

bool LSG_Component::IsMenuItem() const
{
	return (this->xmlNodeName == "menu-item");
}

bool LSG_Component::IsScrollable() const
{
	return (this->IsList() || this->IsTable() || this->IsTextLabel());
}

bool LSG_Component::IsSlider() const
{
	return (this->xmlNodeName == "slider");
}

bool LSG_Component::IsSubMenu() const
{
	return (this->xmlNodeName == "menu-sub");
}

bool LSG_Component::IsTable() const
{
	return (this->xmlNodeName == "table");
}

bool LSG_Component::IsTableColumn() const
{
	return (this->xmlNodeName == "table-column");
}

bool LSG_Component::IsTableGroup() const
{
	return (this->xmlNodeName == "table-group");
}

bool LSG_Component::IsTableHeader() const
{
	return (this->xmlNodeName == "table-header");
}

bool LSG_Component::IsTableRow() const
{
	return (this->xmlNodeName == "table-row");
}

bool LSG_Component::IsTextLabel() const
{
	return (this->xmlNodeName == "text");
}

bool LSG_Component::IsVertical() const
{
	return (this->orientation == LSG_ConstOrientation::Vertical);
}

void LSG_Component::RemoveChild(LSG_Component* child)
{
	if (!child)
		return;

	for (auto iter = this->children.begin(); iter != this->children.end(); iter++)
	{
		if ((*iter)->id != child->id)
			continue;

		this->children.erase(iter);
		delete child;

		break;
	}
}

void LSG_Component::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	this->renderFill(renderer);
	this->renderBorder(renderer);

	for (auto child : this->children) {
		if (!child->IsMenu())
			child->Render(renderer);
	}

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_Component::renderBorder(SDL_Renderer* renderer)
{
	if (this->border < 1)
		return;

	this->renderBorder(renderer, this->border, this->borderColor, this->background);
}

void LSG_Component::renderBorder(SDL_Renderer* renderer, int border, const SDL_Color& borderColor, const SDL_Rect& background)
{
	if (border < 1)
		return;

	SDL_SetRenderDrawBlendMode(renderer, (borderColor.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);

	SDL_Rect borderArea = SDL_Rect(background);

	// TOP
	borderArea.h = border;

	SDL_RenderFillRect(renderer, &borderArea);

	// BOTTOM
	borderArea.y += (background.h - border);

	SDL_RenderFillRect(renderer, &borderArea);

	// LEFT
	borderArea.y = background.y;
	borderArea.w = border;
	borderArea.h = background.h;

	SDL_RenderFillRect(renderer, &borderArea);

	// RIGHT
	borderArea.x += (background.w - border);

	SDL_RenderFillRect(renderer, &borderArea);
}

void LSG_Component::renderDisabled(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);

	SDL_RenderFillRect(renderer, &this->background);
}

void LSG_Component::renderFill(SDL_Renderer* renderer)
{
	this->renderFill(renderer, this->border, this->backgroundColor, this->background);
}

void LSG_Component::renderFill(SDL_Renderer* renderer, int border, const SDL_Color& backgroundColor, const SDL_Rect& background)
{
	auto fillArea = this->getFillArea(background, border);

	SDL_SetRenderDrawBlendMode(renderer, (backgroundColor.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	SDL_RenderFillRect(renderer, &fillArea);
}

void LSG_Component::renderHighlight(SDL_Renderer* renderer)
{
	this->renderHighlight(renderer, this->background);
}

void LSG_Component::renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background)
{
	auto fillArea = this->getFillArea(background, this->border);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, (255 - this->backgroundColor.r), (255 - this->backgroundColor.g), (255 - this->backgroundColor.b), 32);

	SDL_RenderFillRect(renderer, &fillArea);
}

void LSG_Component::renderTexture(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size)
{
	SDL_Rect clip = { 0, 0, std::min(size.width, background.w), std::min(size.height, background.h) };
	auto     dest = LSG_Graphics::GetDestinationAligned(background, size, alignment);

	SDL_RenderCopy(renderer, texture, &clip, &dest);
}

void LSG_Component::renderTextures(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& textures, int maxWidth, int spacing, SDL_Rect& clip, SDL_Rect& destination)
{
	auto offsetX        = clip.x;
	auto remainingWidth = maxWidth;

	for (auto texture : textures)
	{
		auto size = LSG_Graphics::GetTextureSize(texture);

		clip.x = std::max(0, offsetX);
		clip.w = std::max(0, std::min((size.width - clip.x), remainingWidth));

		destination.w = clip.w;
		destination.h = clip.h;

		SDL_RenderCopy(renderer, texture, &clip, &destination);

		auto width = (clip.w + spacing);

		destination.x  += width;
		remainingWidth -= width;
		offsetX        -= size.width;
	}
}

void LSG_Component::SetAlignmentHorizontal(LSG_HAlign alignment)
{
	std::string value = "left";

	switch (alignment) {
		case LSG_HALIGN_CENTER: value = "center"; break;
		case LSG_HALIGN_RIGHT:  value = "right"; break;
		default: break;
	}

	LSG_XML::SetAttribute(this->xmlNode, "halign", value);
}

void LSG_Component::SetAlignmentVertical(LSG_VAlign alignment)
{
	std::string value = "top";

	switch (alignment) {
		case LSG_VALIGN_MIDDLE: value = "middle"; break;
		case LSG_VALIGN_BOTTOM: value = "bottom"; break;
		default: break;
	}

	LSG_XML::SetAttribute(this->xmlNode, "valign", value);
}

void LSG_Component::SetColors()
{
	auto backgroundColor = this->getXmlColor("background-color");
	auto borderColor     = this->getXmlColor("border-color");
	auto textColor       = this->getXmlColor("text-color");

	this->backgroundColor = (!backgroundColor.empty() ? LSG_Graphics::ToSdlColor(backgroundColor) : LSG_ConstDefaultColor::Background);
	this->borderColor     = (!borderColor.empty()     ? LSG_Graphics::ToSdlColor(borderColor)     : LSG_ConstDefaultColor::Border);
	this->textColor       = (!textColor.empty()       ? LSG_Graphics::ToSdlColor(textColor)       : LSG_ConstDefaultColor::Text);

	for (auto child : this->children)
		child->SetColors();
}

void LSG_Component::SetOrientation(LSG_Orientation orientation)
{
	this->orientation = LSG_ConstOrientation::ToString(orientation);

	LSG_XML::SetAttribute(this->GetXmlNode(), "orientation", this->orientation);
}

void LSG_Component::SetPositionAlign(int x, int y)
{
	auto attributes = LSG_XML::GetAttributes(this->xmlNode);
	auto positionX  = (attributes.contains("x") ? attributes["x"] : "");
	auto positionY  = (attributes.contains("y") ? attributes["y"] : "");

	if (!positionX.empty())
		this->background.x = std::atoi(positionX.c_str());
	else
		this->background.x = x;

	if (!positionY.empty())
		this->background.y = std::atoi(positionY.c_str());
	else
		this->background.y = y;
}

/**
* @param sizeX       Remaining size horizontally
* @param sizeY       Remaining size vertically
* @param componentsX Remaining components horizontally
* @param componentsY Remaining components vertically
*/
void LSG_Component::SetSizeBlank(int sizeX, int sizeY, int componentsX, int componentsY)
{
	auto attributes = LSG_XML::GetAttributes(this->xmlNode);
	auto margin2x   = (this->margin * 2);

	auto width  = (!this->IsImage() && attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (!this->IsImage() && attributes.contains("height") ? attributes["height"] : "");

	if (this->parent->IsVertical())
		sizeX -= margin2x;
	else
		sizeY -= margin2x;

	if (width.empty() && (sizeX > 0) && (componentsX > 0) && (this->background.w < 1))
		this->background.w = (sizeX / componentsX);

	if (height.empty() && (sizeY > 0) && (componentsY > 0) && (this->background.h < 1))
		this->background.h = (sizeY / componentsY);
}

void LSG_Component::SetSizeFixed()
{
	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (!this->IsImage() && attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (!this->IsImage() && attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] != '%') && (width != "0"))
		this->background.w = LSG_Graphics::GetDPIScaled(std::atoi(width.c_str()));

	if (!height.empty() && (height[height.length() - 1] != '%') && (height != "0"))
		this->background.h = LSG_Graphics::GetDPIScaled(std::atoi(height.c_str()));

	auto border  = (attributes.contains("border")  ? attributes["border"]  : "");
	auto margin  = (attributes.contains("margin")  ? attributes["margin"]  : "");
	auto padding = (attributes.contains("padding") ? attributes["padding"] : "");

	if (!border.empty())
		this->border = LSG_Graphics::GetDPIScaled(std::atoi(border.c_str()));

	if (!margin.empty())
		this->margin = LSG_Graphics::GetDPIScaled(std::atoi(margin.c_str()));

	if (!padding.empty())
		this->padding = LSG_Graphics::GetDPIScaled(std::atoi(padding.c_str()));
}

void LSG_Component::SetSizePercent(LSG_Component* parent)
{
	if (!parent)
		return;

	auto parentBackground = SDL_Rect(parent->background);

	if (parent->border > 0)
	{
		auto parentBorder2x = (parent->border * 2);

		parentBackground.w -= parentBorder2x;
		parentBackground.h -= parentBorder2x;
	}

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] == '%'))
		this->background.w = (int)((double)parentBackground.w * std::atof(width.c_str()) * 0.01);

	if (!height.empty() && (height[height.length() - 1] == '%'))
		this->background.h = (int)((double)parentBackground.h * std::atof(height.c_str()) * 0.01);

	if (width == "height")
		this->background.w = this->background.h;

	if (height == "width")
		this->background.h = this->background.w;
}

void LSG_Component::SetSpacing(int spacing)
{
	LSG_XML::SetAttribute(this->xmlNode, "spacing", std::to_string(spacing));
}
