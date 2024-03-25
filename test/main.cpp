#define SDL_MAIN_HANDLED

#include <cstdio> // snprintf(x)

#if defined _windows
	#include <windows.h> // WinMain(x)
#endif

#include <libsdl2gui.h>

template<typename... Args>
static std::string TextFormat(const char* formatString, const Args&... args)
{
    if (!formatString)
        return "";

    char buffer[1024] = {};
    std::snprintf(buffer, 1024, formatString, args...);

    return std::string(buffer);
}

static void setColorTheme(const std::string& menuItemId, const std::string& colorThemeFile)
{
    LSG_SetColorTheme(colorThemeFile);
    LSG_SetMenuItemSelected(menuItemId, true);
}

static void showAbout()
{
    auto about = "libsdl2gui is a free cross-platform user interface library using SDL2.\n\n(c) 2021 Adam A. Jammary (Jammary Studio)";

    SDL_ShowSimpleMessageBox(0, "About", about, nullptr);
}

static void handleIdEvent(const std::string& id)
{
    if (id == "MenuIdAbout")
        showAbout();
    else if ((id == "ButtonIdColorThemeDark") || (id == "MenuIdColorThemeDark"))
        setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
    else if ((id == "ButtonIdColorThemeLight") || (id == "MenuIdColorThemeLight"))
        setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
}

static void handleRowEvent(const std::string& id, int row)
{
    auto rowText = (row >= 0 ? std::to_string(row) : "");

    if (id == "List")
        LSG_SetText("ListRow", rowText);
    else if (id == "Table")
        LSG_SetText("TableRow", rowText);
    else if (id == "TableWithGroups")
        LSG_SetText("TableWithGroupsRow", rowText);
}

static void handleUserEvent(const SDL_UserEvent& event)
{
    auto type = (LSG_EventType)event.code;
    auto id   = static_cast<const char*>(event.data1);

    switch (type) {
    case LSG_EVENT_BUTTON_CLICKED:
    case LSG_EVENT_MENU_ITEM_SELECTED:
        handleIdEvent(id);
        break;
    case LSG_EVENT_ROW_SELECTED:
    case LSG_EVENT_ROW_UNSELECTED:
        handleRowEvent(id, *static_cast<int*>(event.data2));
        break;
    case LSG_EVENT_SLIDER_VALUE_CHANGED:
        LSG_SetText("SliderValue", TextFormat("%.2f", *static_cast<double*>(event.data2)));
        break;
    default:
        break;
    }
}

#if defined _android || defined _ios
static int handleMobileEvents(void* userdata, SDL_Event* event)
{
    if (event->type == SDL_APP_TERMINATING) {
        LSG_Quit();
        return 0;
    }

    return 1;
}
#endif

static void handleEvents(const std::vector<SDL_Event>& events)
{
    for (const auto& event : events)
    {
        if ((event.type == SDL_QUIT) || ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE)))
            LSG_Quit();
        else if (event.type >= SDL_USEREVENT)
            handleUserEvent(event.user);
    }
}

static void render(SDL_Renderer* renderer)
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
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
#else
int SDL_main(int argc, char* argv[])
#endif
{
    try
    {
        SDL_Renderer* renderer = LSG_Start("ui/main.xml");

	    #if defined _android || defined _ios
		    SDL_SetEventFilter(handleMobileEvents, nullptr);
	    #endif

        if (LSG_IsRunning())
        {
            if (!LSG_IsPreferredDarkMode())
                setColorTheme("MenuIdColorThemeLight", "ui/light.colortheme");
            else
                setColorTheme("MenuIdColorThemeDark", "ui/dark.colortheme");
        }

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
