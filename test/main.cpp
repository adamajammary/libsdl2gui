#include <cstdio> // snprintf(x)
#include <format>

#if defined _windows
	#include <windows.h> // WinMain(x)
#endif

#include <libsdlui.h>

extern "C" {
	#include "SDL3/SDL_main.h"
}

static void setColorTheme(const std::string& menuItemId, const std::string& colorThemeFile, bool isInit = false)
{
    if ((colorThemeFile == LSG_GetColorTheme()) && !isInit)
        return;

    LSG_SetColorTheme(colorThemeFile);

    LSG_SetMenuItemSelected(menuItemId, true);

    if (menuItemId == "MenuIdColorThemeDark")
    {
        LSG_SetMenuItemIcon("MenuIdAbout",           "img/info-white-512.png");
        LSG_SetMenuItemIcon("MenuIdColorThemeDark",  "img/dark-white-512.png");
        LSG_SetMenuItemIcon("MenuIdColorThemeLight", "img/light-white-512.png");

        if (LSG_IsToggledOn("Toggle"))
            LSG_SetToggle("Toggle", false);
    }
    else
    {
        LSG_SetMenuItemIcon("MenuIdAbout",           "img/info-black-512.png");
        LSG_SetMenuItemIcon("MenuIdColorThemeDark",  "img/dark-black-512.png");
        LSG_SetMenuItemIcon("MenuIdColorThemeLight", "img/light-black-512.png");

        if (!LSG_IsToggledOn("Toggle"))
            LSG_SetToggle("Toggle", true);
    }
}

static void handleIdEvent(const std::string& id)
{
    if (id == "MenuIdAbout")
        LSG_OpenModal("ModalIdAbout");
    else if ((id == "ButtonIdColorThemeDark") || (id == "MenuIdColorThemeDark"))
        setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
    else if ((id == "ButtonIdColorThemeLight") || (id == "MenuIdColorThemeLight"))
        setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
}

static void handleKeyEvent(const SDL_KeyboardEvent& event)
{
    // https://wiki.libsdl.org/SDL2/SDL_Keymod

    auto key     = event.key;
    bool isCtrl  = (event.mod & SDL_KMOD_CTRL);
    bool isShift = (event.mod & SDL_KMOD_SHIFT);

    if (isCtrl && (key == SDLK_D))
        setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
    else if (isCtrl && (key == SDLK_L))
        setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
    else if (isShift && (key == SDLK_F1))
        LSG_OpenModal("ModalIdAbout");
}

static void handleRowEvent(const std::string& id, const std::vector<int>& rows)
{
    std::string rowText = (!rows.empty() ? std::to_string(rows[0]) : "");

    for (size_t i = 1; i < rows.size(); i++) {
        if (rows[i] >= 0)
            rowText.append("," + std::to_string(rows[i]));
    }

    if (id == "Cards")
        LSG_SetText("CardRow", rowText);
    else if (id == "List")
        LSG_SetText("ListRow", rowText);
    else if (id == "TableWithGroups")
        LSG_SetText("TableWithGroupsRow", rowText);
}

static void handleTileEvent(const std::string& id, const std::vector<int>& tiles)
{
    std::string tileText = (!tiles.empty() ? std::to_string(tiles[0]) : "");

    for (size_t i = 1; i < tiles.size(); i++) {
        if (tiles[i] >= 0)
            tileText.append("," + std::to_string(tiles[i]));
    }

    if (id == "Tiles")
        LSG_SetText("TileIndex", tileText);
}

static void handleUserEvent(const SDL_UserEvent& event)
{
    auto type = (LSG_EventType)event.code;
    auto id   = std::string(static_cast<const char*>(event.data1));

    switch (type) {
    case LSG_EVENT_BUTTON_CLICKED:
    case LSG_EVENT_MENU_ITEM_SELECTED:
        handleIdEvent(id);
        break;
    case LSG_EVENT_NAVIGATE_BACK:
        LSG_NavigateBack(id);
        break;
    case LSG_EVENT_NAVIGATE_END:
        LSG_NavigateEnd(id);
        break;
    case LSG_EVENT_NAVIGATE_HOME:
        LSG_NavigateHome(id);
        break;
    case LSG_EVENT_NAVIGATE_NEXT:
        LSG_NavigateNext(id);
        break;
    case LSG_EVENT_ROW_SELECTED:
    case LSG_EVENT_ROW_UNSELECTED:
        handleRowEvent(id, *static_cast<std::vector<int>*>(event.data2));
        break;
    case LSG_EVENT_SLIDER_VALUE_CHANGED:
        if (id == "Slider")
            LSG_SetText("SliderValue", std::format("{:.2f}", *static_cast<double*>(event.data2)));
        break;
    case LSG_EVENT_TILE_SELECTED:
    case LSG_EVENT_TILE_UNSELECTED:
        handleTileEvent(id, *static_cast<std::vector<int>*>(event.data2));
        break;
    case LSG_EVENT_TOGGLE_OFF:
        if (id == "Toggle")
            setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
        break;
    case LSG_EVENT_TOGGLE_ON:
        if (id == "Toggle")
            setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
        break;
    default:
        break;
    }

    if (event.data1)
        free(event.data1);

    if (event.data2)
    {
        if ((type == LSG_EVENT_ROW_ACTIVATED) || (type == LSG_EVENT_ROW_SELECTED) || (type == LSG_EVENT_ROW_UNSELECTED))
            delete static_cast<std::vector<int>*>(event.data2);
        else if (type == LSG_EVENT_SLIDER_VALUE_CHANGED)
            delete static_cast<double*>(event.data2);
        else if (type == LSG_EVENT_COMPONENT_KEY_ENTERED)
            delete static_cast<SDL_Keycode*>(event.data2);
    }
}

static void handleEvents(const std::vector<SDL_Event>& events)
{
    for (const auto& event : events)
    {
        if ((event.type == SDL_EVENT_QUIT) || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED))
            LSG_Quit();
        else if (event.type == SDL_EVENT_KEY_UP)
            handleKeyEvent(event.key);
        else if (event.type >= SDL_EVENT_USER)
            handleUserEvent(event.user);
    }
}

static std::string handleHover() {
    float x, y;
    SDL_GetMouseState(&x, &y);

    SDL_Point mousePosition = { (int)x, (int)y };

    auto value   = LSG_GetSliderValue("Slider", mousePosition);
    auto percent = (int)(value * 100.0);

    return std::format("Percent: {}%", percent);
}

static void init()
{
    LSG_SetSliderOnHoverCallback("Slider", handleHover);

    if (!LSG_IsPreferredDarkMode())
        setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme", true);
    else
        setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme", true);
}

static void render(SDL_Renderer* renderer)
{
    if (!renderer || !LSG_IsRunning())
        return;

    SDL_Color color      = { 255, 0, 0, 64 };
    SDL_Size  windowSize = LSG_GetWindowSize();
    SDL_Size  overlay    = { 100, 100 };

    SDL_FRect destination = {
        (float)((windowSize.width  - overlay.width)  / 2),
        (float)((windowSize.height - overlay.height) / 2),
        (float)overlay.width,
        (float)overlay.height
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &destination);
}

int main(int argc, char* argv[])
{
    try
    {
        SDL_Renderer* renderer = LSG_Start("ui/main.xml");

        if (LSG_IsRunning())
            init();

        std::vector<SDL_Event> events;

        while (LSG_IsRunning())
        {
            events = LSG_Run();

            handleEvents(events);
            //render(renderer); // Draws a transparent red squared overlay in the center of the window.

            if (LSG_IsRunning())
                LSG_Present();

            SDL_Delay(50);
        }

        LSG_Quit();
    }
    catch (const std::exception &e)
    {
        LSG_ShowError(e.what());
        LSG_Quit();

        return 1;
    }

    return 0;
}
