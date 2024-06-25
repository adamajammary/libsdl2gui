#include "LSG_UI.h"

LSG_UMapStrStr       LSG_UI::colorTheme        = {};
std::string          LSG_UI::colorThemeFile    = "";
LSG_UMapStrComponent LSG_UI::components        = {};
LSG_MapIntComponent  LSG_UI::componentsByLayer = {};
int                  LSG_UI::id                = 0;
LSG_Component*       LSG_UI::root              = nullptr;
LibXml::xmlNode*     LSG_UI::windowNode        = nullptr;
LibXml::xmlDoc*      LSG_UI::xmlDocument       = nullptr;

/**
 * @throws invalid_argument
 */
LSG_Component* LSG_UI::AddXmlNode(LibXml::xmlNode* node, LSG_Component* parent)
{
	if (!node)
		throw std::invalid_argument("XML node cannot be null.");

	if (node->type != LibXml::XML_ELEMENT_NODE)
		return nullptr;

	auto xmlID = LSG_XML::GetAttribute(node, "id");
	auto layer = LSG_UI::id++;
	auto name  = std::string(reinterpret_cast<const char*>(node->name));

	if (name == "modal")
		layer += LSG_Modal::LayerOffsetMax;
	else if (name == "menu")
		layer += LSG_Menu::LayerOffset;
	else if (name == "button")
		layer += LSG_Button::LayerOffset;
	else if (name == "list")
		layer += LSG_List::LayerOffset;
	else if (name == "table")
		layer += LSG_Table::LayerOffset;

	if ((name != "modal") && LSG_Modal::IsModalChild(parent))
		layer += LSG_Modal::LayerOffsetMin;

	auto id = (!xmlID.empty() ? xmlID : LSG_Text::Format("%s_%d", name.c_str(), layer));

	if (LSG_UI::components.contains(id))
		throw std::invalid_argument(LSG_Text::Format("Duplicate XML ID '%s' already exists.", id.c_str()));

	LSG_Component* component = nullptr;

	if (name == "button")
		component = new LSG_Button(id, layer, node, name, parent);
	else if (name == "line")
		component = new LSG_Line(id, layer, node, name, parent);
	else if (name == "menu")
		component = new LSG_Menu(id, layer, node, name, parent);
	else if (name == "menu-item")
		component = new LSG_MenuItem(id, layer, node, name, parent);
	else if (name == "menu-sub")
		component = new LSG_MenuSub(id, layer, node, name, parent);
	else if (name == "modal")
		component = new LSG_Modal(id, layer, node, name, parent);
	else if (name == "list")
		component = new LSG_List(id, layer, node, name, parent);
	else if (name == "list-item")
		static_cast<LSG_List*>(parent)->AddItem(LSG_XML::GetValue(node));
	else if (name == "panel")
		component = new LSG_Panel(id, layer, node, name, parent);
	else if (name == "table")
		component = new LSG_Table(id, layer, node, name, parent);
	else if (name == "table-group")
		static_cast<LSG_Table*>(parent)->AddGroup(node);
	else if (name == "table-header")
		static_cast<LSG_Table*>(parent)->SetHeader(node);
	else if (name == "table-row")
		static_cast<LSG_Table*>(parent)->AddRow(node);
	else if (name == "image")
		component = new LSG_Image(id, layer, node, name, parent);
	else if (name == "progress-bar")
		component = new LSG_ProgressBar(id, layer, node, name, parent);
	else if (name == "slider")
		component = new LSG_Slider(id, layer, node, name, parent);
	else if (name == "text")
		component = new LSG_TextLabel(id, layer, node, name, parent);

	if (!component)
		return nullptr;

	if (layer >= LSG_Modal::LayerOffsetMin)
		component->visible = false;

	if (!LSG_UI::root && (name == "panel"))
		LSG_UI::root = component;

	LSG_UI::components[id]           = component;
	LSG_UI::componentsByLayer[layer] = component;

	return component;
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

SDL_Point LSG_UI::GetAlignedPosition(const SDL_Point& position, const LSG_UMapStrStr& attributes, const LSG_UmapStrSize& sizes, int size, const SDL_Size& maxSize, LSG_Component* component, LSG_Component* parent)
{
	if (attributes.empty() || !component || !sizes.contains(component->GetID()) || !parent)
		return {};

	auto orientation = (attributes.contains("orientation") ? attributes.at("orientation") : "");
	auto halign      = (attributes.contains("halign") ? attributes.at("halign") : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes.at("spacing").c_str())) : 0);
	auto valign      = (attributes.contains("valign") ? attributes.at("valign") : "");

	auto border2x      = (parent->border  + parent->border);
	auto childMargin4x = (component->margin * 4);
	bool isVertical    = (orientation == "vertical");
	auto padding2x     = (parent->padding + parent->padding);
	auto totalSpacing  = (spacing * ((int)sizes.size() - 1));

	SDL_Point alignedPosition = position;

	if (isVertical)
	{
		auto remainingHeight = std::max((maxSize.height - size - totalSpacing - padding2x - border2x - childMargin4x), 0);

		if (valign == "middle")
			alignedPosition.y += (remainingHeight / 2);
		else if (valign == "bottom")
			alignedPosition.y += remainingHeight;

		auto remainingWidth = std::max((maxSize.width - sizes.at(component->GetID()).width - padding2x - border2x), 0);

		if (halign == "center")
			alignedPosition.x += (remainingWidth / 2);
		else if (halign == "right")
			alignedPosition.x += (remainingWidth - component->margin);
		else
			alignedPosition.x += component->margin;
	}
	else
	{
		auto remainingWidth = std::max((maxSize.width - size - totalSpacing - padding2x - border2x - childMargin4x), 0);

		if (halign == "center")
			alignedPosition.x += (remainingWidth / 2);
		else if (halign == "right")
			alignedPosition.x += remainingWidth;

		auto remainingHeight = std::max((maxSize.height  - sizes.at(component->GetID()).height - padding2x - border2x), 0);

		if (valign == "middle")
			alignedPosition.y += (remainingHeight / 2);
		else if (valign == "bottom")
			alignedPosition.y += (remainingHeight - component->margin);
		else
			alignedPosition.y += component->margin;
	}

	return alignedPosition;
}

SDL_Rect LSG_UI::GetBackgroundArea()
{
	auto windowSize = LSG_Window::GetSize();

	#if defined _ios
		UIWindow* window = [UIApplication sharedApplication].windows.firstObject;
		auto      scale  = LSG_Window::GetSizeScale();

		auto top    = (int)(window.safeAreaInsets.top    * scale.y);
		auto bottom = (int)(window.safeAreaInsets.bottom * scale.y);
		auto left   = (int)(window.safeAreaInsets.left   * scale.x);
		auto right  = (int)(window.safeAreaInsets.right  * scale.x);

		SDL_Rect background = {
			left,
			top,
			(windowSize.width - left - right),
			(windowSize.height - top - bottom)
		};
	#else
		SDL_Rect background = { 0, 0, windowSize.width, windowSize.height };
	#endif

	return background;
}

std::string LSG_UI::GetColorFromTheme(const std::string& componentID, const std::string& colorAttribute)
{
	auto key   = LSG_Text::Format("%s.%s", componentID.c_str(), colorAttribute.c_str());
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

LSG_Component* LSG_UI::GetComponent(const SDL_Point& mousePosition, bool skipModalChildren)
{
	int modalLayer = 0;

	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		if (!component->visible)
			continue;

		if (component->IsModal())
		{
			if (skipModalChildren)
				return component;

			if (static_cast<LSG_Modal*>(component)->CloseOnMouseClick(mousePosition))
				return nullptr;

			modalLayer = LSG_Modal::LayerOffsetMin;
			continue;
		}

		if (component->IsMenu() && static_cast<LSG_Menu*>(component)->IsOpen())
			return component;

		if ((component->IsMenuItem() || component->IsSubMenu()) && static_cast<LSG_MenuItem*>(component)->IsClosed())
			continue;

		auto background = LSG_UI::GetScrolledBackground(component);

		if ((component->GetLayer() > modalLayer) && SDL_PointInRect(&mousePosition, &background))
			return component;
	}

	return nullptr;
}

SDL_Rect LSG_UI::GetScrolledBackground(LSG_Component* component)
{
	if (!component)
		return { -1, -1 };

	auto scrollableParent = component->GetScrollableParent();

	if (!scrollableParent || (component->GetID() == scrollableParent->GetID()))
		return component->background;

	auto panel = static_cast<LSG_Panel*>(scrollableParent);

	SDL_Rect scrolledBackground = {
		(component->background.x + panel->background.x + panel->margin - panel->padding - panel->GetScrollX()),
		(component->background.y + panel->background.y + panel->margin - panel->padding - panel->GetScrollY()),
		component->background.w,
		component->background.h
	};

	if (!SDL_HasIntersection(&scrolledBackground, &panel->background))
		return { -1, -1 };

	return scrolledBackground;
}

SDL_Point LSG_UI::GetScrolledPosition(const SDL_Point& mousePosition, LSG_Component* component)
{
	if (!component)
		return { -1, -1 };

	auto scrollableParent = component->GetScrollableParent();

	if (!scrollableParent || (component->GetID() == scrollableParent->GetID()))
		return mousePosition;

	auto panel = static_cast<LSG_Panel*>(scrollableParent);

	SDL_Point scrolledPosition = {
		(mousePosition.x - panel->background.x - panel->margin + panel->padding + panel->GetScrollX()),
		(mousePosition.y - panel->background.y - panel->margin + panel->padding + panel->GetScrollY())
	};

	return scrolledPosition;
}

LibXml::xmlDoc* LSG_UI::GetXmlDocument()
{
	return LSG_UI::xmlDocument;
}

void LSG_UI::HighlightComponents(const SDL_Point& mousePosition)
{
	if (!SDL_GetCursor())
		return;

	bool isMenuOpen  = false;
	int  menuScrollY = 0;

	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		component->highlighted = false;

		if (!component->visible || !component->enabled)
			continue;

		if (component->IsModal()) {
			static_cast<LSG_Modal*>(component)->Highlight(mousePosition);
			return;
		}

		if (component->IsMenu())
		{
			auto menu = static_cast<LSG_Menu*>(component);

			if (menu->IsOpen()) {
				isMenuOpen  = true;
				menuScrollY = menu->GetScrollY();
				break;
			} else {
				component->highlighted = menu->IsMouseOverIconOpen(mousePosition);
			}

			continue;
		}

		auto background = LSG_UI::GetScrolledBackground(component);

		component->highlighted = SDL_PointInRect(&mousePosition, &background);
	}

	if (!isMenuOpen)
		return;

	for (auto& component : LSG_UI::componentsByLayer)
	{
		auto background = SDL_Rect(component.second->background);
		background.y   -= menuScrollY;

		if (component.second->IsMenu())
			static_cast<LSG_Menu*>(component.second)->Highlight(mousePosition);
		else if (component.second->IsMenuItem() || component.second->IsSubMenu())
			component.second->highlighted = SDL_PointInRect(&mousePosition, &background);
		else
			component.second->highlighted = false;
	}
}

/**
 * @throws runtime_error
 */
bool LSG_UI::IsDarkMode()
{
#if defined _android
	bool isDarkMode     = false;
	auto jniEnvironment = LSG_AndroidJNI::GetEnvironment();
	auto jniActivity    = LSG_AndroidJNI::GetClass(LSG_ConstAndroid::ActivityClassPath, jniEnvironment);
	auto jniIsDarkMode  = jniEnvironment->GetStaticMethodID(jniActivity, "IsDarkMode", "()Z");

	if (jniIsDarkMode)
		isDarkMode = jniEnvironment->CallStaticBooleanMethod(jniActivity, jniIsDarkMode);

	jniEnvironment->DeleteLocalRef(jniActivity);

	return isDarkMode;
#elif defined _ios
	UIUserInterfaceStyle style = [[[UIScreen mainScreen] traitCollection] userInterfaceStyle];

	return (style == UIUserInterfaceStyleDark);
#elif defined _linux
	GSettings* settings = g_settings_new("org.gnome.desktop.interface");

	if (!settings)
		return false;

	gchar* colorScheme = g_settings_get_string(settings, "color-scheme");
	gchar* gtkTheme    = g_settings_get_string(settings, "gtk-theme");
	auto   scheme      = std::string(colorScheme ? colorScheme : "");
	auto   theme       = std::string(gtkTheme    ? gtkTheme    : "");

	if (colorScheme)
		g_free(colorScheme);

	if (gtkTheme)
		g_free(gtkTheme);

	return ((scheme.find("dark") != std::string::npos) || (theme.find("dark") != std::string::npos));
#elif defined _macosx
	NSString* name  = [[NSApp effectiveAppearance] bestMatchFromAppearancesWithNames: @[NSAppearanceNameAqua, NSAppearanceNameDarkAqua]];
	NSString* style = [[NSUserDefaults standardUserDefaults] stringForKey: @"AppleInterfaceStyle"];

	return ((name == NSAppearanceNameDarkAqua) || [style isEqualToString: @"Dark"]);
#elif defined _windows
	int appsUseLightTheme    = -1;
	int systemUsesLightTheme = -1;

	DWORD size = sizeof(int);
	auto  path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

	RegGetValueA(HKEY_CURRENT_USER, path, "AppsUseLightTheme",    RRF_RT_REG_DWORD, nullptr, &appsUseLightTheme,    &size);
	RegGetValueA(HKEY_CURRENT_USER, path, "SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &systemUsesLightTheme, &size);

	return ((appsUseLightTheme == 0) || (systemUsesLightTheme == 0));
#endif
}

/**
 * @throws invalid_argument
 */
void LSG_UI::Layout()
{
	if (!LSG_UI::root)
		throw std::invalid_argument("Root panel cannot be null.");

	LSG_UI::resetSize(LSG_UI::root);

	LSG_UI::root->background = LSG_UI::GetBackgroundArea();

	LSG_UI::setImages(LSG_UI::root);
	LSG_UI::setListItems(LSG_UI::root);
	LSG_UI::setTableRows(LSG_UI::root);
	LSG_UI::setTextLabels(LSG_UI::root);

	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);
	LSG_UI::layoutModal(LSG_UI::root);

	LSG_UI::setMenu(LSG_UI::root);
}

void LSG_UI::layoutFixed(LSG_Component* component, bool skipModal)
{
	if (!component || (skipModal && component->IsModal()))
		return;

	component->SetSizeFixed();

	for (auto child : component->GetChildren())
		LSG_UI::layoutFixed(child, skipModal);
}

void LSG_UI::layoutModal(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsModal())
	{
		static_cast<LSG_Modal*>(component)->Layout();

		LSG_UI::setImages(component);
		LSG_UI::setListItems(component);
		LSG_UI::setTableRows(component);
		LSG_UI::setTextLabels(component);

		auto height = LSG_Graphics::GetDPIScaled(LSG_Modal::Height);

		component->background.y += height;
		component->background.h -= height;

		LSG_UI::layoutFixed(component,    false);
		LSG_UI::layoutRelative(component, false);

		component->background.y -= height;
		component->background.h += height;

		return;
	}

	for (auto child : component->GetChildren())
		LSG_UI::layoutModal(child);
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
		LSG_UI::layoutModal(parent);

		return;
	}

	LSG_UI::root->background = LSG_UI::GetBackgroundArea();

	LSG_UI::layoutFixed(component);
	LSG_UI::layoutRelative(component);
	LSG_UI::layoutModal(component);
}

void LSG_UI::layoutPositionAlign(LSG_Component* component, const LSG_Components& children)
{
	if (!component || children.empty())
		return;

	auto attributes = component->GetXmlAttributes();
	bool isVertical = component->IsVertical();
	auto halign     = (attributes.contains("halign") ? attributes["halign"] : "");
	auto valign     = (attributes.contains("valign") ? attributes["valign"] : "");
	auto scrollable = (attributes.contains("scrollable") ? attributes["scrollable"] : "");
	auto spacing    = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);
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

		if (!child->visible || child->IsModal())
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
		if (!child->visible || child->IsModal())
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
void LSG_UI::layoutRelative(LSG_Component* component, bool skipModal)
{
	if (!component || (skipModal && component->IsModal()))
		return;

	auto children = component->GetChildren();

	for (auto child : children) {
		if (child->visible && (!skipModal || !child->IsModal()))
			child->SetSizePercent(component);
	}

	LSG_UI::layoutSizeBlank(component,     children);
	LSG_UI::layoutPositionAlign(component, children);

	for (auto child : children)
		LSG_UI::layoutRelative(child, skipModal);
}

/**
 * @throws invalid_argument
 */
void LSG_UI::LayoutRoot()
{
	if (!LSG_UI::root)
		throw std::invalid_argument("Root panel cannot be null.");

	LSG_UI::resetSize(LSG_UI::root);

	LSG_UI::root->background = LSG_UI::GetBackgroundArea();

	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);
	LSG_UI::layoutModal(LSG_UI::root);
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
	auto scrollable  = (attributes.contains("scrollable") ? attributes["scrollable"] : "");
	auto spacing     = (attributes.contains("spacing") ? LSG_Graphics::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);
	auto border2x    = (component->border  * 2);
	auto padding2x   = (component->padding * 2);
	auto sizeX       = (component->background.w - border2x - padding2x);
	auto sizeY       = (component->background.h - border2x - padding2x);
	auto componentsX = 0;
	auto componentsY = 0;

	for (size_t i = 0; i < children.size(); i++)
	{
		auto child = children[i];

		if (!child->visible || child->IsModal())
			continue;

		auto childAttribs  = child->GetXmlAttributes();
		auto width         = (childAttribs.contains("width")  ? childAttribs["width"]  : "");
		auto height        = (childAttribs.contains("height") ? childAttribs["height"] : "");
		auto childMargin2x = (child->margin * 2);
		bool addSpacing    = (i > 0 && children.size() > 1);

		// VERTICAL
		if (component->IsVertical())
		{
			componentsX = 1;

			if (height.empty() && child->IsTextLabel())
			{
				auto textSize = static_cast<LSG_TextLabel*>(child)->GetTextSize({ sizeX, sizeY });

				sizeY              -= (textSize.height + childMargin2x);
				child->background.h = LSG_Graphics::GetDPIScaled(textSize.height);

				if (width.empty())
					child->background.w = LSG_Graphics::GetDPIScaled(textSize.width);
			}
			else if (!height.empty() && child->IsImage())
			{
				auto imageSize = static_cast<LSG_Image*>(child)->GetImageSize();

				sizeY              -= (imageSize.height + childMargin2x);
				child->background.h = LSG_Graphics::GetDPIScaled(imageSize.height);

				if (!width.empty())
					child->background.w = LSG_Graphics::GetDPIScaled(imageSize.width);
			}
			else if (height.empty() || child->IsImage())
			{
				componentsY++;
				sizeY -= childMargin2x;
			}
			else
			{
				sizeY -= (child->background.h + childMargin2x);
			}

			if (addSpacing)
				sizeY -= spacing;
		}
		// HORIZONTAL
		else
		{
			componentsY = 1;

			if (width.empty() && child->IsTextLabel())
			{
				auto textSize = static_cast<LSG_TextLabel*>(child)->GetTextSize({ sizeX, sizeY });

				sizeX              -= (textSize.width + childMargin2x);
				child->background.w = LSG_Graphics::GetDPIScaled(textSize.width);

				if (height.empty())
					child->background.h = LSG_Graphics::GetDPIScaled(textSize.height);
			}
			else if (!width.empty() && child->IsImage())
			{
				auto imageSize = static_cast<LSG_Image*>(child)->GetImageSize();

				sizeX              -= (imageSize.width + childMargin2x);
				child->background.w = LSG_Graphics::GetDPIScaled(imageSize.width);

				if (!height.empty())
					child->background.h = LSG_Graphics::GetDPIScaled(imageSize.height);
			}
			else if (width.empty() || child->IsImage())
			{
				componentsX++;
				sizeX -= childMargin2x;
			}
			else
			{
				sizeX -= (child->background.w + childMargin2x);
			}

			if (addSpacing)
				sizeX -= spacing;
		}
	}

	for (auto child : children) {
		if (child->visible && !child->IsModal())
			child->SetSizeBlank(sizeX, sizeY, componentsX, componentsY);
	}
}

void LSG_UI::Load(const std::string& colorThemeFile)
{
	LSG_UI::loadXmlNodes(LSG_UI::windowNode, nullptr);
	LSG_UI::SetColorTheme(colorThemeFile, true);
	LSG_UI::Layout();
}

/**
 * @throws invalid_argument
 */
void LSG_UI::loadXmlNodes(LibXml::xmlNode* parentNode, LSG_Component* parent)
{
	if (!parentNode)
		throw std::invalid_argument("Parent node cannot be null.");

	auto xmlNodes = LSG_XML::GetChildNodes(parentNode);

	for (auto xmlNode : xmlNodes)
	{
		auto component = LSG_UI::AddXmlNode(xmlNode, parent);

		if (component)
			LSG_UI::loadXmlNodes(xmlNode, component);
	}
}

/**
 * @throws runtime_error
 */
LSG_UMapStrStr LSG_UI::OpenWindow(const std::string& xmlFile)
{
	auto filePath = LSG_Text::GetFullPath(xmlFile);

	LSG_UI::xmlDocument = LSG_XML::Open(filePath);

	if (!LSG_UI::xmlDocument)
		throw std::runtime_error(LSG_Text::Format("Failed to load XML file: %s", filePath.c_str()));

	LSG_UI::windowNode = LSG_XML::GetNode("/window");

	if (!LSG_UI::windowNode)
		throw std::runtime_error(LSG_Text::Format("Failed to find path '/window' in XML file: %s", filePath.c_str()));

	auto windowAttribs = LSG_XML::GetAttributes(LSG_UI::windowNode);

	return windowAttribs;
}

void LSG_UI::Present(SDL_Renderer* renderer)
{
	LSG_UI::renderMenu(renderer,  LSG_UI::root);
	LSG_UI::renderModal(renderer, LSG_UI::root);

	SDL_RenderPresent(renderer);
}

void LSG_UI::RemoveXmlChildNodes(LSG_Component* component)
{
	if (!component)
		throw std::invalid_argument("UI component cannot be null.");

	auto xmlNode = component->GetXmlNode();

	if (xmlNode)
		LSG_XML::RemoveChildNodes(xmlNode);
	
	for (auto child : component->GetChildren())
	{
		LSG_UI::components.erase(child->GetID());
		LSG_UI::componentsByLayer.erase(child->GetLayer());

		component->RemoveChild(child);
	}
}

void LSG_UI::RemoveXmlNode(LSG_Component* component)
{
	if (!component)
		throw std::invalid_argument("UI component cannot be null.");

	auto xmlNode = component->GetXmlNode();

	if (xmlNode)
		LSG_XML::RemoveNode(xmlNode);
	
	LSG_UI::components.erase(component->GetID());
	LSG_UI::componentsByLayer.erase(component->GetLayer());

	auto parent = component->GetParent();

	if (parent)
		parent->RemoveChild(component);
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

	if (component->IsMenu())
		component->Render(renderer);

	for (auto child : component->GetChildren())
		LSG_UI::renderMenu(renderer, child);
}

void LSG_UI::renderModal(SDL_Renderer* renderer, LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsModal())
		component->Render(renderer);

	for (auto child : component->GetChildren())
		LSG_UI::renderModal(renderer, child);
}

void LSG_UI::resetSize(LSG_Component* component)
{
	if (!component)
		return;

	component->background = {};

	for (auto child : component->GetChildren())
		LSG_UI::resetSize(child);
}

void LSG_UI::SetColorTheme(const std::string& colorThemeFile, bool sort)
{
	if (!colorThemeFile.empty() && (colorThemeFile == LSG_UI::colorThemeFile))
		return;

	LSG_UI::colorTheme.clear();

	if (!colorThemeFile.empty())
	{
		auto filePath = LSG_Text::GetFullPath(colorThemeFile);
		auto file     = std::ifstream(filePath);

		if (!file.is_open())
			throw std::runtime_error(LSG_Text::Format("Failed to open Color Theme file: %s", filePath.c_str()));

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#' || line[0] == ';')
				continue;

			auto key   = std::strtok((char*)line.c_str(), "=");
			auto value = std::strtok(nullptr, "");

			if (key && value)
				LSG_UI::colorTheme[key] = value;
		}

		file.close();
	}

	LSG_UI::colorThemeFile = colorThemeFile;

	if (!LSG_UI::root)
		return;

	LSG_UI::root->SetColors();

	LSG_UI::SetText(LSG_UI::root, sort);
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

void LSG_UI::setListItems(LSG_Component* component, bool sort)
{
	if (!component)
		return;

	if (component->IsList())
	{
		if (sort)
			static_cast<LSG_List*>(component)->SetItems();
		else
			static_cast<LSG_List*>(component)->Update();
	}

	for (auto child : component->GetChildren())
		LSG_UI::setListItems(child, sort);
}

void LSG_UI::setMenu(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsMenu())
		static_cast<LSG_Menu*>(component)->SetMenu();
	
	for (auto child : component->GetChildren())
		LSG_UI::setMenu(child);
}

void LSG_UI::setTableRows(LSG_Component* component, bool sort)
{
	if (!component)
		return;

	if (component->IsTable())
	{
		if (sort)
			static_cast<LSG_Table*>(component)->SetRows();
		else
			static_cast<LSG_Table*>(component)->Update();
	}

	for (auto child : component->GetChildren())
		LSG_UI::setTableRows(child, sort);
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

void LSG_UI::SetText(LSG_Component* component, bool sort)
{
	LSG_UI::setListItems(component, sort);
	LSG_UI::setTableRows(component, sort);
	LSG_UI::setTextLabels(component);

	LSG_UI::setMenu(component);

	LSG_UI::layoutModal(component);
}

void LSG_UI::UnhighlightComponents()
{
	if (!SDL_GetCursor())
		return;

	for (auto& component : LSG_UI::componentsByLayer)
		component.second->highlighted = false;
}
