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

#include <functional>
#include <map>
#include <string>
#include <vector>

#if defined _ios
    #include <MediaPlayer/MediaPlayer.h> // MPMediaItem, MPMediaPickerController
    #include <PhotosUI/PhotosUI.h>       // PHPickerResult, PHPickerViewController
#endif

#ifndef LIB_SDL2_H
#define LIB_SDL2_H
extern "C" {
	#include <SDL2/SDL.h>
}
#endif

#ifndef LIB_SDL2_TTF_H
#define LIB_SDL2_TTF_H
extern "C" {
	#include <SDL2/SDL_ttf.h>
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
	LSG_EVENT_NAVIGATE_BACK,
	LSG_EVENT_NAVIGATE_END,
	LSG_EVENT_NAVIGATE_HOME,
	LSG_EVENT_NAVIGATE_NEXT,
	LSG_EVENT_PAGE_NAVIGATED,
	LSG_EVENT_ROW_ACTIVATED, // ENTER or double-click
	LSG_EVENT_ROW_SELECTED,
	LSG_EVENT_ROW_UNSELECTED,
	LSG_EVENT_SLIDER_VALUE_CHANGED,
	LSG_EVENT_TABLE_COLUMN_RESIZED,
	LSG_EVENT_TEXT_INPUT_CLEARED,
	LSG_EVENT_TEXT_INPUT_COMPLETED, // ENTER
	LSG_EVENT_TILE_ACTIVATED, // ENTER or double-click
	LSG_EVENT_TILE_SELECTED,
	LSG_EVENT_TILE_UNSELECTED,
	LSG_EVENT_TOGGLED_OFF,
	LSG_EVENT_TOGGLED_ON
};

// https://www.media.mit.edu/pia/Research/deepview/exif.html
// https://exiftool.org/TagNames/EXIF.html

enum LSG_ExifTagID
{
	LSG_EXIF_TAG_ID_CAMERA_FOCAL_LENGTH  = 0x920a,
	LSG_EXIF_TAG_ID_CAMERA_MAKE          = 0x010f,
	LSG_EXIF_TAG_ID_CAMERA_MODEL         = 0x0110,
	LSG_EXIF_TAG_ID_CAMERA_FSTOP         = 0x829d,
	LSG_EXIF_TAG_ID_CAMERA_EXPOSURE_TIME = 0x829a,
	LSG_EXIF_TAG_ID_CAMERA_ISO           = 0x8827,
	LSG_EXIF_TAG_ID_DATE_TIME_ORIGINAL   = 0x9003,
	LSG_EXIF_TAG_ID_DATE_TIME_OFFSET     = 0x9011,
	LSG_EXIF_TAG_ID_GPS_LATITUDE_REF     = 0x0001,
	LSG_EXIF_TAG_ID_GPS_LATITUDE         = 0x0002,
	LSG_EXIF_TAG_ID_GPS_LONGITUDE_REF    = 0x0003,
	LSG_EXIF_TAG_ID_GPS_LONGITUDE        = 0x0004,
	LSG_EXIF_TAG_ID_GPS_ALTITUDE_REF     = 0x0005,
	LSG_EXIF_TAG_ID_GPS_ALTITUDE         = 0x0006,
	LSG_EXIF_TAG_ID_OFFSET_GPS_INFO      = 0x8825,
	LSG_EXIF_TAG_ID_OFFSET_SUB_IFD       = 0x8769,
	LSG_EXIF_TAG_ID_ORIENTATION          = 0x0112,
	LSG_EXIF_TAG_ID_THUMB_JPEG_OFFSET    = 0x0201,
	LSG_EXIF_TAG_ID_THUMB_JPEG_SIZE      = 0x0202,
	LSG_EXIF_TAG_ID_THUMB_COMPRESSION    = 0x0103
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

struct LSG_CardItem
{
	std::string title       = "";
	std::string description = "";
	std::string thumbnail   = "";
};

using LSG_CardItems = std::vector<LSG_CardItem>;

using LSG_ExifTags = std::map<uint16_t, std::string>;

struct LSG_ExifData
{
	LSG_ExifTags gps       = {};
	LSG_ExifTags tags      = {};
	SDL_Surface* thumbnail = nullptr;
};

struct LSG_ImageOrientation
{
	SDL_RendererFlip flip     = SDL_FLIP_NONE;
	double           rotation = 0.0;
};

struct LSG_GPSCoordinate
{
    double degrees = 0.0;
    double minutes = 0.0;
    double seconds = 0.0;
	double decimal = 0.0;
};

struct LSG_GPS
{
	LSG_GPSCoordinate latitude  = {};
	LSG_GPSCoordinate longitude = {};
	double            altitude  = 0.0;
};

struct LSG_SliderPart
{
	double      value   = 0.0;
	std::string tooltip = "";
};

using LSG_SliderParts = std::vector<LSG_SliderPart>;
using LSG_Strings     = std::vector<std::string>;
using LSG_TableRows   = std::vector<LSG_Strings>;

struct LSG_TableGroup
{
	std::string   group = "";
	LSG_TableRows rows  = {};
};

using LSG_TableGroups = std::vector<LSG_TableGroup>;

struct LSG_TileItem
{
	std::string image = "";
	std::string text  = "";
};

using LSG_TileItems = std::vector<LSG_TileItem>;

/**
 * @brief Adds a new card to the cards list.
 * @param id   <cards> component ID
 * @param card Card item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddCard(const std::string& id, const LSG_CardItem& card);

/**
 * @brief Adds a new item to the list.
 * @param id   <list> component ID
 * @param item List item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddListItem(const std::string& id, const std::string& item);

/**
 * @brief Adds a new item to the sub-menu.
 * @param id     <menu-sub> component ID
 * @param item   <menu-item> value
 * @param itemId <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId);

/**
 * @brief Adds a new group with rows to the table.
 * @param id    <table> component ID
 * @param group Table group and rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddTableGroup(const std::string& id, const LSG_TableGroup& group);

/**
 * @brief Adds a new row to the table.
 * @param id      <table> component ID
 * @param columns Table row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddTableRow(const std::string& id, const LSG_Strings& columns);

/**
 * @brief Adds a new tile to the tiles grid.
 * @param id   <tiles> component ID
 * @param tile Tile item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_AddTile(const std::string& id, const LSG_TileItem& tile);

/**
 * @brief Clears the text input value.
 * @param id <text-input> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ClearTextInput(const std::string& id);

/**
 * @brief Closes the modal.
 * @param id <modal> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_CloseModal(const std::string& id);

/**
 * @returns the background color of the component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Color DLL LSG_GetBackgroundColor(const std::string& id);

/**
 * @returns the card item from the cards list
 * @param id    <cards> component ID
 * @param index 0-based card index position
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_CardItem DLL LSG_GetCard(const std::string& id, int index);

/**
 * @returns all the card items from the cards list
 * @param id <cards> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_CardItems DLL LSG_GetCards(const std::string& id);

/**
 * @returns the number of card rows in the cards list
 * @param id <cards> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT size_t DLL LSG_GetCardsCount(const std::string& id);

/**
 * @returns the currently applied color theme file, ex: "ui/dark.colortheme" or "" if none applied.
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetColorTheme();

/**
 * @returns a scaled value relative to the display DPI factor
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetDPIScaled(int value);

/**
 * @returns the font style of the component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetFontStyle(const std::string& id);

/**
 * @returns EXIF (Exchangeable Image File Format) data from the image file (if it exists)
 * @param filePath Image file path
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_ExifData DLL LSG_GetImageExif(const std::string& filePath);

/**
 * @returns GPS coordinates from the EXIF tags (if they exist)
 * @param gps EXIF GPS tags
 * @throws runtime_error
 */
DLLEXPORT LSG_GPS DLL LSG_GetImageGPS(const LSG_ExifTags& gps);

/**
 * @returns the orientation of the image from the EXIF tags (if it exists)
 * @param tags EXIF data tags
 * @throws runtime_error
 */
DLLEXPORT LSG_ImageOrientation DLL LSG_GetImageOrientation(const LSG_ExifTags& tags);

/**
 * @returns a downscaled thumbnail of the original image
 * @param filePath Image file path
 * @param maxSize  Max size of thumbnail
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Surface* DLL LSG_GetImageThumbnail(const std::string& filePath, const SDL_Size& maxSize);

/**
 * @returns a downscaled thumbnail of the original image
 * @param surface Image surface
 * @param maxSize  Max size of thumbnail
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Surface* DLL LSG_GetImageThumbnail(SDL_Surface* surface, const SDL_Size& maxSize);

/**
 * @returns the last 0-based page index of the list or table
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetLastPage(const std::string& id);

/**
 * @returns the item from the list
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetListItem(const std::string& id, int row);

/**
 * @returns the number of items in the list
 * @param id  <list> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT size_t DLL LSG_GetListItemCount(const std::string& id);

/**
 * @returns all the items from the list
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
 * @returns the total number of items that can be navigated
 * @param id <navigation> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT size_t DLL LSG_GetNavigationItemCount(const std::string& id);

/**
 * @returns the current 0-based position of the navigation component
 * @param id <navigation> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetNavigationPosition(const std::string& id);

/**
 * @returns the layout orientation of the children of a component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Orientation DLL LSG_GetOrientation(const std::string& id);

/**
 * @returns the padding inside a component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetPadding(const std::string& id);

/**
 * @returns the current 0-based page index of the list or table
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetPage(const std::string& id);

/**
 * @returns the item on the current page of the list
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetPageListItem(const std::string& id, int row);

/**
 * @returns the items on the current page of the list
 * @param id <list> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetPageListItems(const std::string& id);

/**
 * @returns the groups on the current page of the table
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroups DLL LSG_GetPageTableGroups(const std::string& id);

/**
 * @returns the columns on the current page of the table
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetPageTableRow(const std::string& id, int row);

/**
 * @returns the rows on the current page of the table
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
 * @returns the value of the progress bar as a percent between 0 and 1
 * @param id <progress-bar> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT double DLL LSG_GetProgressValue(const std::string& id);

/**
 * @returns the horizontal scroll offset of the component
 * @param id <list>, <panel>, <table>, <text> or <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetScrollHorizontal(const std::string& id);

/**
 * @returns the vertical scroll offset of the component
 * @param id <list>, <panel>, <table>, <text> or <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetScrollVertical(const std::string& id);

/**
 * @returns the selected 0-based row indices (-1 for unselected) of the cards list
 * @param id <cards> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::vector<int> DLL LSG_GetSelectedCards(const std::string& id);

/**
 * @returns the selected 0-based row indices (-1 for unselected) of the list or table
 * @param id <list> or <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::vector<int> DLL LSG_GetSelectedRows(const std::string& id);

/**
 * @returns the selected 0-based tile indices (-1 for unselected) of the tiles grid
 * @param id <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::vector<int> DLL LSG_GetSelectedTiles(const std::string& id);

/**
 * @returns the component size
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT SDL_Size DLL LSG_GetSize(const std::string& id);

/**
 * @returns the slider parts as percentage values with an optional tooltip
 * @param id <slider> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_SliderParts DLL LSG_GetSliderParts(const std::string& id);

/**
 * @returns the value of the slider as a percent between 0 and 1
 * @param id <slider> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT double DLL LSG_GetSliderValue(const std::string& id);

/**
 * @returns the sort column index of the table
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetSortColumn(const std::string& id);

/**
 * @returns the sort order of the list or table
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
 * @returns the width of the table column
 * @param id     <table> component ID
 * @param column 0-based column index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT int DLL LSG_GetTableColumnWidth(const std::string& id, int column);

/**
 * @returns the group from the table
 * @param id    <table> component ID
 * @param group The group name
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroup DLL LSG_GetTableGroup(const std::string& id, const std::string& group);

/**
 * @returns all the groups from the table
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableGroups DLL LSG_GetTableGroups(const std::string& id);

/**
 * @returns the header columns from the table
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetTableHeader(const std::string& id);

/**
 * @returns the columns from the table
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_Strings DLL LSG_GetTableRow(const std::string& id, int row);

/**
 * @returns the number of rows in the table
 * @param id  <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT size_t DLL LSG_GetTableRowCount(const std::string& id);

/**
 * @returns all the rows from the table
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TableRows DLL LSG_GetTableRows(const std::string& id);

/**
 * @returns the text value of the component
 * @param id <text> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetText(const std::string& id);

/**
 * @returns the text input value
 * @param id <text-input> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetTextInputValue(const std::string& id);

/**
 * @returns the tile item from the tiles grid
 * @param id    <tiles> component ID
 * @param index 0-based tile index position
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TileItem DLL LSG_GetTile(const std::string& id, int index);

/**
 * @returns all the tile items from the tiles grid
 * @param id <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT LSG_TileItems DLL LSG_GetTiles(const std::string& id);

/**
 * @returns the number of tile items in the tiles grid
 * @param id <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT size_t DLL LSG_GetTilesCount(const std::string& id);

/**
 * @returns the header title of the modal, menu or sub-menu
 * @param id <modal>, <menu> or <menu-sub> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetTitle(const std::string& id);

/**
 * @returns the tooltip text of the component
 * @param id Component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT std::string DLL LSG_GetTooltip(const std::string& id);

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
 * @returns true if the menu item is selected
 * @param id <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsMenuItemSelected(const std::string& id);

/**
 * @returns true if the menu is open
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
 * @returns true if the toggle switch is toggled on
 * @param id <toggle> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT bool DLL LSG_IsToggledOn(const std::string& id);

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
 * @brief Recalculates and redraws the window layout.
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_Layout();

/**
 * @brief Navigates back to the previous item, and displays an updated text label.
 * @param id   <navigation> component ID
 * @param text Optional text label, shows "[new_position] / [total_items]" by default.
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_NavigateBack(const std::string& id, const std::string& text = "");

/**
 * @brief Navigates to the next item, and displays an updated text label.
 * @param id   <navigation> component ID
 * @param text Optional text label, shows "[new_position] / [total_items]" by default.
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_NavigateNext(const std::string& id, const std::string& text = "");

/**
 * @brief Navigates to the first item, and displays an updated text label.
 * @param id   <navigation> component ID
 * @param text Optional text label, shows "1 / [total_items]" by default.
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_NavigateHome(const std::string& id, const std::string& text = "");

/**
 * @brief Navigates to the last item, and displays an updated text label.
 * @param id   <navigation> component ID
 * @param text Optional text label, shows "[last_position] / [total_items]" by default.
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_NavigateEnd(const std::string& id, const std::string& text = "");

/**
 * @brief Navigates to the position, and displays an updated text label.
 * @param id       <navigation> component ID
 * @param position 0-based position
 * @param text     Optional text label, shows "[new_position] / [total_items]" by default.
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_NavigateTo(const std::string& id, int position, const std::string& text = "");

/**
 * @brief Displays an Open File dialog where you can select a single file.
 * @returns the selected file path or an empty string if cancelled
 * @param filters Optional filter by file type
 * @throws runtime_error
 */
#if defined _android || defined _linux || defined _macosx || defined _windows
DLLEXPORT std::string DLL LSG_OpenFile(const LSG_Strings& filters = {});
#endif

/**
 * @brief Displays an Open File dialog where you can select multiple files.
 * @returns the selected file paths or an empty list if cancelled
 * @param filters Optional filter by file type
 * @throws runtime_error
 */
#if defined _linux || defined _macosx || _windows
DLLEXPORT LSG_Strings DLL LSG_OpenFiles(const LSG_Strings& filters = {});
#endif

/**
 * @brief Displays an Open Folder dialog where you can select a single folder.
 * @returns the selected folder path or an empty string if cancelled
 * @throws runtime_error
 */
#if defined _android || defined _linux || defined _macosx || defined _windows
DLLEXPORT std::string DLL LSG_OpenFolder();
#endif

/**
 * @brief Displays an Open Folder dialog where you can select multiple folders.
 * @returns the selected folder paths or an empty list if cancelled
 * @throws runtime_error
 */
#if defined _linux || defined _macosx || defined _windows
DLLEXPORT LSG_Strings DLL LSG_OpenFolders();
#endif

/**
 * @brief Displays asynchronously a Document Picker dialog where you can select a single item file.
 * @param resultsCallback Callback function with an array containing the selected file, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenFile(std::function<void(NSArray<NSURL*>*)> resultsCallback);
#endif

/**
 * @brief Displays asynchronously a Document Picker dialog where you can select multiple item files.
 * @param resultsCallback Callback function with an array of selected files, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenFiles(std::function<void(NSArray<NSURL*>*)> resultsCallback);
#endif

/**
 * @brief Displays asynchronously a Document Picker dialog where you can select a single folder.
 * @param resultsCallback Callback function with an array containing the selected folder, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback);
#endif

/**
 * @brief Displays asynchronously an Open Music dialog where you can select a single media file.
 * @param resultsCallback Callback function with an array containing the selected file, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenMediaFile(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback);
#endif

/**
 * @brief Displays asynchronously an Open Music dialog where you can select multiple media files.
 * @param resultsCallback Callback function with an array of selected files, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenMediaFiles(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback);
#endif

/**
 * @brief Opens the menu.
 * @param id <menu> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_OpenMenu(const std::string& id);

/**
 * @brief Opens the modal.
 * @param id <modal> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_OpenModal(const std::string& id);

/**
 * @brief Displays asynchronously an Open Photo dialog where you can select a single image file.
 * @param resultsCallback Callback function with an array containing the selected file, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenPhotoFile(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback);
#endif

/**
 * @brief Displays asynchronously an Open Photo dialog where you can select multiple image files.
 * @param resultsCallback Callback function with an array of selected files, or an empty array if cancelled or denied access.
 * @throws runtime_error
 */
#if defined _ios
DLLEXPORT void DLL LSG_OpenPhotoFiles(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback);
#endif

/**
 * @brief Opens the sub-menu.
 * @param id <menu-sub> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_OpenSubMenu(const std::string& id);

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
 * @brief Removes the card from the cards list.
 * @param id  <cards> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveCard(const std::string& id, int row);

/**
 * @brief Removes the item from the list.
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveListItem(const std::string& id, int row);

/**
 * @brief Removes the menu item.
 * @param id  <menu-item> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveMenuItem(const std::string& id);

/**
 * @brief Removes the item on the current page of the list.
 * @param id  <list> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemovePageListItem(const std::string& id, int row);

/**
 * @brief Removes the row on the current page of the table.
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemovePageTableRow(const std::string& id, int row);

/**
 * @brief Removes the header columns from the table.
 * @param id <table> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableHeader(const std::string& id);

/**
 * @brief Removes the grouped rows from the table.
 * @param id    <table> component ID
 * @param group Table group name
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableGroup(const std::string& id, const std::string& group);

/**
 * @brief Removes the row from the table.
 * @param id  <table> component ID
 * @param row 0-based row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTableRow(const std::string& id, int row);

/**
 * @brief Removes the tile item from the tiles grid.
 * @param id    <tiles> component ID
 * @param index 0-based tile index position
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RemoveTile(const std::string& id, int index);

/**
 * @brief Renders the texture with rounded corners.
 * @param renderer    The SDL rendering context
 * @param texture     The texture to render
 * @param destination Where the texture should be rendered
 * @param clip        Optional source clipping, or NULL to render the entire texture.
 * @param radius      The corner radius in pixels
 * @param color       The backhround color used to fill the corners
 * @param id          A unique ID (like a component ID) used for texture caching
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_RenderTextureWithRoundedCorners(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& destination, const SDL_Rect* clip, int radius, const SDL_Color& color, const std::string& id);

/**
 * @brief Handles events and renders the UI components.
 * @returns a list of SDL events available during this run
 * @throws runtime_error
 */
DLLEXPORT std::vector<SDL_Event> DLL LSG_Run();

/**
 * @brief Displays a Save File dialog where you can select a single file.
 * @returns the selected file path or an empty string if cancelled
 * @param filters Optional filter by file type
 * @throws runtime_error
 */
#if defined _android || defined _linux || defined _macosx || defined _windows
DLLEXPORT std::string DLL LSG_SaveFile(const LSG_Strings& filters = {});
#endif

/**
 * @brief Scrolls the component horizontally by the specified offset.
 * @param id     <cards>, <list>, <panel>, <table>, <text> or <tiles> component ID
 * @param scroll Horizontal scroll offset
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollHorizontal(const std::string& id, int scroll);

/**
 * @brief Scrolls the component vertically by the specified offset.
 * @param id     <cards>, <list>, <panel>, <table>, <text> or <tiles> component ID
 * @param scroll Vertical scroll offset
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollVertical(const std::string& id, int scroll);

/**
 * @brief Scrolls to the bottom of the component.
 * @param id <cards>, <list>, <panel>, <table>, <text> or <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollToBottom(const std::string& id);

/**
 * @brief Scrolls to the top of the component.
 * @param id <cards>, <list>, <panel>, <table>, <text> or <tiles> component ID
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ScrollToTop(const std::string& id);

/**
 * @brief Selects the row in the cards list.
 * @param id  <cards> component ID
 * @param row 0-based row index (-1 for unselected)
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectCard(const std::string& id, int row);

/**
 * @brief Selects a row relative to the currently selected row in the cards list.
 * @param id     <cards> component ID
 * @param offset 0-based offset from current row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectCardRowByOffset(const std::string& id, int offset);

/**
 * @brief Selects the rows in the cards list.
 * @param id   <cards> component ID
 * @param rows 0-based row indices
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectCards(const std::string& id, const std::vector<int>& rows);

/**
 * @brief Selects the row in the list or table.
 * @param id  <list> or <table> component ID
 * @param row 0-based row index (-1 for unselected)
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectRow(const std::string& id, int row);

/**
 * @brief Selects a row relative to the currently selected row in the list or table.
 * @param id     <list> or <table> component ID
 * @param offset 0-based offset from current row index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectRowByOffset(const std::string& id, int offset);

/**
 * @brief Selects the rows in the list or table.
 * @param id   <list> or <table> component ID
 * @param rows 0-based row indices
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectRows(const std::string& id, const std::vector<int>& rows);

/**
 * @brief Selects the tile item in the tiles grid.
 * @param id    <tiles> component ID
 * @param index 0-based tile index position (-1 for unselected)
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectTile(const std::string& id, int index);

/**
 * @brief Selects the tile items in the tiles grid.
 * @param id      <tiles> component ID
 * @param indices 0-based tile index positions
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SelectTiles(const std::string& id, const std::vector<int>& indices);

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
 * @brief Sets the border color of a component.
 * @param id    Component ID
 * @param color Border color
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBorderColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Sets the border radius of a component.
 * @param id     Component ID
 * @param radius Border radius in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBorderRadius(const std::string& id, int radius);

/**
 * @brief Sets the border width of a component.
 * @param id    Component ID
 * @param width Border width in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetBorderWidth(const std::string& id, int width);

/**
 * @brief Sets the text and icon of a button.
 * @param id   <button> component ID
 * @param text Text label
 * @param icon Image file path
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetButton(const std::string& id, const std::string& text, const std::string& icon);

/**
 * @brief Updates and overwrites the card item in the cards list.
 * @param id    <cards> component ID
 * @param index 0-based card index position
 * @param card  New card item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetCard(const std::string& id, int index, const LSG_CardItem& card);

/**
 * @brief Sets the card items of the cards list.
 * @param id    <cards> component ID
 * @param cards Card items
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetCards(const std::string& id, const LSG_CardItems& cards);

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
 * @brief Sets the font size of a component and sub-components.
 * @param id   Component ID
 * @param size Font size
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetFontSize(const std::string& id, int size);

/**
 * @brief Sets the font style of a component and sub-components.
 * @param id    Component ID
 * @param style Font style
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetFontStyle(const std::string& id, int style);

/**
 * @brief Sets the height of a component.
 * @param id     Component ID
 * @param height Height in pixels
 * @param layout Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetHeight(const std::string& id, int height, bool layout = true);

/**
 * @brief Sets the height of a component as a percent between 0 and 1.
 * @param id      Component ID
 * @param percent [0.0 - 1.0]
 * @param layout  Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetHeight(const std::string& id, double percent, bool layout = true);

/**
 * @brief Sets the file path of an image.
 * @param id   <image> component ID
 * @param file Image file path
 * @param fill Scale the image to fill the entire background
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetImage(const std::string& id, const std::string& file, bool fill = false);

/**
 * @brief Updates and overwrites the item in the list.
 * @param id   <list> component ID
 * @param row  0-based row index
 * @param item New list item value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetListItem(const std::string& id, int row, const std::string& item);

/**
 * @brief Sets the items of the list.
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
 * @brief Sets the icon of the menu-item.
 * @param id        <menu-item> component ID
 * @param imageFile Image file path
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMenuItemIcon(const std::string& id, const std::string& imageFile);

/**
 * @brief Highlights the menu item as selected.
 * @param id       <menu-item> component ID
 * @param selected true to select or false to unselect
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMenuItemSelected(const std::string& id, bool selected = true);

/**
 * @brief Sets the text value of the menu item.
 * @param id    <menu-item> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetMenuItemValue(const std::string& id, const std::string& value);

/**
 * @brief Sets the item count of the navigation.
 * @param id                 <navigation> component ID
 * @param itemsTotal         Number of total items that can be navigated
 * @param itemsPerNavigation Number of items to navigate by
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetNavigationItemCount(const std::string& id, size_t itemsTotal, size_t itemsPerNavigation = 1);

/**
 * @brief Sets the layout orientation of the children of a component.
 * @param id          Component ID
 * @param orientation Horizontal or vertical
 * @param layout      Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetOrientation(const std::string& id, LSG_Orientation orientation, bool layout = true);

/**
 * @brief Sets the padding inside a component.
 * @param id      Component ID
 * @param padding Padding in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPadding(const std::string& id, int padding);

/**
 * @brief Navigates to and sets the page of the list or table.
 * @param id   <list> or <table> component ID
 * @param page 0-based page index
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPage(const std::string& id, int page);

/**
 * @brief Updates and overwrites the item on the current page of the list.
 * @param id   <list> component ID
 * @param row  0-based row index
 * @param item New list item value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPageListItem(const std::string& id, int row, const std::string& item);

/**
 * @brief Updates and overwrites the row columns on the current page of the table.
 * @param id      <table> component ID
 * @param row     0-based row index
 * @param columns New row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetPageTableRow(const std::string& id, int row, const LSG_Strings& columns);

/**
 * @brief Sets the value of the progress bar as a percent between 0 and 1.
 * @param id      <progress-bar> component ID
 * @param percent [0.0-1.0]
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetProgressValue(const std::string& id, double percent);

/**
 * @brief Sets the size of a component.
 * @param id     Component ID
 * @param size   Width and height in pixels
 * @param layout Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSize(const std::string& id, const SDL_Size& size, bool layout = true);

/**
 * @brief Sets the size of a component as a percent between 0 and 1.
 * @param id     Component ID
 * @param width  [0.0-1.0]
 * @param height [0.0-1.0]
 * @param layout Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSize(const std::string& id, double width, double height, bool layout = true);

/**
 * @brief Sets the slider parts as percentage values with an optional tooltip.
 * @param id    <slider> component ID
 * @param parts Slider parts
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetSliderParts(const std::string& id, const LSG_SliderParts& parts);

/**
 * @brief Sets the value of the slider as a percent between 0 and 1.
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
 * @brief Sets the width of the table column.
 * @param id     <table> component ID
 * @param column 0-based column index
 * @param width  Width in pixels
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableColumnWidth(const std::string& id, int column, int width);

/**
 * @brief Sets the rows of the group in the table.
 * @param id    <table> component ID
 * @param group The group to update
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableGroup(const std::string& id, const LSG_TableGroup& group);

/**
 * @brief Sets the groups of the table.
 * @param id     <table> component ID
 * @param groups Groups with rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups);

/**
 * @brief Sets the header columns of the table.
 * @param id     <table> component ID
 * @param header Table header columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableHeader(const std::string& id, const LSG_Strings& header);

/**
 * @brief Updates and overwrites the row columns in the table.
 * @param id      <table> component ID
 * @param row     0-based row index
 * @param columns New row columns
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns);

/**
 * @brief Sets the rows of the table.
 * @param id   <table> component ID
 * @param rows Table rows
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows);

/**
 * @brief Sets the text value of the text.
 * @param id    <text> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetText(const std::string& id, const std::string& value);

/**
 * @brief Sets the text color of a component and all sub-components.
 * @param id    Component ID
 * @param color Text color
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTextColor(const std::string& id, const SDL_Color& color);

/**
 * @brief Sets the text input value.
 * @param id    <text-input> component ID
 * @param value Text value
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTextInputValue(const std::string& id, const std::string& value);

/**
 * @brief Updates and overwrites the tile item in the tiles grid.
 * @param id    <tiles> component ID
 * @param index 0-based tile index position
 * @param tile  New tile item
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTile(const std::string& id, int index, const LSG_TileItem& tile);

/**
 * @brief Sets the tile items of the tiles grid.
 * @param id    <tiles> component ID
 * @param tiles Tile items
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTiles(const std::string& id, const LSG_TileItems& tiles);

/**
 * @brief Sets the header title of the modal, menu or sub-menu.
 * @param id    <modal>, <menu> or <menu-sub> component ID
 * @param title Header title
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTitle(const std::string& id, const std::string& title);

/**
 * @brief Toggles the switch on or off.
 * @param id <toggle> component ID
 * @param on true for on or false for off
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetToggle(const std::string& id, bool on);

/**
 * @brief Sets the tooltip text of the component.
 * @param id      Component ID
 * @param tooltip Tooltip text
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetTooltip(const std::string& id, const std::string& tooltip);

/**
 * @brief Shows or hides the component.
 * @param id      Component ID
 * @param visible true to show or false to hide
 * @param layout  Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetVisible(const std::string& id, bool visible = true, bool layout = true);

/**
 * @brief Sets the width of a component.
 * @param id     Component ID
 * @param width  Width in pixels
 * @param layout Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWidth(const std::string& id, int width, bool layout = true);

/**
 * @brief Sets the width of a component as a percent between 0 and 1.
 * @param id      Component ID
 * @param percent [0.0 - 1.0]
 * @param layout  Recalculates and redraws the window layout after applying the change
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_SetWidth(const std::string& id, double percent, bool layout = true);

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
 * @brief Shows or hides the border/rule between columns.
 * @param id   <table> component ID
 * @param show true to show or false to hide
 * @throws invalid_argument
 * @throws runtime_error
 */
DLLEXPORT void DLL LSG_ShowColumnBorder(const std::string& id, bool show = true);

/**
 * @brief Shows a modal dialog with an error icon and the error message.
 * @param message Error message
 */
DLLEXPORT void DLL LSG_ShowError(const std::string& message);

/**
 * @brief Shows or hides the border/rule between rows.
 * @param id   <list> or <table> component ID
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
 * @returns an SDL renderer
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
