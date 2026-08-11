#include "LSG_Component.h"

LSG_Component::LSG_Component(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
{
	this->background      = {};
	this->backgroundColor = {};
	this->borderColor     = {};
	this->borderRadius    = 0;
	this->borderWidth     = 0;
	this->highlighted     = false;
	this->id              = id;
	this->layer           = layer;
	this->margin          = 0;
	this->padding         = 0;
	this->parent          = parent;
	this->textColor       = {};
	this->texture         = nullptr;
	this->tooltip         = "";
	this->xmlNode         = xmlNode;
	this->xmlNodeName     = xmlNodeName;

	this->text = LSG_XML::GetValue(this->xmlNode);

	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	this->orientation = (xmlAttributes.contains("orientation") ? xmlAttributes["orientation"] : "");
	this->tooltip     = (xmlAttributes.contains("tooltip")     ? xmlAttributes["tooltip"] : "");

	this->enabled = (!xmlAttributes.contains("enabled") || xmlAttributes["enabled"] == "true");
	this->visible = (!xmlAttributes.contains("visible") || xmlAttributes["visible"] == "true");

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
		else if (child->IsCards())
			delete static_cast<LSG_Cards*>(child);
		else if (child->IsImage())
			delete static_cast<LSG_Image*>(child);
		else if (child->IsLine())
			delete static_cast<LSG_Line*>(child);
		else if (child->IsList())
			delete static_cast<LSG_List*>(child);
		else if (child->IsMenu())
			delete static_cast<LSG_Menu*>(child);
		else if (child->IsMenuItem())
			delete static_cast<LSG_MenuItem*>(child);
		else if (child->IsSubMenu())
			delete static_cast<LSG_MenuSub*>(child);
		else if (child->IsModal())
			delete static_cast<LSG_Modal*>(child);
		else if (child->IsNavigation())
			delete static_cast<LSG_Navigation*>(child);
		else if (child->IsPanel())
			delete static_cast<LSG_Panel*>(child);
		else if (child->IsProgressBar())
			delete static_cast<LSG_ProgressBar*>(child);
		else if (child->IsSlider())
			delete static_cast<LSG_Slider*>(child);
		else if (child->IsTable())
			delete static_cast<LSG_Table*>(child);
		else if (child->IsTextInput())
			delete static_cast<LSG_TextInput*>(child);
		else if (child->IsTextLabel())
			delete static_cast<LSG_TextLabel*>(child);
		else if (child->IsTiles())
			delete static_cast<LSG_Tiles*>(child);
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

	for (auto texture : this->textures) {
		if (texture)
			SDL_DestroyTexture(texture);
	}

	this->textures.clear();
}

LSG_Alignment LSG_Component::getAlignment() const
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

LSG_Component* LSG_Component::GetChild(int index) const
{
	if (!this->children.empty() && (index >= 0) && (index < (int)this->children.size()))
		return this->children[index];

	return nullptr;
}

size_t LSG_Component::GetChildCount() const
{
	return this->children.size();
}

LSG_Components LSG_Component::GetChildren()
{
	return this->children;
}

SDL_Rect LSG_Component::getFillArea() const
{
	return LSG_Graphics::GetFillArea(this->background, this->borderWidth);
}

int LSG_Component::getFontSize() const
{
	auto xmlFontSize = LSG_XML::GetAttribute(this->xmlNode, "font-size");

	if (!xmlFontSize.empty())
		return std::atoi(xmlFontSize.c_str());

	if (this->parent)
		return this->parent->getFontSize();

	return LSG_DEFAULT_FONT_SIZE;
}

int LSG_Component::GetFontStyle() const
{
	return this->getFontStyle(false);
}

int LSG_Component::getFontStyle(bool checkParent) const
{
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto bold          = (xmlAttributes.contains("bold")           ? xmlAttributes["bold"] : "");
	auto italic        = (xmlAttributes.contains("italic")         ? xmlAttributes["italic"] : "");
	auto strikeThrough = (xmlAttributes.contains("strike-through") ? xmlAttributes["strike-through"] : "");
	auto underline     = (xmlAttributes.contains("underline")      ? xmlAttributes["underline"] : "");

	int style = TTF_STYLE_NORMAL;

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

	if (checkParent && this->parent)
		return this->parent->getFontStyle();

	return TTF_STYLE_NORMAL;
}

std::string LSG_Component::GetID() const
{
	return this->id;
}

int LSG_Component::GetLayer() const
{
	return this->layer;
}

LSG_Orientation LSG_Component::GetOrientation() const
{
	return LSG_ConstOrientation::ToEnum(this->orientation);
}

LSG_Component* LSG_Component::GetParent() const
{
	return this->parent;
}

LSG_Alignment LSG_Component::getParentAlignment() const
{
	if (this->parent)
		return this->parent->getAlignment();

	return { LSG_HALIGN_LEFT, LSG_VALIGN_TOP };
}

int LSG_Component::GetSpacing() const
{
	auto spacing = LSG_XML::GetAttribute(this->xmlNode, "spacing");

	return (!spacing.empty() ? std::atoi(spacing.c_str()) : 0);
}

int LSG_Component::getTextureHeight() const
{
	if (this->texture)
		return LSG_Graphics::GetTextureSize(this->texture).height;

	if (!this->textures.empty())
		return LSG_Graphics::GetTextureSize(this->textures[0]).height;

	return 0;
}

SDL_Size LSG_Component::getTextureSize() const
{
	if (!this->texture)
		return {};

	return LSG_Graphics::GetTextureSize(this->texture);
}

std::string LSG_Component::GetTooltip() const
{
	return this->tooltip;
}

std::string LSG_Component::GetXmlAttribute(const std::string& attribute) const
{
	return LSG_XML::GetAttribute(this->xmlNode, attribute);
}

LSG_UMapStrStr LSG_Component::GetXmlAttributes() const
{
	return LSG_XML::GetAttributes(this->xmlNode);
}

std::string LSG_Component::getXmlColor(const std::string& xmlAttribute, bool recursive) const
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

LibXml::xmlNode* LSG_Component::GetXmlNode() const
{
	return this->xmlNode;
}

LSG_Component* LSG_Component::GetScrollableParent()
{
	if (this->IsPanel() && static_cast<LSG_Panel*>(this)->IsScroll())
		return this;

	return (this->parent ? this->parent->GetScrollableParent() : nullptr);
}

bool LSG_Component::IsButton() const
{
	return (this->xmlNodeName == "button");
}

bool LSG_Component::IsCards() const
{
	return (this->xmlNodeName == "cards");
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

bool LSG_Component::IsMenu() const
{
	return (this->xmlNodeName == "menu");
}

bool LSG_Component::IsMenuItem() const
{
	return (this->xmlNodeName == "menu-item");
}

bool LSG_Component::IsModal() const
{
	return (this->xmlNodeName == "modal");
}

bool LSG_Component::IsNavigation() const
{
	return (this->xmlNodeName == "navigation");
}

bool LSG_Component::IsPanel() const
{
	return (this->xmlNodeName == "panel");
}

bool LSG_Component::IsProgressBar() const
{
	return (this->xmlNodeName == "progress-bar");
}

bool LSG_Component::IsScrollable() const
{
	return (this->IsCards() || this->IsList() || this->IsPanel() || this->IsTable() || this->IsTextLabel() || this->IsTiles());
}

bool LSG_Component::IsScrollablePanel(bool includeParents) const
{
	if (this->IsPanel() && (LSG_XML::GetAttribute(this->xmlNode, "scrollable") == "true"))
		return true;

	return (includeParents && this->parent ? this->parent->IsScrollablePanel(true) : false);
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

bool LSG_Component::IsTextInput() const
{
	return (this->xmlNodeName == "text-input");
}

bool LSG_Component::IsTextLabel() const
{
	return (this->xmlNodeName == "text");
}

bool LSG_Component::IsTiles() const
{
	return (this->xmlNodeName == "tiles");
}

bool LSG_Component::IsToggle() const
{
	return (this->xmlNodeName == "toggle");
}

bool LSG_Component::IsVertical() const
{
	return (this->orientation == LSG_ConstOrientation::Vertical);
}

bool LSG_Component::IsVisible(bool includeParents) const
{
	if (!this->visible)
		return false;

	return (includeParents && this->parent ? this->parent->IsVisible(true) : true);
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

	if (this->borderRadius > 0) {
		this->renderFillWithRoundedBorder(renderer, std::format("{}_background_fill_with_border", this->id));
	} else {
		this->renderFill(renderer);
		this->renderBorder(renderer);
	}

	for (auto child : this->children)
		child->Render(renderer);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_Component::renderBorder(SDL_Renderer* renderer) const
{
	if (this->borderWidth < 1)
		return;

	LSG_Graphics::RenderBorder(renderer, this->borderWidth, this->borderColor, this->background);
}

void LSG_Component::renderDisabled(SDL_Renderer* renderer) const
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer,     0, 0, 0, 64);

	LSG_Graphics::RenderFill(renderer, &this->background);
}

void LSG_Component::renderFill(SDL_Renderer* renderer) const
{
	LSG_Graphics::RenderFill(renderer, this->borderWidth, this->backgroundColor, this->background);
}

void LSG_Component::renderFillWithRoundedBorder(SDL_Renderer* renderer, const std::string& id) const
{
	LSG_Graphics::RenderFillWithRoundedBorder(
		renderer,
		this->backgroundColor,
		this->borderColor,
		this->borderRadius,
		this->borderWidth,
		this->background,
		id
	);
}

void LSG_Component::renderHighlight(SDL_Renderer* renderer) const
{
	this->renderHighlight(renderer, this->background, this->borderRadius);
}

void LSG_Component::renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background, int borderRadius) const
{
	auto highlightColor = LSG_Graphics::GetInverseColor(this->backgroundColor);

	highlightColor.a = 32;

	if (borderRadius > 0)
	{
		LSG_Graphics::RenderFillRounded(renderer, borderRadius, highlightColor, background, std::format("{}_highlighted_fill", this->id));
	}
	else
	{
		auto fillArea = LSG_Graphics::GetFillArea(background, this->borderWidth);

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, 32);

		LSG_Graphics::RenderFill(renderer, &fillArea);
	}
}

void LSG_Component::RenderTooltip(SDL_Renderer* renderer) const
{
	if (!this->visible)
		return;

	if (!this->highlighted) {
		LSG_Graphics::DestroyTextures(std::format("{}_tooltip", this->id));
		return;
	}

	if (!this->tooltip.empty())
		LSG_Graphics::RenderTooltip(renderer, this->tooltip, LSG_Window::GetMousePosition(), std::format("{}_tooltip", this->id));
}

void LSG_Component::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int)type;
	clickEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&clickEvent);
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

void LSG_Component::SetBackgroundColor(const SDL_Color& color)
{
	this->backgroundColor = color;

	LSG_XML::SetAttribute(this->GetXmlNode(), "background-color", LSG_Graphics::ToXmlAttribute(color));
}

void LSG_Component::SetBorderColor(const SDL_Color& color)
{
	this->borderColor = color;

	LSG_XML::SetAttribute(this->GetXmlNode(), "border-color", LSG_Graphics::ToXmlAttribute(color));
}

void LSG_Component::SetBorderRadius(int radius)
{
	this->borderRadius = LSG_Window::GetDPIScaled(radius);

	LSG_XML::SetAttribute(this->GetXmlNode(), "border-radius", std::to_string(radius));
}

void LSG_Component::SetBorderWidth(int width)
{
	this->borderWidth = LSG_Window::GetDPIScaled(width);

	LSG_XML::SetAttribute(this->GetXmlNode(), "border-width", std::to_string(width));
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

void LSG_Component::SetMargin(int margin)
{
	this->margin = LSG_Window::GetDPIScaled(margin);

	LSG_XML::SetAttribute(this->GetXmlNode(), "margin", std::to_string(margin));
}

void LSG_Component::SetOrientation(LSG_Orientation orientation)
{
	this->orientation = LSG_ConstOrientation::ToString(orientation);

	LSG_XML::SetAttribute(this->GetXmlNode(), "orientation", this->orientation);
}

void LSG_Component::SetPadding(int padding)
{
	this->padding = LSG_Window::GetDPIScaled(padding);

	LSG_XML::SetAttribute(this->GetXmlNode(), "padding", std::to_string(padding));
}

void LSG_Component::SetPositionAlign(int x, int y)
{
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto positionX = (xmlAttributes.contains("x") ? xmlAttributes["x"] : "");
	auto positionY = (xmlAttributes.contains("y") ? xmlAttributes["y"] : "");

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
	auto margin2x      = (this->margin * 2);
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (xmlAttributes.contains("width")  ? xmlAttributes["width"]  : "");
	auto height = (xmlAttributes.contains("height") ? xmlAttributes["height"] : "");

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
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (xmlAttributes.contains("width")  ? xmlAttributes["width"]  : "");
	auto height = (xmlAttributes.contains("height") ? xmlAttributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] != '%') && (width != "0"))
		this->background.w = LSG_Window::GetDPIScaled(std::atoi(width.c_str()));

	if (!height.empty() && (height[height.length() - 1] != '%') && (height != "0"))
		this->background.h = LSG_Window::GetDPIScaled(std::atoi(height.c_str()));

	auto borderRadius = (xmlAttributes.contains("border-radius") ? xmlAttributes["border-radius"] : "");
	auto borderWidth  = (xmlAttributes.contains("border-width")  ? xmlAttributes["border-width"]  : "");

	if (!borderRadius.empty())
		this->borderRadius = LSG_Window::GetDPIScaled(std::atoi(borderRadius.c_str()));

	if (!borderWidth.empty())
		this->borderWidth = LSG_Window::GetDPIScaled(std::atoi(borderWidth.c_str()));

	auto margin  = (xmlAttributes.contains("margin")  ? xmlAttributes["margin"]  : "");
	auto padding = (xmlAttributes.contains("padding") ? xmlAttributes["padding"] : "");

	if (!margin.empty())
		this->margin = LSG_Window::GetDPIScaled(std::atoi(margin.c_str()));

	if (!padding.empty())
		this->padding = LSG_Window::GetDPIScaled(std::atoi(padding.c_str()));
}

void LSG_Component::SetSizePercent(LSG_Component* parent)
{
	if (!parent)
		return;

	auto parentBackground = SDL_Rect(parent->background);

	if (parent->borderWidth > 0)
	{
		auto parentBorder2x = (parent->borderWidth + parent->borderWidth);

		parentBackground.w -= parentBorder2x;
		parentBackground.h -= parentBorder2x;
	}

	this->setSizePercent(parentBackground);
}

void LSG_Component::setSizePercent(const SDL_Rect& parentBackground)
{
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (xmlAttributes.contains("width")  ? xmlAttributes["width"]  : "");
	auto height = (xmlAttributes.contains("height") ? xmlAttributes["height"] : "");

	if (!width.empty() && width.ends_with('%'))
		this->background.w = (int)((double)parentBackground.w * std::atof(width.c_str()) * 0.01);

	if (!height.empty() && height.ends_with('%'))
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

void LSG_Component::SetTooltip(const std::string& tooltip)
{
	this->tooltip = tooltip;
}

void LSG_Component::SetVisible(bool visible)
{
	this->visible = visible;

	for (auto child : this->GetChildren())
		child->SetVisible(visible);
}
