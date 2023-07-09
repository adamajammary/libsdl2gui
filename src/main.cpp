#include "main.h"

const char ERROR_NOT_STARTED[] = "libsdl2gui has not been started, call LSG_Start.";

bool isRunning = false;

std::string getErrorNoID(const std::string& component, const std::string& id)
{
	return std::format("Failed to find a {} component with ID '{}'.", component, id);
}

bool SDL_ColorEquals(const SDL_Color& a, const SDL_Color& b)
{
	return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a));
}

void LSG_AddListItem(const std::string& id, const std::string& item)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	static_cast<LSG_List*>(component)->AddItem(item);
}

void LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSubMenu())
		throw std::invalid_argument(getErrorNoID("<menu-sub>", id).c_str());

	static_cast<LSG_MenuSub*>(component)->AddItem(item, itemId);
}

void LSG_AddTableGroup(const std::string& id, const std::string& group, const LSG_TableRows& rows)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->AddGroup(group, rows);
}

void LSG_AddTableRow(const std::string& id, const LSG_TableColumns& columns)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->AddRow(columns);
}

std::string LSG_GetListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	return static_cast<LSG_List*>(component)->GetItem(row);
}

LSG_ListItems LSG_GetListItems(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	return static_cast<LSG_List*>(component)->GetItems();
}

SDL_Point LSG_GetPosition(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	auto position = SDL_Point(component->background.x, component->background.y);

	return position;
}

SDL_Size LSG_GetSize(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	auto size = SDL_Size(component->background.w, component->background.h);

	return size;
}

LSG_TableColumns LSG_GetTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	return static_cast<LSG_Table*>(component)->GetRow(row);
}

LSG_TableRows LSG_GetTableRows(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	return static_cast<LSG_Table*>(component)->GetRows();
}

SDL_Size LSG_GetWindowMinimumSize()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::GetMinimumSize();
}

SDL_Point LSG_GetWindowPosition()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::GetPosition();
}

SDL_Size LSG_GetWindowSize()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::GetSize();
}

std::string LSG_GetWindowTitle()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::GetTitle();
}

bool LSG_IsWindowMaximized()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::IsMaximized();
}

bool LSG_IsRunning()
{
	return isRunning;
}

std::string LSG_OpenFile()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	#if defined _windows
		auto filePathWide = LSG_Window::OpenFile();
		auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());
		auto filePath     = std::string(filePathUTF8);

		SDL_free(filePathUTF8);

		return filePath;
	#else
		return LSG_Window::OpenFile();
	#endif
}

std::string LSG_OpenFolder()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	#if defined _windows
		auto folderPathWide = LSG_Window::OpenFolder();
		auto folderPathUTF8 = SDL_iconv_wchar_utf8(folderPathWide.c_str());
		auto folderPath     = std::string(folderPathUTF8);

		SDL_free(folderPathUTF8);

		return folderPath;
	#else
		return LSG_Window::OpenFolder();
	#endif
}

void LSG_Present()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::Present();
}

void LSG_Quit()
{
	if (!isRunning)
		return;

	isRunning = false;

	LSG_Window::Close();
	LSG_UI::Close();

	TTF_Quit();

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void LSG_RemoveListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	static_cast<LSG_List*>(component)->RemoveItem(row);
}

void LSG_RemoveTableHeader(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->RemoveHeader();
}

void LSG_RemoveTableGroup(const std::string& id, const std::string& group)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->RemoveGroup(group);
}

void LSG_RemoveTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->RemoveRow(row);
}

std::vector<SDL_Event> LSG_Run()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto events = LSG_Events::Handle();

	LSG_Window::Render();

	return events;
}

void LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetAlignmentHorizontal(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetAlignmentVertical(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->backgroundColor = color;
}

void LSG_SetBorder(const std::string& id, int border)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->border = border;

	LSG_UI::LayoutParent(component);
}

void LSG_SetBorderColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->borderColor = color;
}

void LSG_SetColorTheme(const std::string& colorThemeFile)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_UI::SetColorTheme(colorThemeFile);
}

void LSG_SetEnabled(const std::string& id, bool enabled)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->enabled = enabled;
}

void LSG_SetFontSize(const std::string& id, int size)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id).c_str());

	component->fontSize = size;

	static_cast<LSG_TextLabel*>(component)->SetText();

	LSG_UI::LayoutParent(component);
}

void LSG_SetHeight(const std::string& id, int height)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetHeight(height);

	LSG_UI::LayoutRoot();
}

void LSG_SetImage(const std::string& id, const std::string& file, bool fill)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsImage())
		throw std::invalid_argument(getErrorNoID("<image>", id).c_str());

	static_cast<LSG_Image*>(component)->SetImage(file, fill);

	LSG_UI::LayoutParent(component);
}

void LSG_SetListItem(const std::string& id, int row, const std::string& item)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	static_cast<LSG_List*>(component)->SetItem(row, item);
}

void LSG_SetListItems(const std::string& id, LSG_ListItems& items)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	static_cast<LSG_List*>(component)->SetItems(items);
}

void LSG_SetMargin(const std::string& id, int margin)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->margin = margin;

	LSG_UI::LayoutParent(component);
}

void LSG_SetOrientation(const std::string& id, LSG_Orientation orientation)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetOrientation(orientation);

	LSG_UI::LayoutRoot();
}

void LSG_SetPadding(const std::string& id, int padding)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->padding = padding;

	LSG_UI::LayoutParent(component);
}

void LSG_SetSize(const std::string& id, const SDL_Size& size)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetWidth(size.width);
	component->SetHeight(size.height);

	LSG_UI::LayoutRoot();
}

void LSG_SetSpacing(const std::string& id, int spacing)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetSpacing(spacing);

	LSG_UI::LayoutParent(component);
}

void LSG_SetSubMenuItemSelected(const std::string& id, bool selected)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id).c_str());

	static_cast<LSG_MenuItem*>(component)->SetSelected(selected);
}

void LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetGroups(groups);
}

void LSG_SetTableHeader(const std::string& id, const LSG_TableColumns& header)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetHeader(header);
}

void LSG_SetTableRow(const std::string& id, int row, const LSG_TableColumns& columns)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetRow(row, columns);
}

void LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetRows(rows);
}

void LSG_SetText(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id).c_str());

	static_cast<LSG_TextLabel*>(component)->SetText(value);

	LSG_UI::LayoutParent(component);
}

void LSG_SetTextColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->textColor = color;

	LSG_UI::SetText(component);
}

void LSG_SetWidth(const std::string& id, int width)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetWidth(width);

	LSG_UI::LayoutRoot();
}

void LSG_SetWindowMaximized(bool maximized)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::SetMaximized(maximized);
}

void LSG_SetWindowMinimumSize(int width, int height)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::SetMinimumSize(width, height);
}

void LSG_SetWindowPosition(int x, int y)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::SetPosition(x, y);
}

void LSG_SetWindowSize(int width, int height)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::SetSize(width, height);
}

void LSG_SetWindowTitle(const std::string& title)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	LSG_Window::SetTitle(title);
}

void LSG_ShowError(const std::string& message)
{
	LSG_Window::ShowMessage(message);
}

void LSG_SortList(const std::string& id, LSG_SortOrder sortOrder)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id).c_str());

	static_cast<LSG_List*>(component)->Sort(sortOrder);
}

void LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->Sort(sortOrder, sortColumn);
}

SDL_Renderer* LSG_Start(const std::string& xmlFile)
{
	auto windowAttribs = LSG_UI::OpenWindow(xmlFile);

	auto title  = (windowAttribs.contains("title")  ? windowAttribs["title"] : "");
	auto width  = (windowAttribs.contains("width")  ? std::atoi(windowAttribs["width"].c_str())  : 0);
	auto height = (windowAttribs.contains("height") ? std::atoi(windowAttribs["height"].c_str()) : 0);

	auto renderer = LSG_Start(title, width, height);

	auto minWidth  = (windowAttribs.contains("min-width")  ? std::atoi(windowAttribs["min-width"].c_str())  : 0);
	auto minHeight = (windowAttribs.contains("min-height") ? std::atoi(windowAttribs["min-height"].c_str()) : 0);

	if ((minWidth > 0) && (minHeight > 0))
		LSG_SetWindowMinimumSize(minWidth, minHeight);

	auto x = (windowAttribs.contains("x") ? std::atoi(windowAttribs["x"].c_str()) : 0);
	auto y = (windowAttribs.contains("y") ? std::atoi(windowAttribs["y"].c_str()) : 0);

	if ((x > 0) && (y > 0))
		LSG_SetWindowPosition(x, y);

	bool maximized = (windowAttribs.contains("maximized") ? (windowAttribs["maximized"] == "true") : false);

	if (maximized)
		LSG_SetWindowMaximized();

	auto colorThemeFile = (windowAttribs.contains("color-theme-file") ? windowAttribs["color-theme-file"] : "");

	LSG_UI::Load(colorThemeFile);

	return renderer;
}

SDL_Renderer* LSG_Start(const std::string& title, int width, int height)
{
	if (isRunning)
		LSG_Quit();

	#if defined _macosx
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
	#elif defined _linux
		SDL_setenv("SDL_VIDEO_X11_LEGACY_FULLSCREEN", "0", 1);

		if ((std::getenv("DISPLAY") == NULL) || (strlen(std::getenv("DISPLAY")) == 0))
			SDL_setenv("DISPLAY", ":0", 1);
	#elif defined _windows
		SetProcessDPIAware();
	#endif

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,         "2");
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED,       "0");
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

	if ((SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) || (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0))
		throw std::exception(std::format("Failed to initialize SDL2: {}", SDL_GetError()).c_str());

	if (IMG_Init(IMG_INIT_PNG) < IMG_INIT_PNG)
		throw std::exception(std::format("Failed to initialize SDL2_image: {}", TTF_GetError()).c_str());

	if (TTF_Init() < 0)
		throw std::exception(std::format("Failed to initialize SDL2_ttf: {}", TTF_GetError()).c_str());

	auto renderer = LSG_Window::Open(title, width, height);

	isRunning = true;

	return renderer;
}

#if defined _windows
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch (reason) {
        case DLL_PROCESS_ATTACH: // Initialize once for each new process. Return FALSE to fail DLL load.
			break;
        case DLL_PROCESS_DETACH:
            //if (reserved == nullptr) // do not do cleanup if process termination scenario
			break;
        case DLL_THREAD_ATTACH: // Do thread-specific initialization.
            break;
        case DLL_THREAD_DETACH: // Do thread-specific cleanup.
			break;
    }

    return TRUE;
}
#else
int entry()
{
	return 0;
}
#endif
