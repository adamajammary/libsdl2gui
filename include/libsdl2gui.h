#ifndef LIBSDL2GUI_H
#define LIBSDL2GUI_H

#if defined _windows
    #define DLL __cdecl

    #ifdef sdl2gui_EXPORTS
	    #define DLLEXPORT __declspec(dllexport)
    #else
	    #define DLLEXPORT __declspec(dllimport)
    #endif
#else
    #define DLL

    #if __GNUC__ >= 4
        #define DLLEXPORT __attribute__ ((visibility ("default")))
    #else
        #define DLLEXPORT
    #endif
#endif

#include <string>
#include <vector>

#ifndef LIB_SDL2_H
#define LIB_SDL2_H
extern "C" {
	#include <SDL2/SDL.h>
}
#endif

enum LSG_EventType
{
	LSG_EVENT_BUTTON_CLICKED,
	LSG_EVENT_BUTTON_PRESSED,
	LSG_EVENT_COMPONENT_CLICKED,
	LSG_EVENT_COMPONENT_DOUBLE_CLICKED,
	LSG_EVENT_COMPONENT_RIGHT_CLICKED,
	LSG_EVENT_COMPONENT_KEY_ENTERED,
	LSG_EVENT_COMPONENT_SCROLLED,
	LSG_EVENT_MENU_ITEM_SELECTED,
	LSG_EVENT_ROW_ACTIVATED, // ENTER or double-click
	LSG_EVENT_ROW_SELECTED,
	LSG_EVENT_ROW_UNSELECTED,
	LSG_EVENT_SLIDER_VALUE_CHANGED
};

enum LSG_HAlign
{
	LSG_HALIGN_LEFT,
	LSG_HALIGN_CENTER,
	LSG_HALIGN_RIGHT
};

enum LSG_VAlign
{
	LSG_VALIGN_TOP,
	LSG_VALIGN_MIDDLE,
	LSG_VALIGN_BOTTOM
};

enum LSG_Orientation
{
	LSG_ORIENTATION_HORIZONTAL,
	LSG_ORIENTATION_VERTICAL
};

enum LSG_SortOrder
{
	LSG_SORT_ORDER_ASCENDING,
	LSG_SORT_ORDER_DESCENDING
};

const int LSG_DEFAULT_FONT_SIZE = 14;
const int LSG_MAX_ROWS_PER_PAGE = 100;

struct SDL_Size
{
	int width  = 0;
	int height = 0;
};

using LSG_Strings   = std::vector<std::string>;
using LSG_TableRows = std::vector<LSG_Strings>;

struct LSG_TableGroup
{
	std::string   group;
	LSG_TableRows rows;
};

using LSG_TableGroups = std::vector<LSG_TableGroup>;

/**
 * @brief Adds a new item to the <list> component.
 * @param id   <list> component ID
 * @param item List item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddListItem(const std::string& id, const std::string& item);

/**
 * @brief Adds a new item to the <menu-sub> component.
 * @param id     <menu-sub> component ID
 * @param item   <menu-item> value
 * @param itemId <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId);

/**
 * @brief Adds a new group with rows to the <table> component.
 * @param id    <table> component ID
 * @param group Table group and rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddTableGroup(const std::string& id, const LSG_TableGroup& group);

/**
 * @brief Adds a new row to the <table> component.
 * @param id      <table> component ID
 * @param columns Table row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddTableRow(const std::string& id, const LSG_Strings& columns);

/**
 * @returns the background color of the component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Color DLL LSG_GetBackgroundColor(const std::string& id);

/**
 * @returns the currently applied color theme file, ex: "ui/dark.colortheme" or "" if none applied.
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetColorTheme();

/**
 * @returns the last 0-based page index of the <list> or <table> component
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetLastPage(const std::string& id);

/**
 * @returns the item from the <list> component
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetListItem(const std::string& id, int row);

/**
 * @returns all the items from the <list> component
 * @param id <list> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetListItems(const std::string& id);

/**
 * @returns the margin around a component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetMargin(const std::string& id);

/**
 * @returns the padding inside a component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetPadding(const std::string& id);

/**
 * @returns the current 0-based page index of the <list> or <table> component
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetPage(const std::string& id);

/**
 * @returns the item on the current page of the <list> component
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetPageListItem(const std::string& id, int row);

/**
 * @returns the items on the current page of the <list> component
 * @param id <list> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetPageListItems(const std::string& id);

/**
 * @returns the groups on the current page of the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroups DLL LSG_GetPageTableGroups(const std::string& id);

/**
 * @returns the columns on the current page of the <table> component
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetPageTableRow(const std::string& id, int row);

/**
 * @returns the rows on the current page of the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableRows DLL LSG_GetPageTableRows(const std::string& id);

/**
 * @returns the component position
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Point DLL LSG_GetPosition(const std::string& id);

/**
 * @returns the value of the <progress-bar> component as a percent between 0 and 1
 * @param id <progress-bar> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT double DLL LSG_GetProgressValue(const std::string& id);

/**
 * @returns the horizontal scroll offset/position of the <list>, <table> or <text> component
 * @param id <list>, <table> or <text> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetScrollHorizontal(const std::string& id);

/**
 * @returns the vertical scroll offset/position of the <list>, <table> or <text> component
 * @param id <list>, <table> or <text> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetScrollVertical(const std::string& id);

/**
 * @returns the selected 0-based row index of the <list> or <table> component
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetSelectedRow(const std::string& id);

/**
 * @returns the component size
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Size DLL LSG_GetSize(const std::string& id);

/**
 * @returns the value of the <slider> component as a percent between 0 and 1
 * @param id <slider> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT double DLL LSG_GetSliderValue(const std::string& id);

/**
 * @returns the sort column index of the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetSortColumn(const std::string& id);

/**
 * @returns the sort order of the <list> or <table> component
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_SortOrder DLL LSG_GetSortOrder(const std::string& id);

/**
 * @returns the spacing between child components
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetSpacing(const std::string& id);

/**
 * @returns the group from the <table> component
 * @param id    <table> component ID
 * @param group The group name
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroup DLL LSG_GetTableGroup(const std::string& id, const std::string& group);

/**
 * @returns all the groups from the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroups DLL LSG_GetTableGroups(const std::string& id);

/**
 * @returns the header columns from the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetTableHeader(const std::string& id);

/**
 * @returns the columns from the <table> component
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetTableRow(const std::string& id, int row);

/**
 * @returns all the rows from the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableRows DLL LSG_GetTableRows(const std::string& id);

/**
 * @returns the text value of the <text> component
 * @param id <text> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetText(const std::string& id);

/**
 * @returns the header title of the <modal>, <menu> or <menu-sub> component
 * @param id <modal>, <menu> or <menu-sub> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetTitle(const std::string& id);

/**
 * @returns the minimum window size
 * @throws runtime_error
 */
DLLEXPORT SDL_Size DLL LSG_GetWindowMinimumSize();

/**
 * @returns the window position
 * @throws runtime_error
 */
DLLEXPORT SDL_Point DLL LSG_GetWindowPosition();

/**
 * @returns the window size
 * @throws runtime_error
 */
DLLEXPORT SDL_Size DLL LSG_GetWindowSize();

/**
 * @returns the window title
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetWindowTitle();

/**
 * @returns true if the component is enabled
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsEnabled(const std::string& id);

/**
 * @returns true if the <menu-item> component is selected
 * @param id <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsMenuItemSelected(const std::string& id);

/**
 * @returns true if the <menu> component is open
 * @param id <menu> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsMenuOpen(const std::string& id);

/**
 * @returns true if the platform prefers dark mode
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsPreferredDarkMode();

/**
 * @returns true if the library has been initialized and window created
 */
DLLEXPORT bool DLL LSG_IsRunning();

/**
 * @returns true if the component is visible
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsVisible(const std::string& id);

/**
 * @returns true if the window is maximized
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsWindowMaximized();

/**
 * @brief Displays an Open File dialog where you can select a single file.
 * @returns the selected file path or an empty string if cancelled
 * @throws runtime_error
 */
#if defined _windows
DLLEXPORT std::string DLL LSG_OpenFile(const wchar_t* filter = nullptr); // https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew#members
#elif defined _linux || defined _macosx
DLLEXPORT std::string DLL LSG_OpenFile();
#endif

/**
 * @brief Displays an Open File dialog where you can select multiple files.
 * @returns the selected file paths or an empty list if cancelled
 * @throws runtime_error
 */
#if defined _windows
DLLEXPORT std::vector<std::string> DLL LSG_OpenFiles(const wchar_t* filter = nullptr);
#elif defined _linux || defined _macosx
DLLEXPORT std::vector<std::string> DLL LSG_OpenFiles();
#endif

/**
 * @brief Displays an Open Folder dialog where you can select a single folder.
 * @returns the selected folder path or an empty string if cancelled
 * @throws runtime_error
 */
#if defined _windows || defined _linux || defined _macosx
DLLEXPORT std::string DLL LSG_OpenFolder();
#endif

/**
 * @brief Displays an Open Folder dialog where you can select multiple folders.
 * @returns the selected folder paths or an empty list if cancelled
 * @throws runtime_error
 */
#if defined _windows || defined _linux || defined _macosx
DLLEXPORT std::vector<std::string> DLL LSG_OpenFolders();
#endif

/**
 * @brief Presents the render buffer to the screen/window.
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_Present();

/**
 * @brief Cleans up allocated resources and closes the window.
 */
DLLEXPORT void DLL LSG_Quit();

/**
 * @brief Removes the item from the <list> component.
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveListItem(const std::string& id, int row);

/**
 * @brief Removes the <menu-item> component.
 * @param id  <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveMenuItem(const std::string& id);

/**
 * @brief Removes the item on the current page of the <list> component.
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemovePageListItem(const std::string& id, int row);

/**
 * @brief Removes the row on the current page of the <table> component.
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemovePageTableRow(const std::string& id, int row);

/**
 * @brief Removes the header columns from the <table> component.
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableHeader(const std::string& id);

/**
 * @brief Removes the grouped rows from the <table> component.
 * @param id    <table> component ID
 * @param group Table group name
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableGroup(const std::string& id, const std::string& group);

/**
 * @brief Removes the row from the <table> component.
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableRow(const std::string& id, int row);

/**
 * @brief Handles events and renders the UI components.
 * @returns a list of SDL2 events available during this run
 * @throws runtime_error
 */
DLLEXPORT std::vector<SDL_Event> DLL LSG_Run();

/**
 * @brief Displays a Save File dialog where you can select a single file.
 * @returns the selected file path or an empty string if cancelled
 * @throws runtime_error
 */
#if defined _windows
DLLEXPORT std::string DLL LSG_SaveFile(const wchar_t* filter = nullptr); // https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew#members
#elif defined _linux || defined _macosx
DLLEXPORT std::string DLL LSG_SaveFile();
#endif

/**
 * @brief Scrolls the <list>, <table> or <text> component horizontally by the specified offset/position.
 * @param id     <list>, <table> or <text> component ID
 * @param scroll Horizontal scroll offset/position
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollHorizontal(const std::string& id, int scroll);

/**
 * @brief Scrolls the <list>, <table> or <text> component vertically by the specified offset/position.
 * @param id     <list>, <table> or <text> component ID
 * @param scroll Vertical scroll offset/position
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollVertical(const std::string& id, int scroll);

/**
 * @brief Scrolls to the bottom of the <list>, <table> or <text> component.
 * @param id <list>, <table> or <text> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollToBottom(const std::string& id);

/**
 * @brief Selects the row in the <list> or <table> component.
 * @param id  <list> or <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectRow(const std::string& id, int row);

/**
 * @brief Selects a row relative to the currently selected row in the <list> or <table> component.
 * @param id     <list> or <table> component ID
 * @param offset 0-based offset from current row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectRowByOffset(const std::string& id, int offset);

/**
 * @brief Sets the horizontal alignment of child components in containers like <panel> and <button>, or alignment of textured components like <image> and <text> relative to available space in their background component.
 * @param id        Component ID
 * @param alignment Horizontal alignment
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment);

/**
 * @brief Sets the vertical alignment of child components in containers like <panel> and <button>, or alignment of textured components like <image> and <text> relative to available space in their background component.
 * @param id        Component ID
 * @param alignment Vertical alignment
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment);

/**
 * @brief Sets the background color of a component.
 * @param id    Component ID
 * @param color Background color
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Sets the border width of a component.
 * @param id     Component ID
 * @param border Border width in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBorder(const std::string& id, int border);

/**
 * @brief Sets the border color of a component.
 * @param id    Component ID
 * @param color Border color
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBorderColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Highlights the <button> as selected.
 * @param id       <button> component ID
 * @param selected true to select or false to unselect
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetButtonSelected(const std::string& id, bool selected = true);

/**
 * @brief Tries to load and apply the color theme file.
 * @param colorThemeFile Color theme file. ex: "ui/dark.colortheme"
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetColorTheme(const std::string& colorThemeFile);

/**
 * @brief Enables or disables the component.
 * @param id      Component ID
 * @param enabled true to enable or false to disable
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetEnabled(const std::string& id, bool enabled = true);

/**
 * @brief Sets the font size of a component.
 * @param id   Component ID
 * @param size Font size
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetFontSize(const std::string& id, int size);

/**
 * @brief Sets the height of a component.
 * @param id     Component ID
 * @param height Height in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetHeight(const std::string& id, int height);

/**
 * @brief Sets the file path of an <image> component.
 * @param id   <image> component ID
 * @param file Image file path
 * @param fill Scale the image to fill the entire background
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetImage(const std::string& id, const std::string& file, bool fill = false);

/**
 * @brief Updates and overwrites the item in the <list> component.
 * @param id   <list> component ID
 * @param row  0-based row index
 * @param item New list item value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetListItem(const std::string& id, int row, const std::string& item);

/**
 * @brief Sets the items of the <list> component.
 * @param id    <list> component ID
 * @param items List items
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetListItems(const std::string& id, const LSG_Strings& items);

/**
 * @brief Sets the margin around a component.
 * @param id     Component ID
 * @param margin Margin in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMargin(const std::string& id, int margin);

/**
 * @brief Highlights the <menu-item> as selected.
 * @param id       <menu-item> component ID
 * @param selected true to select or false to unselect
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMenuItemSelected(const std::string& id, bool selected = true);

/**
 * @brief Sets the text value of the <menu-item> component.
 * @param id    <menu-item> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMenuItemValue(const std::string& id, const std::string& value);

/**
 * @brief Sets the layout orientation of the children of a component.
 * @param id          Component ID
 * @param orientation Horizontal or vertical
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetOrientation(const std::string& id, LSG_Orientation orientation);

/**
 * @brief Sets the padding inside a component.
 * @param id      Component ID
 * @param padding Padding in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPadding(const std::string& id, int padding);

/**
 * @brief Navigates to and sets the page of the <list> or <table> component.
 * @param id   <list> or <table> component ID
 * @param page 0-based page index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPage(const std::string& id, int page);

/**
 * @brief Updates and overwrites the item on the current page of the <list> component.
 * @param id   <list> component ID
 * @param row  0-based row index
 * @param item New list item value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPageListItem(const std::string& id, int row, const std::string& item);

/**
 * @brief Updates and overwrites the row columns on the current page of the <table> component.
 * @param id      <table> component ID
 * @param row     0-based row index
 * @param columns New row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPageTableRow(const std::string& id, int row, const LSG_Strings& columns);

/**
 * @brief Sets the value of the <progress-bar> component as a percent between 0 and 1.
 * @param id      <progress-bar> component ID
 * @param percent [0.0-1.0]
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetProgressValue(const std::string& id, double percent);

/**
 * @brief Sets the size of a component.
 * @param id   Component ID
 * @param size Width and height in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSize(const std::string& id, const SDL_Size& size);

/**
 * @brief Sets the value of the <slider> component as a percent between 0 and 1.
 * @param id      <slider> component ID
 * @param percent [0.0-1.0]
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSliderValue(const std::string& id, double percent);

/**
 * @brief Sets the spacing between child components.
 * @param id      Component ID
 * @param spacing Spacing in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSpacing(const std::string& id, int spacing);

/**
 * @brief Sets the rows of the group in the <table> component.
 * @param id    <table> component ID
 * @param group The group to update
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableGroup(const std::string& id, const LSG_TableGroup& group);

/**
 * @brief Sets the groups of the <table> component.
 * @param id     <table> component ID
 * @param groups Groups with rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups);

/**
 * @brief Sets the header columns of the <table> component.
 * @param id    <table> component ID
 * @param header Table header columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableHeader(const std::string& id, const LSG_Strings& header);

/**
 * @brief Updates and overwrites the row columns in the <table> component.
 * @param id      <table> component ID
 * @param row     0-based row index
 * @param columns New row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns);

/**
 * @brief Sets the rows of the <table> component.
 * @param id   <table> component ID
 * @param rows Table rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows);

/**
 * @brief Sets the text value of the <text> component.
 * @param id    <text> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetText(const std::string& id, const std::string& value);

/**
 * @brief Sets the text color of a component.
 * @param id    Component ID
 * @param color Text color
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTextColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Sets the header title of the <modal>, <menu> or <menu-sub> component.
 * @param id    <modal>, <menu> or <menu-sub> component ID
 * @param title Header title
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTitle(const std::string& id, const std::string& title);

/**
 * @brief Shows or hides the component.
 * @param id      Component ID
 * @param visible true to show or false to hide
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetVisible(const std::string& id, bool visible = true);

/**
 * @brief Sets the width of a component.
 * @param id    Component ID
 * @param width Width in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWidth(const std::string& id, int width);

/**
 * @brief Maximizes or restores the window.
 * @param maximized true to maximize or false to restore
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWindowMaximized(bool maximized = true);

/**
 * @brief Sets the minimum window size.
 * @param width  Window width
 * @param height Window height
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWindowMinimumSize(int width, int height);

/**
 * @brief Sets the window position.
 * @param x Window horizontal positon
 * @param y Window vertical position
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWindowPosition(int x, int y);

/**
 * @brief Sets the window size.
 * @param width  Window width
 * @param height Window height
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWindowSize(int width, int height);

/**
 * @brief Sets the window title.
 * @param title Window title
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWindowTitle(const std::string& title);

/**
 * @brief Shows a modal dialog with an error icon and the error message.
 * @param message Error message
 */
DLLEXPORT void DLL LSG_ShowError(const std::string& message);

/**
 * @brief Shows or hides the border/rule between rows.
 * @param show true to show or false to hide
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ShowRowBorder(const std::string& id, bool show = true);

/**
 * @brief Sorts the list items.
 * @param id        <list> component ID
 * @param sortOrder Ascending or descending
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SortList(const std::string& id, LSG_SortOrder sortOrder);

/**
 * @brief Sorts the table rows.
 * @param id         <table> component ID
 * @param sortOrder  Ascending or descending
 * @param sortColumn Sort column index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn);

/**
 * @brief Tries to initialize the library and open a new window based on layout from XML file.
 * @param xmlFile Window and UI component layout file. ex: "ui/main.xml"
 * @returns an SDL2 renderer
 * @throws runtime_error
 */
DLLEXPORT SDL_Renderer* DLL LSG_Start(const std::string& xmlFile);

#if defined _windows && defined _DEBUG
/**
 * @brief Used by unit testing framework.
 */
DLLEXPORT void DLL LSG_StartTest(const std::string& xmlFile, const std::string& workingDir);
#endif

#endif
