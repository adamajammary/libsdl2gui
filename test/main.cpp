#define SDL_MAIN_HANDLED

#include "main.h"

void test_setColorTheme(const std::string& menuItemId, const std::string& colorThemeFile)
{
    LSG_SetColorTheme(colorThemeFile);
    LSG_SetMenuItemSelected(menuItemId, true);
}

void test_showAbout()
{
    auto about = "libsdl2gui is a free cross-platform user interface library using SDL2.\n\nCopyright (C) 2021 Adam A. Jammary (Jammary Studio)";

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "About", about, nullptr);
}

void test_handleIdEvent(const std::string& id)
{
    if (id == "MenuIdAbout")
        test_showAbout();
    else if (id == "MenuIdQuit")
        LSG_Quit();
    else if ((id == "ButtonIdColorThemeDark") || (id == "MenuIdColorThemeDark"))
        test_setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
    else if ((id == "ButtonIdColorThemeLight") || (id == "MenuIdColorThemeLight"))
        test_setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
}

void test_handleKeyEvent(const SDL_KeyboardEvent& event)
{
    // https://wiki.libsdl.org/SDL2/SDL_Keymod

    bool isControl = (event.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL));
    bool isKeyQ    = (event.keysym.sym == SDLK_q);

    if (isControl && isKeyQ)
    	LSG_Quit();
}

void test_handleRowEvent(const std::string& id, int row)
{
    if (id == "List")
        LSG_SetText("ListRow", std::to_string(row));
    else if (id == "Table")
        LSG_SetText("TableRow", std::to_string(row));
    else if (id == "TableWithGroups")
        LSG_SetText("TableWithGroupsRow", std::to_string(row));
}

void test_handleUserEvent(const SDL_UserEvent& event)
{
    auto type = (LSG_EventType)event.code;
    auto id   = static_cast<const char*>(event.data1);

    switch (type) {
    case LSG_EVENT_BUTTON_CLICKED:
    case LSG_EVENT_MENU_ITEM_SELECTED:
        test_handleIdEvent(id);
        break;
    case LSG_EVENT_ROW_SELECTED:
        test_handleRowEvent(id, *static_cast<int*>(event.data2));
        break;
    case LSG_EVENT_SLIDER_VALUE_CHANGED:
        LSG_SetText("SliderValue", std::format("{:.2f}", *static_cast<double*>(event.data2)));
        break;
    default:
        break;
    }
}

void test_handleEvents(const std::vector<SDL_Event>& events)
{
    for (const auto& event : events)
    {
        if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE))
            LSG_Quit();
        else if (event.type >= SDL_USEREVENT)
            test_handleUserEvent(event.user);
        else if (event.type == SDL_KEYUP)
            test_handleKeyEvent(event.key);
    }
}

void test_render(SDL_Renderer* renderer)
{
    if (!renderer || !LSG_IsRunning())
        return;

    SDL_Color color      = { 255, 0, 0, 64 };
    SDL_Size  windowSize = LSG_GetWindowSize();
    SDL_Size  overlay    = { 100, 100 };

    SDL_Rect destination = {
        ((windowSize.width  - overlay.width)  / 2),
        ((windowSize.height - overlay.height) / 2),
        overlay.width,
        overlay.height
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &destination);
}

#if defined _windows && defined _DEBUG
int wmain(int argc, wchar_t* argv[])
#elif defined _windows && defined NDEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int SDL_main(int argc, char* argv[])
#endif
{
    try
    {
        //SDL_Renderer* renderer = LSG_Start("Test SDL2 GUI", 800, 600); // Load an empty window with no UI components
        SDL_Renderer* renderer = LSG_Start("ui/main.xml");               // Load a window and UI components from an XML file

        LSG_SetMenuItemSelected("MenuIdColorThemeDark", true);

        std::vector<SDL_Event> events;

        while (LSG_IsRunning())
        {
            events = LSG_Run();

            test_handleEvents(events);
            //test_render(renderer); // Draws a transparent red squared overlay in the center of the window.

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

#if !defined _windows
#if defined __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
#if defined _ios
	return SDL_UIKitRunApp(argc, argv, SDL_main);
#else
	return SDL_main(argc, argv);
#endif
}
#endif
