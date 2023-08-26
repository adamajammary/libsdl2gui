#include "LSG_UI.h"

LSG_UMapStrStr       LSG_UI::colorTheme        = {};
std::string          LSG_UI::colorThemeFile    = "";
LSG_UMapStrComponent LSG_UI::components        = {};
LSG_MapIntComponent  LSG_UI::componentsByLayer = {};
int                  LSG_UI::id                = 0;
LSG_Component*       LSG_UI::root              = nullptr;
LibXml::xmlNode*     LSG_UI::windowNode        = nullptr;
LibXml::xmlDoc*      LSG_UI::xmlDocument       = nullptr;

void LSG_UI::AddSubMenuItem(LSG_MenuSub* subMenu, const std::string& item, const std::string& itemId)
{
	if (!subMenu || item.empty())
		return;

	auto layer    = LSG_UI::id++;
	auto nodeName = "menu-item";
	auto nodeID   = (!itemId.empty() ? itemId : std::format("{}_{}", nodeName, layer));

	if (LSG_UI::components.contains(nodeID))
		throw std::invalid_argument(std::format("Duplicate XML ID '{}' already exists.", nodeID).c_str());

	layer += LSG_MENU_LAYER_OFFSET;

	auto xmlNode = LSG_XML::AddChildNode(subMenu->GetXmlNode(), LSG_UI::xmlDocument, nodeName, item);

	if (!xmlNode)
		return;

	LSG_XML::SetAttribute(xmlNode, "id", nodeID);

	auto component = new LSG_MenuItem(nodeID, layer, LSG_UI::xmlDocument, xmlNode, nodeName, subMenu);

	component->visible = false;

	LSG_UI::components[nodeID]       = component;
	LSG_UI::componentsByLayer[layer] = component;
}

void LSG_UI::Close()
{
	LSG_UI::colorTheme.clear();
	LSG_UI::components.clear();
	LSG_UI::componentsByLayer.clear();

	if (LSG_UI::root) {
		delete LSG_UI::root;
		LSG_UI::root = nullptr;
	}

	if (LSG_UI::xmlDocument) {
		LibXml::xmlFreeDoc(LSG_UI::xmlDocument);
		LSG_UI::xmlDocument = nullptr;
	}

	LibXml::xmlCleanupParser();
}

void LSG_UI::CloseSubMenu()
{
	if (LSG_UI::root)
		LSG_UI::SetSubMenuVisible(LSG_UI::root, false);
}

SDL_Color LSG_UI::GetBackgroundColor()
{
	return (LSG_UI::root ? LSG_UI::root->backgroundColor : LSG_DEFAULT_BACK_COLOR);
}

LSG_Button* LSG_UI::GetButton(const SDL_Point& mousePosition)
{
	auto component = LSG_UI::GetComponent(mousePosition);

	return (component ? component->GetButton() : nullptr);
}

std::string LSG_UI::GetColorFromTheme(const std::string& componentID, const std::string& colorAttribute)
{
	auto key   = std::format("{}.{}", componentID, colorAttribute);
	auto color = (LSG_UI::colorTheme.contains(key) ? LSG_UI::colorTheme[key] : "");

	return color;
}

std::string LSG_UI::GetColorTheme()
{
	return LSG_UI::colorThemeFile;
}

LSG_Component* LSG_UI::GetComponent(const std::string& id)
{
	if (LSG_UI::components.contains(id))
		return LSG_UI::components[id];

	return nullptr;
}

LSG_Component* LSG_UI::GetComponent(const SDL_Point& mousePosition)
{
	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		if (component->visible && SDL_PointInRect(&mousePosition, &component->background))
			return component;
	}

	return nullptr;
}

void LSG_UI::HighlightComponents(const SDL_Point& mousePosition)
{
	bool isHighlightedMenu = false;

	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		component->highlighted = false;

		if (!isHighlightedMenu && component->visible && SDL_PointInRect(&mousePosition, &component->background))
			component->highlighted = true;

		if (component->highlighted && (component->IsSubMenu() || component->IsMenuItem()))
			isHighlightedMenu = true;
	}
}

bool LSG_UI::IsMenuOpen(LSG_Component* component)
{
	if (!component)
		return false;

	if (component->visible && (component->IsSubMenu() || component->IsMenuItem()))
		return true;

	bool isOpen = false;

	for (auto child : component->GetChildren())
	{
		if (LSG_UI::IsMenuOpen(child)) {
			isOpen = true;
			break;
		}
	}

	return isOpen;
}

/**
 * @throws invalid_argument
 */
void LSG_UI::Layout()
{
	if (!LSG_UI::root)
		throw std::invalid_argument("Root panel cannot be null.");

	LSG_UI::resetSize(LSG_UI::root);

	auto windowSize = LSG_Window::GetSize();

	LSG_UI::root->background = { 0, 0, windowSize.width, windowSize.height };

	LSG_UI::CloseSubMenu();

	LSG_UI::setImages(LSG_UI::root);
	LSG_UI::setListItems(LSG_UI::root);
	LSG_UI::setMenuItems(LSG_UI::root);
	LSG_UI::setTableRows(LSG_UI::root);
	LSG_UI::setTextLabels(LSG_UI::root);

	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);
}

void LSG_UI::layoutFixed(LSG_Component* component)
{
	if (!component)
		return;

	component->SetSizeFixed();

	for (auto child : component->GetChildren())
		LSG_UI::layoutFixed(child);
}

void LSG_UI::LayoutParent(LSG_Component* component)
{
	if (!component)
		return;

	LSG_UI::resetSize(component);

	auto parent = component->GetParent();

	if (parent)
	{
		LSG_UI::layoutFixed(parent);
		LSG_UI::layoutRelative(parent);

		return;
	}

	auto windowSize = LSG_Window::GetSize();

	LSG_UI::root->background = { 0, 0, windowSize.width, windowSize.height };

	LSG_UI::layoutFixed(component);
	LSG_UI::layoutRelative(component);
}

void LSG_UI::layoutRelative(LSG_Component* component)
{
	if (!component)
		return;

	auto children = component->GetChildren();

	for (auto child : children) {
		if (child->visible)
			child->SetSizePercent(component);
	}

	LSG_UI::layoutSizeBlank(component,     children);
	LSG_UI::layoutPositionAlign(component, children);

	for (auto child : children)
		LSG_UI::layoutRelative(child);
}

/**
 * @throws invalid_argument
 */
void LSG_UI::LayoutRoot()
{
	if (!LSG_UI::root)
		throw std::invalid_argument("Root panel cannot be null.");

	LSG_UI::resetSize(LSG_UI::root);

	auto windowSize = LSG_Window::GetSize();

	LSG_UI::root->background = { 0, 0, windowSize.width, windowSize.height };

	LSG_UI::CloseSubMenu();
	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);
}

void LSG_UI::layoutSizeBlank(LSG_Component* component, const LSG_Components& children)
{
	if (!component || children.empty())
		return;

	if (!component->GetParent())
	{
		auto rootMargin   = component->margin;
		auto rootMargin2x = (rootMargin * 2);

		component->background.x += rootMargin;
		component->background.y += rootMargin;
		component->background.w -= rootMargin2x;
		component->background.h -= rootMargin2x;
	}

	auto attributes  = component->GetXmlAttributes();
	auto spacing     = (attributes.contains("spacing") ? std::atoi(attributes["spacing"].c_str()) : 0);
	auto border2x    = (component->border  * 2);
	auto padding2x   = (component->padding * 2);
	auto sizeX       = (component->background.w - border2x - padding2x);
	auto sizeY       = (component->background.h - border2x - padding2x);
	auto componentsX = 0;
	auto componentsY = 0;

	for (size_t i = 0; i < children.size(); i++)
	{
		auto child = children[i];

		if (!child->visible)
			continue;

		auto childAttribs  = child->GetXmlAttributes();
		auto width         = (childAttribs.contains("width")  ? childAttribs["width"]  : "");
		auto height        = (childAttribs.contains("height") ? childAttribs["height"] : "");
		auto childBorder2x = (child->border * 2);
		auto childMargin2x = (child->margin * 2);
		auto textureSize   = child->GetTextureSize();
		auto textureHeight = (textureSize.height + childBorder2x);
		auto textureWidth  = (textureSize.width  + childBorder2x);
		bool addSpacing    = (i > 0 && children.size() > 1);


		// VERTICAL
		if (component->IsVertical())
		{
			componentsX = 1;

			if (height.empty())
			{
				if (child->HasCustomAlign() && (textureHeight < sizeY))
				{
					child->background.h = textureHeight;

					if (textureWidth > sizeX)
						child->background.h += LSG_SCROLL_WIDTH;

					sizeY -= child->background.h;
				} else {
					componentsY++;
					sizeY -= childMargin2x;
				}
			} else {
				sizeY -= (child->background.h + childMargin2x);
			}

			if (addSpacing)
				sizeY -= spacing;
		}
		// HORIZONTAL
		else
		{
			componentsY = 1;

			if (width.empty())
			{
				if (child->HasCustomAlign() && (textureWidth < sizeX))
				{
					child->background.w = textureWidth;

					if (textureHeight > sizeY)
						child->background.w += LSG_SCROLL_WIDTH;

					sizeX -= child->background.w;
				} else {
					componentsX++;
					sizeX -= childMargin2x;
				}
			} else {
				sizeX -= (child->background.w + childMargin2x);
			}

			if (addSpacing)
				sizeX -= spacing;
		}
	}

	for (auto child : children) {
		if (child->visible)
			child->SetSizeBlank(sizeX, sizeY, componentsX, componentsY);
	}
}

void LSG_UI::layoutPositionAlign(LSG_Component* component, const LSG_Components& children)
{
	if (!component || children.empty())
		return;

	auto attributes = component->GetXmlAttributes();
	bool isVertical = component->IsVertical();
	auto halign     = (attributes.contains("halign") ? attributes["halign"] : "");
	auto valign     = (attributes.contains("valign") ? attributes["valign"] : "");
	auto spacing    = (attributes.contains("spacing") ? std::atoi(attributes["spacing"].c_str()) : 0);
	auto border     = component->border;
	auto border2x   = (border * 2);
	auto padding    = component->padding;
	auto padding2x  = (padding * 2);
	auto offsetX    = (component->background.x + border + padding);
	auto offsetY    = (component->background.y + border + padding);
	auto maxX       = (component->background.w - border2x - padding2x);
	auto maxY       = (component->background.h - border2x - padding2x);
	auto remainingX = maxX;
	auto remainingY = maxY;

	// TOP-LEFT ALIGN
	for (size_t i = 0; i < children.size(); i++)
	{
		auto child = children[i];

		if (!child->visible)
			continue;

		auto childMargin   = child->margin;
		auto childMargin2x = (childMargin * 2);

		bool addSpacing     = (i > 0 && children.size() > 1);
		auto childSpacingX  = (addSpacing && !isVertical ? spacing : 0);
		auto childSpacingY  = (addSpacing && isVertical  ? spacing : 0);

		child->SetPositionAlign(offsetX + childMargin + childSpacingX, offsetY + childMargin + childSpacingY);

		if (isVertical) {
			offsetY    += (child->background.h + childMargin2x + childSpacingY);
			remainingY -= (child->background.h + childMargin2x + childSpacingY);
		} else {
			offsetX    += (child->background.w + childMargin2x + childSpacingX);
			remainingX -= (child->background.w + childMargin2x + childSpacingX);
		}
	}

	// ALIGN
	for (auto child : children)
	{
		if (!child->visible)
			continue;

		auto childOffsetX  = child->background.x;
		auto childOffsetY  = child->background.y;
		auto childMargin2x = (child->margin * 2);

		// VERTICAL
		if (isVertical)
		{
			// V-ALIGN
			if (valign == "middle")
				childOffsetY += (remainingY / 2);
			else if (valign == "bottom")
				childOffsetY += remainingY;

			// H-ALIGN
			if (halign == "center")
				childOffsetX += ((maxX - child->background.w - childMargin2x) / 2);
			else if (halign == "right")
				childOffsetX += (maxX - child->background.w - childMargin2x);
		}
		// HORIZONTAL
		else
		{
			// H-ALIGN
			if (halign == "center")
				childOffsetX += (remainingX / 2);
			else if (halign == "right")
				childOffsetX += remainingX;

			// V-ALIGN
			if (valign == "middle")
				childOffsetY += ((maxY - child->background.h - childMargin2x) / 2);
			else if (valign == "bottom")
				childOffsetY += (maxY - child->background.h - childMargin2x);
		}

		child->SetPositionAlign(childOffsetX, childOffsetY);
	}
}

void LSG_UI::Load(const std::string& colorThemeFile)
{
	LSG_UI::loadXmlNodes(LSG_UI::windowNode, nullptr, LSG_UI::xmlDocument, LSG_UI::root, LSG_UI::components);
	LSG_UI::SetColorTheme(colorThemeFile);
	LSG_UI::Layout();
}

/**
 * @throws invalid_argument
 */
void LSG_UI::loadXmlNodes(LibXml::xmlNode* parentNode, LSG_Component* parent, LibXml::xmlDoc* xmlDoc, LSG_Component* &root, LSG_UMapStrComponent& components)
{
	if (!parentNode)
		throw std::invalid_argument("Parent node cannot be null.");

	auto xmlNodes = LSG_XML::GetChildNodes(parentNode);

	for (auto xmlNode : xmlNodes)
	{
		if (xmlNode->type != LibXml::XML_ELEMENT_NODE)
			continue;

		auto id       = LSG_XML::GetAttribute(xmlNode, "id");
		auto layer    = LSG_UI::id++;
		auto nodeName = std::string(reinterpret_cast<const char*>(xmlNode->name));
		auto nodeID   = (!id.empty() ? id : std::format("{}_{}", nodeName, layer));

		if (components.contains(nodeID))
			throw std::invalid_argument(std::format("Duplicate XML ID '{}' already exists.", nodeID).c_str());

		LSG_Component* component = nullptr;

		if ((nodeName == "panel") || (nodeName == "button"))
		{
			if (nodeName == "panel")
			{
				component = new LSG_Component(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);

				if (!root)
					root = component;
			} else if (nodeName == "button") {
				component = new LSG_Button(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			}

			components[nodeID] = component;

			LSG_UI::componentsByLayer[layer] = component;

			LSG_UI::loadXmlNodes(xmlNode, component, xmlDoc, root, components);
		}
		else if (nodeName.starts_with("menu"))
		{
			layer += LSG_MENU_LAYER_OFFSET;

			if (nodeName == "menu")
				component = new LSG_Menu(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "menu-sub")
				component = new LSG_MenuSub(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "menu-item")
				component = new LSG_MenuItem(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);

			components[nodeID] = component;

			LSG_UI::componentsByLayer[layer] = component;

			if (nodeName != "menu-item")
				LSG_UI::loadXmlNodes(xmlNode, component, xmlDoc, root, components);
		}
		else if (nodeName.starts_with("list"))
		{
			if (nodeName == "list")
				component = new LSG_List(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "list-item")
				component = new LSG_ListItem(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);

			components[nodeID] = component;

			if (nodeName == "list")
			{
				LSG_UI::componentsByLayer[layer] = component;

				LSG_UI::loadXmlNodes(xmlNode, component, xmlDoc, root, components);
			}
		}
		else if (nodeName.starts_with("table"))
		{
			if (nodeName == "table")
				component = new LSG_Table(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "table-group")
				component = new LSG_TableGroup(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "table-header")
				component = new LSG_TableHeader(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "table-row")
				component = new LSG_TableRow(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "table-column")
				component = new LSG_TableColumn(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);

			components[nodeID] = component;

			if (nodeName == "table")
				LSG_UI::componentsByLayer[layer] = component;

			if (nodeName != "table-column")
				LSG_UI::loadXmlNodes(xmlNode, component, xmlDoc, root, components);
		}
		else if ((nodeName == "image") || (nodeName == "slider") || (nodeName == "text"))
		{
			if (nodeName == "image")
				component = new LSG_Image(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "slider")
				component = new LSG_Slider(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);
			else if (nodeName == "text")
				component = new LSG_TextLabel(nodeID, layer, xmlDoc, xmlNode, nodeName, parent);

			components[nodeID] = component;

			LSG_UI::componentsByLayer[layer] = component;
		}
	}
}

/**
 * @throws exception
 */
LSG_UMapStrStr LSG_UI::OpenWindow(const std::string& xmlFile)
{
	auto filePath = LSG_Text::GetFullPath(xmlFile);

	LSG_UI::xmlDocument = LSG_XML::Open(filePath.c_str());

	if (!LSG_UI::xmlDocument)
		throw std::exception(std::format("Failed to load XML file: {}", filePath).c_str());

	LSG_UI::windowNode = LSG_XML::GetNode("/window", LSG_UI::xmlDocument);

	if (!LSG_UI::windowNode)
		throw std::exception(std::format("Failed to find path '/window' in XML file: {}", filePath).c_str());

	auto windowAttribs = LSG_XML::GetAttributes(LSG_UI::windowNode);

	return windowAttribs;
}

void LSG_UI::Present(SDL_Renderer* renderer)
{
	LSG_UI::renderMenu(renderer, LSG_UI::root);

	SDL_RenderPresent(renderer);
}

void LSG_UI::RemoveMenuItem(LSG_MenuItem* menuItem)
{
	if (!menuItem)
		return;

	LSG_XML::RemoveNode(menuItem->GetXmlNode());

	auto id     = menuItem->GetID();
	auto layer  = menuItem->GetLayer();
	auto parent = menuItem->GetParent();

	if (parent)
		parent->RemoveChild(menuItem);

	LSG_UI::components.erase(id);
	LSG_UI::componentsByLayer.erase(layer);
}

void LSG_UI::Render(SDL_Renderer* renderer)
{
	if (LSG_UI::root)
		LSG_UI::root->Render(renderer);
}

void LSG_UI::renderMenu(SDL_Renderer* renderer, LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsMenu() || component->IsSubMenu() || component->IsMenuItem())
		component->Render(renderer);

	for (auto child : component->GetChildren())
		LSG_UI::renderMenu(renderer, child);
}

void LSG_UI::resetSize(LSG_Component* component)
{
	if (!component)
		return;

	component->background = {};

	for (auto child : component->GetChildren())
		LSG_UI::resetSize(child);
}

void LSG_UI::SetColorTheme(const std::string& colorThemeFile)
{
	if (!colorThemeFile.empty() && (colorThemeFile == LSG_UI::colorThemeFile))
		return;

	LSG_UI::colorTheme.clear();

	if (!colorThemeFile.empty())
	{
		auto filePath = LSG_Text::GetFullPath(colorThemeFile);
		auto file     = std::ifstream(filePath);

		if (!file.is_open())
			throw std::exception(std::format("Failed to open Color Theme file: {}", filePath).c_str());

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#' || line[0] == ';')
				continue;

			auto key   = std::strtok((char*)line.c_str(), "=");
			auto value = std::strtok(nullptr, "");

			LSG_UI::colorTheme[key] = value;
		}

		file.close();
	}

	LSG_UI::colorThemeFile = colorThemeFile;

	if (LSG_UI::root)
	{
		LSG_UI::root->SetColors();

		LSG_UI::setListItems(LSG_UI::root);
		LSG_UI::setMenuItems(LSG_UI::root);
		LSG_UI::setTableRows(LSG_UI::root);
		LSG_UI::setTextLabels(LSG_UI::root);
	}
}

void LSG_UI::SetEnabled(LSG_Component* component, bool enabled)
{
	if (!component)
		return;

	component->enabled = enabled;

	for (auto child : component->GetChildren())
		LSG_UI::SetEnabled(child, enabled);
}

void LSG_UI::setImages(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsImage())
		static_cast<LSG_Image*>(component)->SetImage();

	for (auto child : component->GetChildren())
		LSG_UI::setImages(child);
}

void LSG_UI::setListItems(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsList())
		static_cast<LSG_List*>(component)->SetItems();

	for (auto child : component->GetChildren())
		LSG_UI::setListItems(child);
}

void LSG_UI::setMenuItems(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsMenu())
		static_cast<LSG_Menu*>(component)->SetItems();

	for (auto child : component->GetChildren())
		LSG_UI::setMenuItems(child);
}

void LSG_UI::SetSubMenuVisible(LSG_Component* component, bool visible)
{
	if (!component)
		return;

	if (!visible)
	{
		if (component->IsMenu())
			static_cast<LSG_Menu*>(component)->Close();
		else if (component->IsSubMenu())
			static_cast<LSG_MenuSub*>(component)->Close();
	}

	if (component->IsSubMenu() || component->IsMenuItem())
		component->visible = visible;

	for (auto child : component->GetChildren())
	{
		if (visible)
			child->visible = true;
		else
			LSG_UI::SetSubMenuVisible(child, false);
	}
}

void LSG_UI::setTableRows(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsTable())
		static_cast<LSG_Table*>(component)->SetRows();

	for (auto child : component->GetChildren())
		LSG_UI::setTableRows(child);
}

void LSG_UI::setTextLabels(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->SetText();

	for (auto child : component->GetChildren())
		LSG_UI::setTextLabels(child);
}

void LSG_UI::SetText(LSG_Component* component)
{
	LSG_UI::setListItems(component);
	LSG_UI::setMenuItems(component);
	LSG_UI::setTableRows(component);
	LSG_UI::setTextLabels(component);
}

/**
* Valid color strings:
* "#00FF00"           0 red, 255 green, 0 blue
* "#00FF0080"         0 red, 255 green, 0 blue, 128 alpha
* "rgb(0,0,255)"      0 red, 255 green, 0 blue
* "rgba(0,0,255,0.5)" 0 red, 255 green, 0 blue, 128 alpha
*/
SDL_Color LSG_UI::ToSdlColor(const std::string &color)
{
	if (color.empty())
		return {};

	SDL_Color sdlColor = { 0, 0, 0, 255 };

	// HEX: "#00FF0080" / "#00FF00"
	if ((color[0] == '#') && (color.size() >= 7))
	{
		sdlColor.r = (uint8_t)std::strtoul(std::format("0x{}", color.substr(1, 2)).c_str(), nullptr, 16);
		sdlColor.g = (uint8_t)std::strtoul(std::format("0x{}", color.substr(3, 2)).c_str(), nullptr, 16);
		sdlColor.b = (uint8_t)std::strtoul(std::format("0x{}", color.substr(5, 2)).c_str(), nullptr, 16);

		if (color.size() == 9)
			sdlColor.a = (uint8_t)std::strtoul(std::format("0x{}", color.substr(7, 2)).c_str(), nullptr, 16);
	}
	else if (color.substr(0, 4) == "rgb(")
	{
		auto values = color.substr(4, color.size() - 5);

		sdlColor.r = (uint8_t)std::atoi(std::strtok((char*)values.c_str(), ","));
		sdlColor.g = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.b = (uint8_t)std::atoi(std::strtok(nullptr, ","));
	}
	else if (color.substr(0, 5) == "rgba(")
	{
		auto values = color.substr(5, color.size() - 6);

		sdlColor.r = (uint8_t)std::atoi(std::strtok((char*)values.c_str(), ","));
		sdlColor.g = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.b = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.a = (uint8_t)(256.0 * std::atof(std::strtok(nullptr, ",")));
	}

	return sdlColor;
}
