#include "main.h"

#ifndef LSG_UI_H
#define LSG_UI_H

class LSG_UI
{
private:
    LSG_UI()  {}
    ~LSG_UI() {}

private:
    static LSG_ColorThemes      colorThemes;
    static std::string          colorThemeFile;
    static LSG_UMapStrComponent components;
    static LSG_MapIntComponent  componentsByLayer;
    static int                  id;
    static LSG_Component*       root;
    static LibXml::xmlNode*     windowNode;
    static LibXml::xmlDoc*      xmlDocument;

public:
    static LSG_Component*  AddXmlNode(LibXml::xmlNode* node, LSG_Component* parent);
    static void            Close();
    static void            CloseMenu();
    static void            CloseModals();
    static SDL_Point       GetAlignedPosition(const SDL_Point& offsetPosition, int contentSize, int contentSpacing, const SDL_Size& maxSize, LSG_Component* component, LSG_Component* panel);
    static SDL_Rect        GetBackgroundArea();
    static std::string     GetColorFromTheme(const std::string& componentID, const std::string& xmlAttribute);
    static std::string     GetColorTheme();
    static LSG_Component*  GetComponent(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
    static LSG_Component*  GetComponent(const std::string& id);
    static LSG_Component*  GetComponent(const SDL_Point& mousePosition);
    static LSG_Component*  GetComponentInModal(const std::string& id);
    static LSG_Component*  GetComponentInScrollablePanel(const SDL_Point& mousePosition, LSG_Component* component);
    static SDL_Cursor*     GetCursor(LSG_Component* component, const SDL_Point& mousePosition);
    static int             GetNextId();
    static SDL_Rect        GetScrolledBackground(LSG_Component* component);
    static SDL_Point       GetScrolledPosition(const SDL_Point& mousePosition, LSG_Component* component);
    static LibXml::xmlDoc* GetXmlDocument();
    static void            HighlightComponents(const SDL_Point& mousePosition);
    static bool            IsDarkMode();
    static void            Layout();
    static void            LayoutModal(LSG_Modal* modal);
    static void            LayoutParent(LSG_Component* component);
    static void            LayoutRoot();
    static void            Load(const std::string& colorThemeFile = "");
    static LSG_UMapStrStr  OpenWindow(const std::string& xmlFile);
    static void            Present(SDL_Renderer* renderer);
    static void            RemoveXmlChildNodes(LSG_Component* component);
    static void            RemoveXmlNode(LSG_Component*       component);
    static void            Render(SDL_Renderer* renderer);
    static void            SetColorTheme(const std::string& colorThemeFile, bool sort = false);
    static void            SetEnabled(LSG_Component*   component, bool enabled = true);
    static void            SetFontSize(LSG_Component*  component, int size);
    static void            SetFontStyle(LSG_Component* component, int style);
    static void            SetModal(LSG_Modal* modal);
    static void            SetText(LSG_Component*      component, bool sort = false);
    static void            SetTextColor(LSG_Component* component, const SDL_Color& color);
    static void            UnhighlightComponents();

private:
    static void layoutFixed(LSG_Component* component);
    static void layoutModals();
    static void layoutRelative(LSG_Component*      component);
    static void layoutPositionAlign(LSG_Component* component, const LSG_Components& children);
    static void layoutSizeBlank(LSG_Component*     component, const LSG_Components& children);
    static void loadXmlNodes(LibXml::xmlNode* parentNode, LSG_Component* parent);
    static void renderMenu(SDL_Renderer*    renderer);
    static void renderModal(SDL_Renderer*   renderer);
    static void renderTooltip(SDL_Renderer* renderer);
    static void resetSize(LSG_Component*  component);
    static void setButtons(LSG_Component* component);
    static void setCards(LSG_Component*   component);
    static void setColors();
    static void setImages(LSG_Component*    component);
    static void setListItems(LSG_Component* component, bool sort = true);
    static void setMenu(LSG_Component*      component);
    static void setModals();
    static void setNavigation(LSG_Component* component);
    static void setTableRows(LSG_Component*  component, bool sort = true);
    static void setTextLabels(LSG_Component* component);
    static void setTiles(LSG_Component*      component);
    static void setToggle(LSG_Component*     component);
};

#endif
