#ifndef LSG_MAIN_H
#define LSG_MAIN_H

#include <algorithm> // min/max(x)
#include <cstdio>    // snprintf(x)
#include <cstring>   // strtok(x)
#include <fstream>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>

#if defined _android
	#include <android/asset_manager_jni.h> // AAsset*, JNI*, j*
	#include <sys/stat.h>                  // mkdir(x)
#elif defined _ios
	#include <UIKit/UIKit.h> // UIScreen, UIUserInterfaceStyle*
#elif defined _linux
	#include <gtk/gtk.h> // gtk_file_chooser_dialog_new(x), gtk_dialog_run(x), gtk_file_chooser_get_uri(x)
#elif defined _macosx
	#include <AppKit/AppKit.h>         // NSApp, NSAppearanceName*, NSOpenPanel
	#include <Foundation/Foundation.h> // NSString, NSUserDefaults
#elif defined _windows
	#include <shobjidl_core.h> // GetOpenFileNameW(x), IFileOpenDialog
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
using LSG_StringsCompare   = std::function<bool(const LSG_Strings& s1, const LSG_Strings& s2)>;
using LSG_UMapStrStr       = std::unordered_map<std::string, std::string>;
using LSG_UMapStrComponent = std::unordered_map<std::string, LSG_Component*>;

const struct LSG_Const
{
	static inline const int DoubleClickTimeLimit = 200;
	static inline const int ListUnitPage         = 5;
	static inline const int PaginationFontSize   = 13;
	static inline const int TableColumnSpacing   = 10;
	static inline const int TextureMaxSize       = 8192;
	static inline const int WindowMinSize        = 400;
};

const struct LSG_ConstDefaultColor
{
	static inline const SDL_Color Background       = { 245, 245, 245, 255 };
	static inline const SDL_Color Border           = { 0, 0, 0, 255 };
	static inline const SDL_Color Text             = { 0, 0, 0, 255 };
	static inline const SDL_Color ScrollThumb      = { 128, 128, 128, 255 };
	static inline const SDL_Color SliderBackground = { 192, 192, 192, 255 };
	static inline const SDL_Color SliderProgress   = { 20, 130, 255, 255 };
	static inline const SDL_Color SliderThumb      = { 128, 128, 128, 255 };
};

const struct LSG_ConstMenu
{
	static inline const int LayerOffset   = 1000;
	static inline const int LabelSpacing  = 4;
	static inline const int SpacingHalf   = 5;
	static inline const int Spacing       = 10;
	static inline const int Spacing2x     = 20;
	static inline const int SubPaddingX   = 20;
	static inline const int SubPaddingX2x = 40;
	static inline const int SubPaddingY   = 10;
	static inline const int SubPaddingY2x = 20;
};

const struct LSG_ConstOrientation
{
	static inline const std::string Horizontal = "horizontal";
	static inline const std::string Vertical   = "vertical";

	static inline const std::string ToString(LSG_Orientation orientationEnum)
	{
		return (orientationEnum == LSG_ORIENTATION_VERTICAL ? Vertical : Horizontal);
	}
};

const struct LSG_ConstScrollBar
{
	static inline const int MinSize   = 120;
	static inline const int Padding   = 5;
	static inline const int Padding2x = 10;
	static inline const int Unit      = 40;
	static inline const int UnitPage  = 140;
	static inline const int UnitWheel = 20;
	static inline const int Width     = 20;
	static inline const int Width2x   = 40;
};

const struct LSG_ConstSlider
{
	static inline const int MinHeight  = 20;
	static inline const int ThumbWidth = 10;
};

const struct LSG_ConstSortOrder
{
	static inline const std::string Ascending  = "ascending";
	static inline const std::string Descending = "descending";

	static inline const LSG_SortOrder ToEnum(const std::string& sortOrder)
	{
		return (sortOrder == Descending ? LSG_SORT_ORDER_DESCENDING : LSG_SORT_ORDER_ASCENDING);
	}

	static inline const std::string ToString(LSG_SortOrder sortOrderEnum)
	{
		return (sortOrderEnum == LSG_SORT_ORDER_DESCENDING ? Descending : Ascending);
	}
};

const struct LSG_ConstUnicodeCharacter
{
	static inline const char ArrowUp[4]        = { (char)0xE2, (char)0x86, (char)0x91, 0 };
	static inline const char ArrowDown[4]      = { (char)0xE2, (char)0x86, (char)0x93, 0 };
	static inline const char Checkmark[4]      = { (char)0xe2, (char)0x9c, (char)0x93, 0 };
	static inline const char CheckmarkHeavy[4] = { (char)0xe2, (char)0x9c, (char)0x94, 0 };
};

const char* LSG_GetBasePath();

#include "LSG_IEvent.h"
#include "LSG_Graphics.h"
#include "LSG_Pagination.h"
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
