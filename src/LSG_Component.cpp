#include "LSG_Component.h"

LSG_Component::LSG_Component(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
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
	this->xmlDoc          = xmlDoc;
	this->xmlNode         = xmlNode;
	this->xmlNodeName     = xmlNodeName;

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

		if (child->xmlNodeName == "button")
			delete static_cast<LSG_Button*>(child);
		else if (child->IsImage())
			delete static_cast<LSG_Image*>(child);
		else if (child->IsList())
			delete static_cast<LSG_List*>(child);
		else if (child->IsMenu())
			delete static_cast<LSG_Menu*>(child);
		else if (child->IsSubMenu())
			delete static_cast<LSG_MenuSub*>(child);
		else if (child->IsMenuItem())
			delete static_cast<LSG_MenuItem*>(child);
		else if (child->IsSlider())
			delete static_cast<LSG_Slider*>(child);
		else if (child->IsTable())
			delete static_cast<LSG_Table*>(child);
		else if (child->IsTableGroup())
			delete static_cast<LSG_TableGroup*>(child);
		else if (child->IsTableHeader())
			delete static_cast<LSG_TableHeader*>(child);
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

LSG_Button* LSG_Component::GetButton()
{
	if (this->xmlNodeName == "button")
		return static_cast<LSG_Button*>(this);

	if (this->parent)
		return this->parent->GetButton();

	return nullptr;
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

	if (!bold.empty())
		style |= TTF_STYLE_BOLD;

	if (!italic.empty())
		style |= TTF_STYLE_ITALIC;

	if (!strikeThrough.empty())
		style |= TTF_STYLE_STRIKETHROUGH;

	if (!underline.empty())
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

int LSG_Component::GetLayer()
{
	return this->layer;
}

LSG_Component* LSG_Component::GetParent()
{
	return this->parent;
}

SDL_Rect LSG_Component::getRenderDestinationAligned(const SDL_Rect& backgroundArea, const SDL_Size& size)
{
	SDL_Rect destination = {
		backgroundArea.x,
		backgroundArea.y,
		min(size.width,  backgroundArea.w),
		min(size.height, backgroundArea.h)
	};

	auto attributes = this->GetXmlAttributes();
	auto halign     = (attributes.contains("halign") ? attributes["halign"] : "");
	auto valign     = (attributes.contains("valign") ? attributes["valign"] : "");

	if (halign == "center")
		destination.x += ((backgroundArea.w - destination.w) / 2);
	else if (halign == "right")
		destination.x += (backgroundArea.w - destination.w);

	if (valign == "middle")
		destination.y += ((backgroundArea.h - destination.h) / 2);
	else if (valign == "bottom")
		destination.y += (backgroundArea.h - destination.h);

	return destination;
}

SDL_Size LSG_Component::GetTextureSize()
{
	return LSG_Graphics::GetTextureSize(this->texture);
}

TexturSizes LSG_Component::getTextureSizes(int spacing)
{
	if (this->textures.empty())
		return {};

	TexturSizes size = {};

	for (auto texture : this->textures)
	{
		SDL_Size textureSize = {};
		SDL_QueryTexture(texture, nullptr, nullptr, &textureSize.width, &textureSize.height);

		size.sizes.push_back({ textureSize.width, textureSize.height });

		size.totalSize.height = (textureSize.height > size.totalSize.height ? textureSize.height : size.totalSize.height);
		size.totalSize.width += textureSize.width;
	}

	if (!this->textures.empty() && (spacing > 0))
		size.totalSize.width += (spacing * (int)(this->textures.size() - 1));

	return size;
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
	auto xmlAttribColor = LSG_XML::GetAttribute(this->xmlNode, xmlAttribute.c_str());

	if (!xmlAttribColor.empty())
		return xmlAttribColor;

	auto color = LSG_UI::GetColorFromTheme(this->id, xmlAttribute);

	if (!color.empty())
		return color;

	if (recursive && this->parent)
		return this->parent->getXmlColor(xmlAttribute);

	return "";
}

LibXml::xmlNode* LSG_Component::GetXmlNode()
{
	return this->xmlNode;
}

std::string LSG_Component::GetXmlValue()
{
	return LSG_XML::GetValue(this->xmlNode, this->xmlDoc);
}

bool LSG_Component::HasCustomAlign()
{
	return ((this->xmlNodeName == "image") || (this->xmlNodeName == "text"));
}

bool LSG_Component::isButton()
{
	if (this->xmlNodeName == "button")
		return true;

	if (this->parent)
		return this->parent->isButton();

	return false;
}

bool LSG_Component::IsImage()
{
	return (this->xmlNodeName == "image");
}

bool LSG_Component::IsList()
{
	return (this->xmlNodeName == "list");
}

bool LSG_Component::IsMenu()
{
	return (this->xmlNodeName == "menu");
}

bool LSG_Component::IsMenuItem()
{
	return (this->xmlNodeName == "menu-item");
}

bool LSG_Component::IsScrollable()
{
	return (this->IsList() || this->IsTable() || this->IsTextLabel());
}

bool LSG_Component::IsSlider()
{
	return (this->xmlNodeName == "slider");
}

bool LSG_Component::IsSubMenu()
{
	return (this->xmlNodeName == "menu-sub");
}

bool LSG_Component::IsTable()
{
	return (this->xmlNodeName == "table");
}

bool LSG_Component::IsTableGroup()
{
	return (this->xmlNodeName == "table-group");
}

bool LSG_Component::IsTableHeader()
{
	return (this->xmlNodeName == "table-header");
}

bool LSG_Component::IsTableRow()
{
	return (this->xmlNodeName == "table-row");
}

bool LSG_Component::IsTextLabel()
{
	return (this->xmlNodeName == "text");
}

bool LSG_Component::IsVertical()
{
	return (this->orientation == LSG_VERTICAL);
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
		this->renderDisabledOverlay(renderer);
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

void LSG_Component::renderDisabledOverlay(SDL_Renderer* renderer)
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
	auto fillArea = this->getFillArea(this->background, this->border);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 32);

	SDL_RenderFillRect(renderer, &fillArea);
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

	this->backgroundColor = (!backgroundColor.empty() ? LSG_UI::ToSdlColor(backgroundColor) : LSG_DEFAULT_BACK_COLOR);
	this->borderColor     = (!borderColor.empty()     ? LSG_UI::ToSdlColor(borderColor)     : LSG_DEFAULT_BORDER_COLOR);
	this->textColor       = (!textColor.empty()       ? LSG_UI::ToSdlColor(textColor)       : LSG_DEFAULT_TEXT_COLOR);

	for (auto child : this->children)
		child->SetColors();
}

void LSG_Component::SetFontSize(int size)
{
	LSG_XML::SetAttribute(this->xmlNode, "font-size", std::to_string(size));
}

void LSG_Component::SetHeight(int height)
{
	LSG_XML::SetAttribute(this->xmlNode, "height", std::to_string(height));
}

void LSG_Component::setMinSizeSlider()
{
	if (this->xmlNodeName != "slider")
		return;

	if (this->orientation == LSG_VERTICAL)
		this->background.w = max(LSG_SLIDER_MIN_HEIGHT, this->background.w);
	else
		this->background.h = max(LSG_SLIDER_MIN_HEIGHT, this->background.h);
}

void LSG_Component::SetOrientation(LSG_Orientation orientation)
{
	this->orientation = (orientation == LSG_ORIENTATION_VERTICAL ? LSG_VERTICAL : LSG_HORIZONTAL);
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
	auto attributes  = LSG_XML::GetAttributes(this->xmlNode);
	auto width       = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height      = (attributes.contains("height") ? attributes["height"] : "");
	auto margin2x    = (this->margin * 2);

	if (this->parent->IsVertical())
		sizeX -= margin2x;
	else
		sizeY -= margin2x;

	if (width.empty() && (sizeX > 0) && (componentsX > 0) && (this->background.w < 1))
		this->background.w = (sizeX / componentsX);

	if (height.empty() && (sizeY > 0) && (componentsY > 0) && (this->background.h < 1))
		this->background.h = (sizeY / componentsY);

	this->setMinSizeSlider();
}

void LSG_Component::SetSizeFixed()
{
	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width   = (attributes.contains("width")   ? attributes["width"]   : "");
	auto height  = (attributes.contains("height")  ? attributes["height"]  : "");
	auto border  = (attributes.contains("border")  ? attributes["border"]  : "");

	if (!width.empty() && (width[width.length() - 1] != '%') && (width != "0"))
		this->background.w = std::atoi(width.c_str());

	if (!height.empty() && (height[height.length() - 1] != '%') && (height != "0"))
		this->background.h = std::atoi(height.c_str());

	this->setMinSizeSlider();

	if (!border.empty())
		this->border = std::atoi(border.c_str());

	if (this->HasCustomAlign())
		return;

	auto margin  = (attributes.contains("margin")  ? attributes["margin"]  : "");
	auto padding = (attributes.contains("padding") ? attributes["padding"] : "");

	if (!margin.empty())
		this->margin = std::atoi(margin.c_str());

	if (!padding.empty())
		this->padding = std::atoi(padding.c_str());
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
	auto width      = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height     = (attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] == '%'))
		this->background.w = (int)((double)parentBackground.w * std::atof(width.c_str()) * 0.01);

	if (!height.empty() && (height[height.length() - 1] == '%'))
		this->background.h = (int)((double)parentBackground.h * std::atof(height.c_str()) * 0.01);

	if (width == "height")
		this->background.w = this->background.h;

	if (height == "width")
		this->background.h = this->background.w;

	this->setMinSizeSlider();
}

void LSG_Component::SetSpacing(int spacing)
{
	LSG_XML::SetAttribute(this->xmlNode, "spacing", std::to_string(spacing));
}

void LSG_Component::SetWidth(int width)
{
	LSG_XML::SetAttribute(this->xmlNode, "width", std::to_string(width));
}
