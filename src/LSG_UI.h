#include "main.h"

#ifndef LSG_UI_H
#define LSG_UI_H

class LSG_UI
{
private:
    LSG_UI()  {}
    ~LSG_UI() {}

private:
    static LSG_UMapStrStr       colorTheme;
    static std::string          colorThemeFile;
    static LSG_UMapStrComponent components;
    static LSG_MapIntComponent  componentsByLayer;
    static int                  id;
    static LSG_Component*       root;
    static LibXml::xmlNode*     windowNode;
    static LibXml::xmlDoc*      xmlDocument;

public:
    static void           AddSubMenuItem(LSG_MenuSub* subMenu, const std::string& item, const std::string& itemId = "");
    static void           Close();
    static void           CloseSubMenu();
    static SDL_Color      GetBackgroundColor();
    static LSG_Button*    GetButton(const SDL_Point& mousePosition);
    static std::string    GetColorFromTheme(const std::string& componentID, const std::string& xmlAttribute);
    static std::string    GetColorTheme();
    static LSG_Component* GetComponent(const std::string& id);
    static LSG_Component* GetComponent(const SDL_Point& mousePosition);
    static void           HighlightComponents(const SDL_Point& mousePosition);
    static bool           IsMenuOpen(LSG_Component* component);
    static void           Layout();
    static void           LayoutParent(LSG_Component* component);
    static void           LayoutRoot();
    static void           Load(const std::string& colorThemeFile);
    static LSG_UMapStrStr OpenWindow(const std::string& xmlFile);
    static void           Present(SDL_Renderer* renderer);
    static void           RemoveMenuItem(LSG_MenuItem* menuItem);
    static void           Render(SDL_Renderer* renderer);
    static void           SetColorTheme(const std::string& colorThemeFile);
    static void           SetEnabled(LSG_Component* component, bool enabled = true);
    static void           SetSubMenuVisible(LSG_Component* component, bool visible = true);
    static void           SetText(LSG_Component* component);
    static SDL_Color      ToSdlColor(const std::string& color);

private:
    static int  getDistanceFromMenu(LibXml::xmlNode* xmlNode);
    static void layoutFixed(LSG_Component* component);
    static void layoutRelative(LSG_Component* component);
    static void layoutPositionAlign(LSG_Component* component, const LSG_Components& children);
    static void layoutSizeBlank(LSG_Component* component,     const LSG_Components& children);
    static void loadXmlNodes(LibXml::xmlNode* parentNode, LSG_Component* parent, LibXml::xmlDoc* xmlDoc, LSG_Component* &root, LSG_UMapStrComponent& components);
    static void renderMenu(SDL_Renderer* renderer, LSG_Component* component);
    static void resetSize(LSG_Component* component);
    static void setImages(LSG_Component* component);
    static void setListItems(LSG_Component* component);
    static void setMenuItems(LSG_Component* component);
    static void setTableRows(LSG_Component* component);
    static void setTextLabels(LSG_Component* component);

};

#endif
