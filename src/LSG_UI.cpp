#include "LSG_UI.h"

LSG_ColorThemes      LSG_UI::colorThemes       = {};
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

	auto name  = std::string(reinterpret_cast<const char*>(node->name));
	auto xmlID = LSG_XML::GetAttribute(node, "id");

	auto nodeId = LSG_UI::GetNextId();
	auto layer  = (LSG_Component::LayerOffset + nodeId);

	if (parent && parent->IsScrollablePanel(true))
		layer = nodeId;
	else if (name == "menu")
		layer = (LSG_Menu::LayerOffset + nodeId);
	else if (name == "modal")
		layer = (LSG_Modal::LayerOffset + nodeId);

	auto id = (!xmlID.empty() ? xmlID : std::format("{}_{}", name, layer));

	if (LSG_UI::components.contains(id))
		throw std::invalid_argument(std::format("Duplicate XML ID '{}' already exists.", id));

	auto component = LSG_UI::GetComponent(id, layer, node, name, parent);

	if (!component)
		return nullptr;

	if (!LSG_UI::root && (name == "panel"))
		LSG_UI::root = component;

	LSG_UI::components[id]           = component;
	LSG_UI::componentsByLayer[layer] = component;

	return component;
}

void LSG_UI::Close()
{
	LSG_UI::colorThemes.clear();
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

void LSG_UI::CloseMenu()
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->IsMenu())
			static_cast<LSG_Menu*>(component.second)->Close();
	}
}

void LSG_UI::CloseModals()
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->IsModal())
			static_cast<LSG_Modal*>(component.second)->Close();
	}
}

SDL_Point LSG_UI::GetAlignedPosition(const SDL_Point& offsetPosition, int contentSize, int contentSpacing, const SDL_Size& maxSize, LSG_Component* component, LSG_Component* panel)
{
	if (!component || !panel)
		return offsetPosition;

	auto attributes = panel->GetXmlAttributes();

	if (attributes.empty())
		return offsetPosition;

	auto halign = (attributes.contains("halign") ? attributes.at("halign") : "");
	auto valign = (attributes.contains("valign") ? attributes.at("valign") : "");

	auto border2x  = (panel->borderWidth + panel->borderWidth);
	auto padding2x = (panel->padding + panel->padding);

	auto childMargin4x = (component->margin * 4);

	SDL_Point alignedPosition = offsetPosition;

	if (panel->IsVertical())
	{
		auto remainingHeight = std::max((maxSize.height - contentSize - contentSpacing - padding2x - border2x - childMargin4x), 0);

		if (valign == "middle")
			alignedPosition.y += (remainingHeight / 2);
		else if (valign == "bottom")
			alignedPosition.y += remainingHeight;

		auto remainingWidth = std::max((maxSize.width - component->background.w - padding2x - border2x), 0);

		if (halign == "center")
			alignedPosition.x += (remainingWidth / 2);
		else if (halign == "right")
			alignedPosition.x += (remainingWidth - component->margin);
		else
			alignedPosition.x += component->margin;
	}
	else
	{
		auto remainingWidth = std::max((maxSize.width - contentSize - contentSpacing - padding2x - border2x - childMargin4x), 0);

		if (halign == "center")
			alignedPosition.x += (remainingWidth / 2);
		else if (halign == "right")
			alignedPosition.x += remainingWidth;

		auto remainingHeight = std::max((maxSize.height - component->background.h - padding2x - border2x), 0);

		if (valign == "middle")
			alignedPosition.y += (remainingHeight / 2);
		else if (valign == "bottom")
			alignedPosition.y += (remainingHeight - component->margin);
		else
			alignedPosition.y += component->margin;
	}

	return {
		std::max(alignedPosition.x, offsetPosition.x),
		std::max(alignedPosition.y, offsetPosition.y),
	};
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
			(windowSize.width  - left - right),
			(windowSize.height - top  - bottom)
		};
	#else
		SDL_Rect background = { 0, 0, windowSize.width, windowSize.height };
	#endif

	return background;
}

std::string LSG_UI::GetColorFromTheme(const std::string& componentID, const std::string& colorAttribute)
{
	if (!LSG_UI::colorThemes.contains(LSG_UI::colorThemeFile))
		return "";

	auto key   = std::format("{}.{}", componentID, colorAttribute);
	auto color = (LSG_UI::colorThemes[LSG_UI::colorThemeFile].contains(key) ? LSG_UI::colorThemes[LSG_UI::colorThemeFile][key] : "");

	return color;
}

std::string LSG_UI::GetColorTheme()
{
	return LSG_UI::colorThemeFile;
}

LSG_Component* LSG_UI::GetComponent(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
{
	LSG_Component* component = nullptr;

	if (xmlNodeName == "button")
		component = new LSG_Button(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "cards")
		component = new LSG_Cards(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "card")
		static_cast<LSG_Cards*>(parent)->AddCard(xmlNode);
	else if (xmlNodeName == "image")
		component = new LSG_Image(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "line")
		component = new LSG_Line(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "list")
		component = new LSG_List(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "list-item")
		static_cast<LSG_List*>(parent)->AddItem(LSG_XML::GetValue(xmlNode));
	else if (xmlNodeName == "menu")
		component = new LSG_Menu(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "menu-item")
		component = new LSG_MenuItem(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "menu-sub")
		component = new LSG_MenuSub(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "modal")
		component = new LSG_Modal(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "navigation")
		component = new LSG_Navigation(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "panel")
		component = new LSG_Panel(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "progress-bar")
		component = new LSG_ProgressBar(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "slider")
		component = new LSG_Slider(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "slider-part")
		static_cast<LSG_Slider*>(parent)->AddPart(xmlNode);
	else if (xmlNodeName == "table")
		component = new LSG_Table(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "table-group")
		static_cast<LSG_Table*>(parent)->AddGroup(xmlNode);
	else if (xmlNodeName == "table-header")
		static_cast<LSG_Table*>(parent)->SetHeader(xmlNode);
	else if (xmlNodeName == "table-row")
		static_cast<LSG_Table*>(parent)->AddRow(xmlNode);
	else if (xmlNodeName == "text-input")
		component = new LSG_TextInput(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "text")
		component = new LSG_TextLabel(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "tiles")
		component = new LSG_Tiles(id, layer, xmlNode, xmlNodeName, parent);
	else if (xmlNodeName == "tile")
		static_cast<LSG_Tiles*>(parent)->AddTile(xmlNode);
	else if (xmlNodeName == "toggle")
		component = new LSG_Toggle(id, layer, xmlNode, xmlNodeName, parent);

	return component;
}

LSG_Component* LSG_UI::GetComponent(const std::string& id)
{
	return (LSG_UI::components.contains(id) ? LSG_UI::components[id] : nullptr);
}

LSG_Component* LSG_UI::GetComponent(const SDL_Point& mousePosition)
{
	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		if (!component->visible)
			continue;

		if (component->IsModal())
			return component;

		if (component->IsMenu() && static_cast<LSG_Menu*>(component)->IsOpen())
			return component;

		if ((component->IsMenuItem() || component->IsSubMenu()) && static_cast<LSG_MenuItem*>(component)->IsClosed())
			continue;

		auto background  = LSG_UI::GetScrolledBackground(component);
		bool isMouseOver = SDL_PointInRect(&mousePosition, &background);

		if (isMouseOver && component->IsScrollablePanel())
		{
			if (static_cast<LSG_Panel*>(component)->IsMouseOverScrollbar(mousePosition))
				return component;

			for (auto child : component->GetChildren())
			{
				if (!child->visible)
					continue;

				auto child2 = LSG_UI::GetComponentInScrollablePanel(mousePosition, child);

				if (child2 && child2->visible)
					return child2;
			}
		}

		if (isMouseOver && (component->GetLayer() >= LSG_Component::LayerOffset))
			return component;
	}

	return nullptr;
}

LSG_Component* LSG_UI::GetComponentInModal(const std::string& id)
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->visible && component.second->IsModal())
			return static_cast<LSG_Modal*>(component.second)->GetComponent(id);
	}

	return nullptr;
}

LSG_Component* LSG_UI::GetComponentInScrollablePanel(const SDL_Point& mousePosition, LSG_Component* component)
{
	if (!component->visible)
		return nullptr;

	if (component->IsPanel())
	{
		for (auto child : component->GetChildren())
		{
			if (!child->visible)
				continue;

			auto child2 = LSG_UI::GetComponentInScrollablePanel(mousePosition, child);

			if (child2 && child2->visible)
				return child2;
		}
	}

	auto position = LSG_UI::GetScrolledPosition(mousePosition, component);

	if (SDL_PointInRect(&position, &component->background))
		return component;

	return nullptr;
}

SDL_Cursor* LSG_UI::GetCursor(LSG_Component* component, const SDL_Point& mousePosition)
{
	auto scrolledPosition = LSG_UI::GetScrolledPosition(mousePosition, component);

	SDL_Cursor* cursor = nullptr;

	if (component->IsButton())
	{
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	}
	else if (component->IsCards())
	{
		static_cast<LSG_Cards*>(component)->OnMouseOver(scrolledPosition);
	}
	else if (component->IsNavigation())
	{
		if (static_cast<LSG_Navigation*>(component)->IsMouseOverArrow(scrolledPosition))
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	}
	else if (component->IsTable())
	{
		if (static_cast<LSG_Table*>(component)->IsMouseOverColumnBorder(scrolledPosition))
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	}
	else if (component->IsTextInput())
	{
		auto textInput = static_cast<LSG_TextInput*>(component);

		if (textInput->IsMouseOverIconClear(mousePosition))
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		else
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	}
	else if (component->IsTiles())
	{
		static_cast<LSG_Tiles*>(component)->OnMouseOver(scrolledPosition);
	}
	else if (component->IsToggle())
	{
		if (static_cast<LSG_Toggle*>(component)->IsMouseOver(scrolledPosition))
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	}

	return cursor;
}

int LSG_UI::GetNextId()
{
	return ++LSG_UI::id;
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
		(component->background.x + panel->background.x + panel->margin - panel->GetScrollHorizontal()),
		(component->background.y + panel->background.y + panel->margin - panel->GetScrollVertical()),
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
		(mousePosition.x - panel->background.x - panel->margin + panel->GetScrollHorizontal()),
		(mousePosition.y - panel->background.y - panel->margin + panel->GetScrollVertical())
	};

	return scrolledPosition;
}

LibXml::xmlDoc* LSG_UI::GetXmlDocument()
{
	return LSG_UI::xmlDocument;
}

void LSG_UI::HighlightComponents(const SDL_Point& mousePosition)
{
	auto activeCursor = SDL_GetCursor();

	if (!activeCursor)
		return;

	SDL_Cursor* cursor        = nullptr;
	auto        defaultCursor = SDL_GetDefaultCursor();
	bool        isMenuOpen    = false;
	int         menuScrollY   = 0;

	for (const auto& component : LSG_UI::componentsByLayer)
		component.second->highlighted = false;

	for (auto i = LSG_UI::componentsByLayer.rbegin(); i != LSG_UI::componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		if (!component->visible || !component->enabled)
			continue;

		if (component->IsModal()) {
			cursor = static_cast<LSG_Modal*>(component)->Highlight(mousePosition);
			break;
		}

		if (component->IsMenu())
		{
			auto menu = static_cast<LSG_Menu*>(component);

			if (menu->IsOpen())
			{
				isMenuOpen  = true;
				menuScrollY = menu->GetScrollVertical();

				break;
			}

			component->highlighted = menu->IsMouseOverIconOpen(mousePosition);

			if (component->highlighted) {
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				break;
			}

			continue;
		}

		auto background = LSG_UI::GetScrolledBackground(component);

		component->highlighted = SDL_PointInRect(&mousePosition, &background);

		if (component->highlighted)
		{
			if (component->IsScrollablePanel())
			{
				if (static_cast<LSG_Panel*>(component)->IsMouseOverScrollbar(mousePosition))
					break;

				for (auto child : component->GetChildren())
				{
					if (!child->visible || !child->enabled)
						continue;

					auto child2 = LSG_UI::GetComponentInScrollablePanel(mousePosition, child);

					if (!child2 || !child->visible || !child->enabled)
						continue;

					child2->highlighted = true;

					cursor = LSG_UI::GetCursor(child2, mousePosition);

					break;
				}
			} else {
				cursor = LSG_UI::GetCursor(component, mousePosition);
			}

			break;
		}
	}

	if (isMenuOpen)
	{
		for (const auto& component : LSG_UI::componentsByLayer)
		{
			auto background = SDL_Rect(component.second->background);
			background.y   -= menuScrollY;

			if (!component.second->visible || !component.second->enabled)
				continue;

			if (component.second->IsMenu())
			{
				auto menu = static_cast<LSG_Menu*>(component.second);

				menu->Highlight(mousePosition);

				if (menu->IsHighlightedIconClose() || menu->IsHighlightedNavBack())
					cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

				continue;
			}

			bool isMenuItem = (component.second->IsMenuItem() || component.second->IsSubMenu());

			if (isMenuItem && !static_cast<LSG_MenuItem*>(component.second)->IsClosed())
			{
				component.second->highlighted = SDL_PointInRect(&mousePosition, &background);

				if (component.second->highlighted)
					cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

				continue;
			}

			component.second->highlighted = false;
		}
	}

	if (cursor) {
		if (activeCursor != cursor)
			SDL_SetCursor(cursor);
	} else {
		if (activeCursor != defaultCursor)
			SDL_SetCursor(defaultCursor);
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

	LSG_Window::SetDPIScale();

	LSG_UI::resetSize(LSG_UI::root);

	LSG_UI::root->background = LSG_UI::GetBackgroundArea();

	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);

	LSG_UI::setButtons(LSG_UI::root);
	LSG_UI::setCards(LSG_UI::root);
	LSG_UI::setImages(LSG_UI::root);
	LSG_UI::setListItems(LSG_UI::root);
	LSG_UI::setNavigation(LSG_UI::root);
	LSG_UI::setTableRows(LSG_UI::root);
	LSG_UI::setTextLabels(LSG_UI::root);
	LSG_UI::setTiles(LSG_UI::root);
	LSG_UI::setToggle(LSG_UI::root);

	LSG_UI::setMenu(LSG_UI::root);

	LSG_UI::layoutModals();

	LSG_UI::setModals();

	LSG_Graphics::DestroyTextures();
}

void LSG_UI::layoutFixed(LSG_Component* component)
{
	if (!component)
		return;

	component->SetSizeFixed();

	for (auto child : component->GetChildren())
		LSG_UI::layoutFixed(child);
}

void LSG_UI::LayoutModal(LSG_Modal* modal)
{
	if (!modal || !modal->visible)
		return;

	for (auto child : modal->GetChildren())
		LSG_UI::resetSize(child);

	modal->SetBackground();

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	modal->background.h -= headerHeight;

	LSG_UI::layoutFixed(modal);
	LSG_UI::layoutRelative(modal);

	modal->background.h += headerHeight;
}

void LSG_UI::layoutModals()
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->visible && component.second->IsModal())
			LSG_UI::LayoutModal(static_cast<LSG_Modal*>(component.second));
	}
}

void LSG_UI::LayoutParent(LSG_Component* component)
{
	if (!component)
		return;

	auto scrollableParent = component->GetScrollableParent();

	if (scrollableParent) {
		LSG_UI::LayoutRoot();
		return;
	}

	auto parent = component->GetParent();

	if (!parent) {
		LSG_UI::LayoutRoot();
		return;
	}

	LSG_UI::resetSize(component);

	LSG_UI::layoutFixed(parent);
	LSG_UI::layoutRelative(parent);

	LSG_Graphics::DestroyTextures();
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
	auto spacing    = (attributes.contains("spacing") ? LSG_Window::GetDPIScaled(std::atoi(attributes["spacing"].c_str())) : 0);
	auto border     = component->borderWidth;
	auto border2x   = (border * 2);
	auto padding    = component->padding;
	auto padding2x  = (padding * 2);
	auto offsetX    = (component->background.x + border + padding);
	auto offsetY    = (component->background.y + border + padding);
	auto maxX       = (component->background.w - border2x - padding2x);
	auto maxY       = (component->background.h - border2x - padding2x);
	auto remainingX = maxX;
	auto remainingY = maxY;

	LSG_Components visibleChildren;

	for (auto child : children) {
		if (child->visible)
			visibleChildren.push_back(child);
	}

	// TOP-LEFT ALIGN
	for (size_t i = 0; i < visibleChildren.size(); i++)
	{
		auto child = visibleChildren[i];

		if (child->IsMenu()) {
			child->background.x = offsetX;
			child->background.y = offsetY;
			return;
		}

		auto childMargin   = child->margin;
		auto childMargin2x = (childMargin * 2);

		bool addSpacing    = (i > 0);
		auto childSpacingX = (addSpacing && !isVertical ? spacing : 0);
		auto childSpacingY = (addSpacing && isVertical  ? spacing : 0);

		child->SetPositionAlign(
			(offsetX + childMargin + childSpacingX),
			(offsetY + childMargin + childSpacingY)
		);

		if (isVertical) {
			offsetY    += (child->background.h + childMargin2x + childSpacingY);
			remainingY -= (child->background.h + childMargin2x + childSpacingY);
		} else {
			offsetX    += (child->background.w + childMargin2x + childSpacingX);
			remainingX -= (child->background.w + childMargin2x + childSpacingX);
		}
	}

	// ALIGN
	for (auto child : visibleChildren)
	{
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

	LSG_UI::root->background = LSG_UI::GetBackgroundArea();

	LSG_UI::layoutFixed(LSG_UI::root);
	LSG_UI::layoutRelative(LSG_UI::root);

	LSG_UI::layoutModals();
	LSG_UI::setModals();

	LSG_UI::CloseMenu();

	LSG_Graphics::DestroyTextures();
}

void LSG_UI::layoutSizeBlank(LSG_Component* component, const LSG_Components& children)
{
	if (!component || children.empty())
		return;

	if (!component->GetParent())
	{
		auto rootMargin   = component->margin;
		auto rootMargin2x = (rootMargin + rootMargin);

		component->background.x += rootMargin;
		component->background.y += rootMargin;
		component->background.w -= rootMargin2x;
		component->background.h -= rootMargin2x;
	}

	auto componentsX = 0;
	auto componentsY = 0;

	auto border2x  = (component->borderWidth + component->borderWidth);
	auto padding2x = (component->padding + component->padding);

	bool isParentScrollablePanel = component->IsScrollablePanel(true);

	int sizeX, sizeY;

	if (isParentScrollablePanel)
	{
		auto size = static_cast<LSG_Panel*>(component)->GetSize();

		sizeX = (size.width  - border2x - padding2x);
		sizeY = (size.height - border2x - padding2x);
	}
	else
	{
		sizeX = (component->background.w - border2x - padding2x);
		sizeY = (component->background.h - border2x - padding2x);
	}

	auto xmlSpacing = component->GetXmlAttribute("spacing");
	auto spacing    = (!xmlSpacing.empty() ? LSG_Window::GetDPIScaled(std::atoi(xmlSpacing.c_str())) : 0);

	LSG_Components visibleChildren;

	for (auto child : children) {
		if (child->visible)
			visibleChildren.push_back(child);
	}

	for (size_t i = 0; i < visibleChildren.size(); i++)
	{
		auto child = visibleChildren[i];

		if (!child->visible)
			continue;

		if (child->IsMenu())
		{
			child->background.w = sizeX;
			child->background.h = sizeY;

			return;
		}

		bool addSpacing    = (i > 0);
		auto childAttribs  = child->GetXmlAttributes();

		auto width  = (childAttribs.contains("width")  ? childAttribs["width"]  : "");
		auto height = (childAttribs.contains("height") ? childAttribs["height"] : "");

		if (child->IsImage() && isParentScrollablePanel && (width.empty() || height.empty()))
		{
			auto textureSize = static_cast<LSG_Image*>(child)->GetSize();

			child->background.w = textureSize.width;
			child->background.h = textureSize.height;

			if (component->IsVertical())
				sizeY -= (child->background.h + (addSpacing ? spacing : 0));
			else
				sizeX -= (child->background.w + (addSpacing ? spacing : 0));

			continue;
		}

		if (child->IsLine())
		{
			if (child->IsVertical())
			{
				child->background.w = LSG_Line::DefaultSize;
				child->background.h = sizeY;

				if (component->IsVertical())
					sizeY = 0;
				else
					sizeX -= LSG_Line::DefaultSize;
			}
			else
			{
				child->background.h = LSG_Line::DefaultSize;
				child->background.w = sizeX;

				if (component->IsVertical())
					sizeY -= LSG_Line::DefaultSize;
				else
					sizeX = 0;
			}

			continue;
		}

		if (child->IsTextLabel())
		{
			auto textureSize = static_cast<LSG_TextLabel*>(child)->GetSize();

			if (isParentScrollablePanel) {
				child->background.w = textureSize.width;
				child->background.h = textureSize.height;
			} else {
				child->background.w = std::min(sizeX, textureSize.width);
				child->background.h = std::min(sizeY, textureSize.height);
			}

			if (component->IsVertical())
				sizeY -= (child->background.h + (addSpacing ? spacing : 0));
			else
				sizeX -= (child->background.w + (addSpacing ? spacing : 0));

			continue;
		}

		auto childMargin2x = (child->margin * 2);

		if (component->IsVertical())
		{
			componentsX = 1;

			if (height.empty()) {
				componentsY++;
				sizeY -= childMargin2x;
			} else {
				sizeY -= (child->background.h + childMargin2x);
			}

			if (addSpacing)
				sizeY -= spacing;
		}
		else
		{
			componentsY = 1;

			if (width.empty()) {
				componentsX++;
				sizeX -= childMargin2x;
			} else {
				sizeX -= (child->background.w + childMargin2x);
			}

			if (addSpacing)
				sizeX -= spacing;
		}
	}

	for (auto child : visibleChildren)
		child->SetSizeBlank(sizeX, sizeY, componentsX, componentsY);
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

		if (component && !component->IsModal())
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
		throw std::runtime_error(std::format("Failed to load XML file: {}", filePath));

	LSG_UI::windowNode = LSG_XML::GetNode("/window");

	if (!LSG_UI::windowNode)
		throw std::runtime_error(std::format("Failed to find path '/window' in XML file: {}", filePath));

	return LSG_XML::GetAttributes(LSG_UI::windowNode);
}

void LSG_UI::Present(SDL_Renderer* renderer)
{
	LSG_UI::renderMenu(renderer);
	LSG_UI::renderModal(renderer);
	LSG_UI::renderTooltip(renderer);

	SDL_RenderPresent(renderer);
}

void LSG_UI::RemoveXmlChildNodes(LSG_Component* component)
{
	if (!component)
		throw std::invalid_argument("UI component cannot be null.");

	for (auto child : component->GetChildren())
	{
		LSG_UI::components.erase(child->GetID());
		LSG_UI::componentsByLayer.erase(child->GetLayer());

		LSG_UI::RemoveXmlChildNodes(child);

		component->RemoveChild(child);
	}

	auto xmlNode = component->GetXmlNode();

	if (xmlNode)
		LSG_XML::RemoveChildNodes(xmlNode);
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

void LSG_UI::renderMenu(SDL_Renderer* renderer)
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->IsMenu())
			static_cast<LSG_Menu*>(component.second)->Render(renderer);
	}
}

void LSG_UI::renderModal(SDL_Renderer* renderer)
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->IsModal())
			static_cast<LSG_Modal*>(component.second)->Render(renderer);
	}
}

void LSG_UI::renderTooltip(SDL_Renderer* renderer)
{
	for (const auto& component : LSG_UI::componentsByLayer)
		component.second->RenderTooltip(renderer);
}

void LSG_UI::resetSize(LSG_Component* component)
{
	if (!component)
		return;

	component->background = {};

	for (auto child : component->GetChildren())
		LSG_UI::resetSize(child);
}

void LSG_UI::setButtons(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsButton())
		static_cast<LSG_Button*>(component)->Set();

	for (auto child : component->GetChildren())
		LSG_UI::setButtons(child);
}

void LSG_UI::setCards(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->SetCards();

	for (auto child : component->GetChildren())
		LSG_UI::setCards(child);
}

void LSG_UI::SetColorTheme(const std::string& colorThemeFile, bool sort)
{
	if (!colorThemeFile.empty() && (colorThemeFile == LSG_UI::colorThemeFile))
		return;

	if (!colorThemeFile.empty() && !LSG_UI::colorThemes.contains(colorThemeFile))
	{
		auto filePath = LSG_Text::GetFullPath(colorThemeFile);
		auto file     = std::ifstream(filePath);

		if (!file.is_open())
			throw std::runtime_error(std::format("Failed to open Color Theme file: {}", filePath));

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#' || line[0] == ';')
				continue;

			auto key   = std::strtok((char*)line.c_str(), "=");
			auto value = std::strtok(nullptr, "");

			if (key && value)
				LSG_UI::colorThemes[colorThemeFile][key] = value;
		}

		file.close();
	}

	LSG_UI::colorThemeFile = colorThemeFile;

	if (!LSG_UI::root)
		return;

	LSG_UI::setColors();

	LSG_UI::SetText(LSG_UI::root, sort);

	LSG_Graphics::DestroyTextures();
}

void LSG_UI::setColors()
{
	if (LSG_UI::root)
		LSG_UI::root->SetColors();

	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->IsModal())
			component.second->SetColors();
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

void LSG_UI::SetFontSize(LSG_Component* component, int size)
{
	if (!component)
		return;

	LSG_XML::SetAttribute(component->GetXmlNode(), "font-size", std::to_string(size));

	for (auto child : component->GetChildren())
		LSG_UI::SetFontSize(child, size);
}

void LSG_UI::SetFontStyle(LSG_Component* component, int style)
{
	if (!component)
		return;

	auto bold          = ((style & TTF_STYLE_BOLD)          ? "true" : "false");
	auto italic        = ((style & TTF_STYLE_ITALIC)        ? "true" : "false");
	auto strikeThrough = ((style & TTF_STYLE_STRIKETHROUGH) ? "true" : "false");
	auto underline     = ((style & TTF_STYLE_UNDERLINE)     ? "true" : "false");

	LSG_XML::SetAttribute(component->GetXmlNode(), "bold",           bold);
	LSG_XML::SetAttribute(component->GetXmlNode(), "italic",         italic);
	LSG_XML::SetAttribute(component->GetXmlNode(), "strike-through", strikeThrough);
	LSG_XML::SetAttribute(component->GetXmlNode(), "underline",      underline);

	for (auto child : component->GetChildren())
		LSG_UI::SetFontStyle(child, style);
}

void LSG_UI::setImages(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsImage())
		static_cast<LSG_Image*>(component)->Set();

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

void LSG_UI::SetModal(LSG_Modal* modal)
{
	if (!modal || !modal->visible)
		return;

	modal->Set();

	LSG_UI::setButtons(modal);
	LSG_UI::setCards(modal);
	LSG_UI::setImages(modal);
	LSG_UI::setListItems(modal);
	LSG_UI::setNavigation(modal);
	LSG_UI::setTableRows(modal);
	LSG_UI::setTextLabels(modal);
	LSG_UI::setTiles(modal);
	LSG_UI::setToggle(modal);
}

void LSG_UI::setModals()
{
	for (const auto& component : LSG_UI::componentsByLayer)
	{
		if (component.second->visible && component.second->IsModal())
			LSG_UI::SetModal(static_cast<LSG_Modal*>(component.second));
	}
}

void LSG_UI::setNavigation(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsNavigation())
		static_cast<LSG_Navigation*>(component)->Set();

	for (auto child : component->GetChildren())
		LSG_UI::setNavigation(child);
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

void LSG_UI::SetTextColor(LSG_Component* component, const SDL_Color& color)
{
	if (!component)
		return;

	component->textColor = color;

	LSG_XML::SetAttribute(component->GetXmlNode(), "text-color", LSG_Graphics::ToXmlAttribute(color));

	for (auto child : component->GetChildren())
		LSG_UI::SetTextColor(child, color);
}

void LSG_UI::setTextLabels(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->Set();
	else if (component->IsTextInput())
		static_cast<LSG_TextInput*>(component)->SetText();

	for (auto child : component->GetChildren())
		LSG_UI::setTextLabels(child);
}

void LSG_UI::SetText(LSG_Component* component, bool sort)
{
	LSG_UI::setButtons(component);
	LSG_UI::setCards(component);
	LSG_UI::setListItems(component, sort);
	LSG_UI::setNavigation(component);
	LSG_UI::setTableRows(component, sort);
	LSG_UI::setTextLabels(component);
	LSG_UI::setTiles(component);
	LSG_UI::setToggle(component);

	LSG_UI::setMenu(component);

	LSG_UI::setModals();
}

void LSG_UI::setTiles(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->SetTiles();

	for (auto child : component->GetChildren())
		LSG_UI::setTiles(child);
}

void LSG_UI::setToggle(LSG_Component* component)
{
	if (!component)
		return;

	if (component->IsToggle())
		static_cast<LSG_Toggle*>(component)->Set();

	for (auto child : component->GetChildren())
		LSG_UI::setToggle(child);
}

void LSG_UI::UnhighlightComponents()
{
	if (!SDL_GetCursor())
		return;

	for (const auto& component : LSG_UI::componentsByLayer)
		component.second->highlighted = false;
}
