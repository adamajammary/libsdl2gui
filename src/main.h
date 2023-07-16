#ifndef LSG_MAIN_H
#define LSG_MAIN_H

#include <algorithm>
#include <fstream>
#include <map>
#include <set>

#if defined _linux
	#include <gtk/gtk.h>
#elif defined _macosx
	#include <AppKit/AppKit.h>
#elif defined _windows
	#include <shobjidl_core.h>
#endif

#ifndef LIB_SDL2_TTF_H
#define LIB_SDL2_TTF_H
extern "C" {
	#include <SDL2/SDL_image.h>
	#include <SDL2/SDL_ttf.h>
}
#endif

#ifndef LIB_XML2_H
#define LIB_XML2_H
namespace LibXml {
	extern "C" {
		#include <libxml/xpath.h>
	}
}
#endif

#include <libsdl2gui.h>

#if defined _windows
	#define strdup _strdup
#endif

class LSG_Component;

using LSG_Components       = std::vector<LSG_Component*>;
using LSG_MapIntComponent  = std::map<int, LSG_Component*>;
using LSG_UMapStrStr       = std::unordered_map<std::string, std::string>;
using LSG_UMapStrComponent = std::unordered_map<std::string, LSG_Component*>;

const char        LSG_ARROW_UP[4]             = { (char)0xE2, (char)0x86, (char)0x91, 0 };
const char        LSG_ARROW_DOWN[4]           = { (char)0xE2, (char)0x86, (char)0x93, 0 };
const std::string LSG_ASCENDING               = "ascending";
const SDL_Color   LSG_DEFAULT_BACK_COLOR      = { 245, 245, 245, 255 };
const SDL_Color   LSG_DEFAULT_BORDER_COLOR    = { 0, 0, 0, 255 };
const SDL_Color   LSG_DEFAULT_TEXT_COLOR      = { 0, 0, 0, 255 };
const std::string LSG_DESCENDING              = "descending";
const int         LSG_DOUBLE_CLICK_TIME_LIMIT = 200;
const int         LSG_FONT_SIZE               = 14;
const std::string LSG_HORIZONTAL              = "horizontal";
const int         LSG_LIST_UNIT_PAGE          = 5;
const int         LSG_MAX_TEXTURE_SIZE        = 8192;
const int         LSG_MENU_LAYER_OFFSET       = 1000000;
const int         LSG_MENU_SPACING_HALF       = 5;
const int         LSG_MENU_SPACING            = 10;
const int         LSG_MENU_SPACING_2X         = 20;
const int         LSG_MENU_SUB_PADDING_X      = 20;
const int         LSG_MENU_SUB_PADDING_X_2X   = 40;
const int         LSG_MENU_SUB_PADDING_Y      = 10;
const int         LSG_MENU_SUB_PADDING_Y_2X   = 20;
const SDL_Color   LSG_SCROLL_COLOR_THUMB      = { 128, 128, 128, 255 };
const int         LSG_SCROLL_PADDING          = 5;
const int         LSG_SCROLL_PADDING_2X       = 10;
const int         LSG_SCROLL_UNIT             = 40;
const int         LSG_SCROLL_UNIT_PAGE        = 140;
const int         LSG_SCROLL_UNIT_WHEEL       = 20;
const int         LSG_SCROLL_WIDTH            = 20;
const int         LSG_SCROLL_WIDTH_2X         = 40;
const int         LSG_SCROLL_MIN_SIZE         = (LSG_SCROLL_WIDTH_2X + LSG_SCROLL_UNIT);
const SDL_Color   LSG_SLIDER_BACK_COLOR       = { 192, 192, 192, 255 };
const SDL_Color   LSG_SLIDER_PROGRESS_COLOR   = { 20, 130, 255, 255 };
const SDL_Color   LSG_SLIDER_THUMB_COLOR      = { 128, 128, 128, 255 };
const int         LSG_SLIDER_MIN_HEIGHT       = 20;
const int         LSG_SLIDER_THUMB_WIDTH      = 10;
const int         LSG_TABLE_COLUMN_SPACING    = 10;
const uint32_t    LSG_UINT32_HALF             = (UINT32_MAX / 2);
const std::string LSG_VERTICAL                = "vertical";
const int         LSG_WINDOW_MIN_SIZE         = 400;

bool SDL_ColorEquals(const SDL_Color& a, const SDL_Color& b);

#include "LSG_IEvent.h"
#include "LSG_ScrollBar.h"
#include "LSG_Component.h"
#include "LSG_Text.h"

#include "LSG_Button.h"
#include "LSG_Events.h"
#include "LSG_Image.h"
#include "LSG_List.h"
#include "LSG_ListItem.h"
#include "LSG_Menu.h"
#include "LSG_MenuItem.h"
#include "LSG_MenuSub.h"
#include "LSG_Slider.h"
#include "LSG_Table.h"
#include "LSG_TableColumn.h"
#include "LSG_TableGroup.h"
#include "LSG_TableHeader.h"
#include "LSG_TableRow.h"
#include "LSG_TextLabel.h"
#include "LSG_UI.h"
#include "LSG_Window.h"
#include "LSG_XML.h"

#endif
