#ifndef LIBSDL2GUI_H
#define LIBSDL2GUI_H

#define DLL __stdcall

#ifdef libsdl2gui_EXPORTS
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT __declspec(dllimport)
#endif

#include <format>
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
	LSG_EVENT_COMPONENT_CLICKED,
	LSG_EVENT_COMPONENT_DOUBLE_CLICKED,
	LSG_EVENT_COMPONENT_RIGHT_CLICKED,
	LSG_EVENT_MENU_ITEM_SELECTED,
	LSG_EVENT_ROW_ACTIVATED, // ENTER or double-click
	LSG_EVENT_ROW_SELECTED,
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

struct SDL_Size
{
	int width  = 0;
	int height = 0;
};

using LSG_Strings   = std::vector<std::string>;
using LSG_TableRows = std::vector<LSG_Strings>;

struct LSG_TableGroupRows
{
	std::string   header;
	LSG_TableRows rows;
};

using LSG_TableGroups = std::vector<LSG_TableGroupRows>;

/**
 * @brief Adds a new item to the <list> component.
 * @param id   <list> component ID
 * @param item List item
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_AddListItem(const std::string& id, const std::string& item);

/**
 * @brief Adds a new item to the <menu-sub> component.
 * @param id     <menu-sub> component ID
 * @param item   <menu-item> value
 * @param itemId Optional <menu-item> component ID, otherwise one will be generated.
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId = "");

/**
 * @brief Adds a new group with rows to the <table> component.
 * @param id    <table> component ID
 * @param group Table group name
 * @param rows  Table group rows
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_AddTableGroup(const std::string& id, const std::string& group, const LSG_TableRows& rows);

/**
 * @brief Adds a new row to the <table> component.
 * @param id      <table> component ID
 * @param columns Table row columns
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_AddTableRow(const std::string& id, const LSG_Strings& columns);

/**
 * @returns the item row from the <list> component
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT std::string DLL LSG_GetListItem(const std::string& id, int row);

/**
 * @returns all items from the <list> component
 * @param id <list> component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT LSG_Strings DLL LSG_GetListItems(const std::string& id);

/**
 * @returns the component position
 * @param id Component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT SDL_Point DLL LSG_GetPosition(const std::string& id);

/**
 * @returns the component size
 * @param id Component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT SDL_Size DLL LSG_GetSize(const std::string& id);

/**
 * @returns the row columns from the <table> component
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT LSG_Strings DLL LSG_GetTableRow(const std::string& id, int row);

/**
 * @returns all rows from the <table> component
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT LSG_TableRows DLL LSG_GetTableRows(const std::string& id);

/**
 * @returns the text value of a <text> component
 * @param id <text> component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT std::string DLL LSG_GetText(const std::string& id);

/**
 * @returns the minimum window size
 * @throws exception
 */
DLLEXPORT SDL_Size DLL LSG_GetWindowMinimumSize();

/**
 * @returns the window position
 * @throws exception
 */
DLLEXPORT SDL_Point DLL LSG_GetWindowPosition();

/**
 * @returns the window size
 * @throws exception
 */
DLLEXPORT SDL_Size DLL LSG_GetWindowSize();

/**
 * @returns the window title
 * @throws exception
 */
DLLEXPORT std::string DLL LSG_GetWindowTitle();

/**
 * @returns true if the window is maximized
 * @throws exception
 */
DLLEXPORT bool DLL LSG_IsWindowMaximized();

/**
 * @returns true if the library has been initialized and window created
 */
DLLEXPORT bool DLL LSG_IsRunning();

/**
 * @brief Displays an Open File dialog where you can select a single file.
 * @returns the selected file path or an empty string if cancelled
 * @throws exception
 */
DLLEXPORT std::string DLL LSG_OpenFile();

/**
 * @brief Displays an Open File dialog where you can select multiple files.
 * @returns the selected file paths or an empty list if cancelled
 * @throws exception
 */
DLLEXPORT std::vector<std::string> DLL LSG_OpenFiles();

/**
 * @brief Displays an Open Folder dialog where you can select a single folder.
 * @returns the selected folder path or an empty string if cancelled
 * @throws exception
 */
DLLEXPORT std::string DLL LSG_OpenFolder();

/**
 * @brief Displays an Open Folder dialog where you can select multiple folders.
 * @returns the selected folder paths or an empty list if cancelled
 * @throws exception
 */
DLLEXPORT std::vector<std::string> DLL LSG_OpenFolders();

/**
 * @brief Presents the render buffer to the screen/window.
 * @throws exception
 */
DLLEXPORT void DLL LSG_Present();

/**
 * @brief Cleans up allocated resources and closes the window.
 */
DLLEXPORT void DLL LSG_Quit();

/**
 * @brief Removes the item row from a <list> component.
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_RemoveListItem(const std::string& id, int row);

/**
 * @brief Removes the header columns from a <table> component.
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_RemoveTableHeader(const std::string& id);

/**
 * @brief Removes the grouped rows from a <table> component.
 * @param id    <table> component ID
 * @param group Table group name
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_RemoveTableGroup(const std::string& id, const std::string& group);

/**
 * @brief Removes the row columns from a <table> component.
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_RemoveTableRow(const std::string& id, int row);

/**
 * @brief Handles events and renders the UI components.
 * @returns a list of SDL2 events available during this run
 * @throws exception
 */
DLLEXPORT std::vector<SDL_Event> DLL LSG_Run();

/**
 * @brief Sets the horizontal alignment of child components in containers like <panel> and <button>, or alignment of textured components like <image> and <text> relative to available space in their background component.
 * @param id        Component ID
 * @param alignment Horizontal alignment
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment);

/**
 * @brief Sets the vertical alignment of child components in containers like <panel> and <button>, or alignment of textured components like <image> and <text> relative to available space in their background component.
 * @param id        Component ID
 * @param alignment Vertical alignment
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment);

/**
 * @brief Sets the background color of a component.
 * @param id    Component ID
 * @param color Background color
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Sets the border width of a component.
 * @param id     Component ID
 * @param border Border width in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetBorder(const std::string& id, int border);

/**
 * @brief Sets the border color of a component.
 * @param id    Component ID
 * @param color Border color
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetBorderColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Tries to load and apply the color theme file.
 * @param colorThemeFile Color theme file. ex: "ui/dark.colortheme"
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetColorTheme(const std::string& colorThemeFile);

/**
 * @brief Enables or disables the component.
 * @param id      Component ID
 * @param enabled true to enable or false to disable
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetEnabled(const std::string& id, bool enabled = true);

/**
 * @brief Sets the font size of a component.
 * @param id   Component ID
 * @param size Font size
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetFontSize(const std::string& id, int size);

/**
 * @brief Sets the height of a component.
 * @param id     Component ID
 * @param height Height in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetHeight(const std::string& id, int height);

/**
 * @brief Sets the file path of an <image> component.
 * @param id   <image> component ID
 * @param file Image file path
 * @param fill Scale the image to fill the entire background
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetImage(const std::string& id, const std::string& file, bool fill = false);

/**
 * @brief Updates and overwrites the item row in a <list> component.
 * @param id   <list> component ID
 * @param row  0-based row index
 * @param item New list item value
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetListItem(const std::string& id, int row, const std::string& item);

/**
 * @brief Sets the items of a <list> component.
 * @param id    <list> component ID
 * @param items List items
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetListItems(const std::string& id, LSG_Strings& items);

/**
 * @brief Sets the margin around a component.
 * @param id     Component ID
 * @param margin Margin in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetMargin(const std::string& id, int margin);

/**
 * @brief Sets the text value of a <menu-item> component.
 * @param id    <menu-item> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetMenuItemValue(const std::string& id, const std::string& value);

/**
 * @brief Sets the layout orientation of the children of a component.
 * @param id          Component ID
 * @param orientation Horizontal or vertical
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetOrientation(const std::string& id, LSG_Orientation orientation);

/**
 * @brief Sets the padding inside a component.
 * @param id      Component ID
 * @param padding Padding in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetPadding(const std::string& id, int padding);

/**
 * @brief Sets the size of a component.
 * @param id   Component ID
 * @param size Width and height in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetSize(const std::string& id, const SDL_Size& size);

/**
 * @brief Sets the value of a <slider> component as a percent between 0 and 1.
 * @param id      <slider> component ID
 * @param percent [0.0-1.0]
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetSliderValue(const std::string& id, double percent);

/**
 * @brief Sets the spacing between child components.
 * @param id      Component ID
 * @param spacing Spacing in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetSpacing(const std::string& id, int spacing);

/**
 * @brief Highlights the <menu-item> as selected.
 * @param id       <menu-item> component ID
 * @param selected true to select or false to unselect
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetSubMenuItemSelected(const std::string& id, bool selected = true);

/**
 * @brief Sets the grouped rows of a <table> component.
 * @param id    <table> component ID
 * @param groups Grouped table rows
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups);

/**
 * @brief Sets the header columns of a <table> component.
 * @param id    <table> component ID
 * @param header Table header columns
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetTableHeader(const std::string& id, const LSG_Strings& header);

/**
 * @brief Updates and overwrites the row columns in a <table> component.
 * @param id      <table> component ID
 * @param row     0-based row index
 * @param columns New row columns
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns);

/**
 * @brief Sets the rows of a <table> component.
 * @param id   <table> component ID
 * @param rows Table rows
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows);

/**
 * @brief Sets the text value of a <text> component.
 * @param id    <text> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetText(const std::string& id, const std::string& value);

/**
 * @brief Sets the text color of a component.
 * @param id    Component ID
 * @param color Text color
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetTextColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Shows or hides the component.
 * @param id      Component ID
 * @param visible true to show or false to hide
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetVisible(const std::string& id, bool visible = true);

/**
 * @brief Sets the width of a component.
 * @param id    Component ID
 * @param width Width in pixels
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetWidth(const std::string& id, int width);

/**
 * @brief Maximizes or restores the window.
 * @param maximized true to maximize or false to restore
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetWindowMaximized(bool maximized = true);

/**
 * @brief Sets the minimum window size.
 * @param width  Window width
 * @param height Window height
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetWindowMinimumSize(int width, int height);

/**
 * @brief Sets the window position.
 * @param x Window horizontal positon
 * @param y Window vertical position
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetWindowPosition(int x, int y);

/**
 * @brief Sets the window size.
 * @param width  Window width
 * @param height Window height
 * @throws exception
 */
DLLEXPORT void DLL LSG_SetWindowSize(int width, int height);

/**
 * @brief Sets the window title.
 * @param title Window title
 * @throws exception
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
 * @throws exception
 */
DLLEXPORT void DLL LSG_ShowRowBorder(const std::string& id, bool show = true);

/**
 * @brief Sorts the list items.
 * @param id        <list> component ID
 * @param sortOrder Ascending or descending
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SortList(const std::string& id, LSG_SortOrder sortOrder);

/**
 * @brief Sorts the table rows.
 * @param id         <table> component ID
 * @param sortOrder  Ascending or descending
 * @param sortColumn Sort column index
 * @throws invalid_argument
 * @throws exception
 */
DLLEXPORT void DLL LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn);

/**
 * @brief Tries to initialize the library and open a new window based on layout from XML file.
 * @param xmlFile Window and UI component layout file. ex: "ui/main.xml"
 * @returns an SDL2 renderer
 * @throws exception
 */
DLLEXPORT SDL_Renderer* DLL LSG_Start(const std::string& xmlFile);

/**
 * @brief Tries to initialize the library and open a new window.
 * @param title  Window title
 * @param width  Window width
 * @param height Window height
 * @returns an SDL2 renderer
 * @throws exception
 */
DLLEXPORT SDL_Renderer* DLL LSG_Start(const std::string& title, int width, int height);

#endif
