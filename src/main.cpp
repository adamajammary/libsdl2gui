#include "main.h"

const char ERROR_NOT_STARTED[] = "libsdl2gui has not been started, call LSG_Start.";

char* basePath  = nullptr;
bool  isRunning = false;

char* LSG_GetBasePath()
{
	return basePath;
}

std::string getErrorNoID(const std::string& component, const std::string& id)
{
	return std::format("Failed to find a {} component with ID '{}'.", component, id);
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

void LSG_AddTableRow(const std::string& id, const LSG_Strings& columns)
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

LSG_Strings LSG_GetListItems(const std::string& id)
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

int LSG_GetScrollHorizontal(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable() && !component->IsTextLabel()))
		throw std::invalid_argument(getErrorNoID("<list>, <table> or <text>", id).c_str());

	return static_cast<LSG_Text*>(component)->GetScrollX();
}

int LSG_GetScrollVertical(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable() && !component->IsTextLabel()))
		throw std::invalid_argument(getErrorNoID("<list>, <table> or <text>", id).c_str());

	return static_cast<LSG_Text*>(component)->GetScrollY();
}

int LSG_GetSelectedRow(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id).c_str());

	return static_cast<LSG_List*>(component)->GetSelectedRow();
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

double LSG_GetSliderValue(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id).c_str());

	auto value = static_cast<LSG_Slider*>(component)->GetValue();

	return value;
}

int LSG_GetSortColumn(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	return static_cast<LSG_Table*>(component)->GetSortColumn();
}

LSG_SortOrder LSG_GetSortOrder(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id).c_str());

	return static_cast<LSG_List*>(component)->GetSortOrder();
}

LSG_Strings LSG_GetTableRow(const std::string& id, int row)
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

std::string LSG_GetText(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id).c_str());

	return static_cast<LSG_TextLabel*>(component)->GetText();
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

bool LSG_IsMenuOpen(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenu())
		throw std::invalid_argument(getErrorNoID("<menu>", id).c_str());

	return LSG_UI::IsMenuOpen(component);
}

bool LSG_IsRunning()
{
	return isRunning;
}

bool LSG_IsVisible(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	return component->visible;
}

bool LSG_IsWindowMaximized()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::IsMaximized();
}

#if defined _windows
std::string LSG_OpenFile(const wchar_t* filter)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto filePathWide = LSG_Window::OpenFile(filter);
	auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());
	auto filePath     = std::string(filePathUTF8);

	SDL_free(filePathUTF8);

	return filePath;
}
#else
std::string LSG_OpenFile()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::OpenFile();
}
#endif

#if defined _windows
std::vector<std::string> LSG_OpenFiles(const wchar_t* filter)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	std::vector<std::string> filePaths;

	auto filePathsWide = LSG_Window::OpenFiles(filter);

	for (const auto& filePathWide : filePathsWide)
	{
		auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());

		filePaths.push_back(std::string(filePathUTF8));

		SDL_free(filePathUTF8);
	}

	return filePaths;
}
#else
std::vector<std::string> LSG_OpenFiles()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::OpenFiles();
}
#endif

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

std::vector<std::string> LSG_OpenFolders()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	#if defined _windows
		std::vector<std::string> folderPaths;

		auto folderPathsWide = LSG_Window::OpenFolders();

		for (const auto& folderPathWide : folderPathsWide)
		{
			auto folderPathUTF8 = SDL_iconv_wchar_utf8(folderPathWide.c_str());

			folderPaths.push_back(std::string(folderPathUTF8));

			SDL_free(folderPathUTF8);
		}

		return folderPaths;
	#else
		return LSG_Window::OpenFolders();
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

	if (basePath) {
		SDL_free(basePath);
		basePath = nullptr;
	}

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

void LSG_RemoveMenuItem(const std::string& id)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id).c_str());

	LSG_UI::RemoveMenuItem(static_cast<LSG_MenuItem*>(component));
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

#if defined _windows
std::string LSG_SaveFile(const wchar_t* filter)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto filePathWide = LSG_Window::SaveFile(filter);
	auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());
	auto filePath     = std::string(filePathUTF8);

	SDL_free(filePathUTF8);

	return filePath;
}
#else
std::string LSG_SaveFile()
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	return LSG_Window::OpenFile();
}
#endif

void LSG_ScrollHorizontal(const std::string& id, int scroll)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable() && !component->IsTextLabel()))
		throw std::invalid_argument(getErrorNoID("<list>, <table> or <text>", id).c_str());

	return static_cast<LSG_Text*>(component)->ScrollHorizontal(scroll);
}

void LSG_ScrollVertical(const std::string& id, int scroll)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable() && !component->IsTextLabel()))
		throw std::invalid_argument(getErrorNoID("<list>, <table> or <text>", id).c_str());

	return static_cast<LSG_Text*>(component)->ScrollVertical(scroll);
}

void LSG_SelectRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id).c_str());

	if (component->IsList())
		static_cast<LSG_List*>(component)->Select(row);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->Select(row);
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

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->SetFontSize(size);

	LSG_UI::SetText(component);
	LSG_UI::Layout();
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

void LSG_SetListItems(const std::string& id, const LSG_Strings& items)
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

void LSG_SetMenuItemSelected(const std::string& id, bool selected)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id).c_str());

	static_cast<LSG_MenuItem*>(component)->SetSelected(selected);
}

void LSG_SetMenuItemValue(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id).c_str());

	static_cast<LSG_MenuItem*>(component)->SetValue(value);
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

void LSG_SetSliderValue(const std::string& id, double percent)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id).c_str());

	static_cast<LSG_Slider*>(component)->SetValue(percent);
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

void LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetGroups(groups);
}

void LSG_SetTableHeader(const std::string& id, const LSG_Strings& header)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id).c_str());

	static_cast<LSG_Table*>(component)->SetHeader(header);
}

void LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns)
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

void LSG_SetVisible(const std::string& id, bool visible)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id).c_str());

	component->visible = visible;

	LSG_UI::LayoutRoot();
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

void LSG_ShowRowBorder(const std::string& id, bool show)
{
	if (!isRunning)
		throw std::exception(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id).c_str());

	static_cast<LSG_List*>(component)->showRowBorder = show;
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
	if (!basePath)
		basePath = SDL_GetBasePath();

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

	if (!basePath)
		basePath = SDL_GetBasePath();

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
