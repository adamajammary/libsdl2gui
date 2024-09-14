#ifndef LSG_MAIN_H
#define LSG_MAIN_H

#include <algorithm> // min/max(x)
#include <cstdio>    // snprintf(x)
#include <cstring>   // strtok(x)
#include <filesystem>
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

#ifndef LIB_SDL2_SYSWM_H
#define LIB_SDL2_SYSWM_H
extern "C" {
	#include <SDL2/SDL_syswm.h>
}
#endif

#ifndef LIB_SDL2_IMAGE_H
#define LIB_SDL2_IMAGE_H
extern "C" {
	#include <SDL2/SDL_image.h>
}
#endif

#ifndef LIB_SDL2_TTF_H
#define LIB_SDL2_TTF_H
extern "C" {
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
using LSG_TableRowCompare  = std::function<bool(const LSG_Strings& row1, const LSG_Strings& row2)>;
using LSG_MapIntComponent  = std::map<int, LSG_Component*>;
using LSG_UMapStrStr       = std::unordered_map<std::string, std::string>;
using LSG_UMapStrComponent = std::unordered_map<std::string, LSG_Component*>;
using LSG_UmapStrSize      = std::unordered_map<std::string, SDL_Size>;
using LSG_XmlNodes         = std::vector<LibXml::xmlNode*>;

enum LSG_MenuTexture
{
	LSG_MENU_TEXTURE_ICON_CLOSE,
	LSG_MENU_TEXTURE_ICON_OPEN,
	LSG_MENU_TEXTURE_NAV_BACK,
	LSG_MENU_TEXTURE_TITLE,
	NR_OF_MENU_TEXTURES
};

enum LSG_MenuItemTexture
{
	LSG_MENU_ITEM_TEXTURE_ICON,
	LSG_MENU_ITEM_TEXTURE_TEXT,
	LSG_MENU_ITEM_TEXTURE_KEY,
	LSG_MENU_ITEM_TEXTURE_SELECTED,
	NR_OF_MENU_ITEM_TEXTURES
};

enum LSG_MenuSubTexture
{
	LSG_SUB_MENU_TEXTURE_TEXT,
	LSG_SUB_MENU_TEXTURE_ARROW,
	NR_OF_SUB_MENU_TEXTURES
};

enum LSG_ModalTexture
{
	LSG_MODAL_TEXTURE_ICON_CLOSE,
	LSG_MODAL_TEXTURE_TITLE,
	NR_OF_MODAL_TEXTURES
};

enum LSG_TextInputTexture
{
	LSG_TEXT_INPUT_TEXTURE_ICON_CLEAR,
	LSG_TEXT_INPUT_TEXTURE_PLACEHOLDER,
	LSG_TEXT_INPUT_TEXTURE_VALUE,
	NR_OF_TEXT_INPUT_TEXTURES
};

enum LSG_TriangleOrientation
{
	LSG_TRIANGLE_ORIENTATION_LEFT,
	LSG_TRIANGLE_ORIENTATION_RIGHT,
	LSG_TRIANGLE_ORIENTATION_UP,
	LSG_TRIANGLE_ORIENTATION_DOWN
};

#if defined _android
struct LSG_ConstAndroid
{
	static inline const std::string ActivityClassPath = "com/libsdl2gui/lib/Sdl2GuiActivity";
};
#endif

struct LSG_ConstBytes
{
private:
	static inline const auto FirstByte  = (1 + 0x0);
	static inline const auto SecondByte = (1 + 0xFF);
	static inline const auto ThirdByte  = (1 + 0xFFFF);
	static inline const auto FourthByte = (1 + 0xFFFFFF);

public:
	static inline const int ToInt(uint8_t a, uint8_t b, bool littleEndian = true)
	{
		if (littleEndian)
			return ((b * SecondByte) + (a * FirstByte));

		return ((a * SecondByte) + (b * FirstByte));
	}

	static inline const int ToInt(uint8_t a, uint8_t b, uint8_t c, uint8_t d, bool littleEndian = true)
	{
		if (littleEndian)
			return ((d * FourthByte) + (c * ThirdByte) + (b * SecondByte) + (a * FirstByte));

		return ((a * FourthByte) + (b * ThirdByte) + (c * SecondByte) + (d * FirstByte));
	}
};

struct LSG_Cursor
{
	static inline const int IBeamOffset = 3;
};

struct LSG_ConstDefaultColor
{
	static inline const SDL_Color Background = { 245, 245, 245, 255 };
	static inline const SDL_Color Border     = { 0, 0, 0, 255 };
	static inline const SDL_Color Text       = { 0, 0, 0, 255 };
};

struct LSG_ConstClickTime
{
	static inline const int DoubleClick = 300;
	static inline const int RightClick  = 1000;
};

struct LSG_ConstOrientation
{
	static inline const std::string Horizontal = "horizontal";
	static inline const std::string Vertical   = "vertical";

	static inline const std::string ToString(LSG_Orientation orientationEnum)
	{
		return (orientationEnum == LSG_ORIENTATION_VERTICAL ? Vertical : Horizontal);
	}
};

struct LSG_ConstSortOrder
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

struct LSG_ConstTexture
{
	static inline const int MaxSize = 8192;
};

struct LSG_ConstUnicodeCharacter
{
	static inline const char ArrowUp[4]   = { (char)0xE2, (char)0x86, (char)0x91, 0 };
	static inline const char ArrowDown[4] = { (char)0xE2, (char)0x86, (char)0x93, 0 };
	static inline const char Checkmark[4] = { (char)0xE2, (char)0x9C, (char)0x93, 0 };
};

struct LSG_Alignment
{
	LSG_HAlign halign = LSG_HALIGN_LEFT;
	LSG_VAlign valign = LSG_VALIGN_TOP;
};

struct LSG_ImageOrientation
{
	SDL_RendererFlip flip     = SDL_FLIP_NONE;
	double           rotation = 0.0;
};

const char* LSG_GetBasePath();

#if defined _android
	#include "LSG_AndroidJNI.h"
#endif

#include "LSG_IEvent.h"
#include "LSG_IRenderable.h"

#include "LSG_Graphics.h"
#include "LSG_Pagination.h"
#include "LSG_ScrollBar.h"
#include "LSG_Component.h"
#include "LSG_Text.h"

#include "LSG_Button.h"
#include "LSG_Events.h"
#include "LSG_Image.h"
#include "LSG_Line.h"
#include "LSG_List.h"
#include "LSG_Menu.h"
#include "LSG_MenuItem.h"
#include "LSG_MenuSub.h"
#include "LSG_Modal.h"
#include "LSG_Panel.h"
#include "LSG_ProgressBar.h"
#include "LSG_Slider.h"
#include "LSG_Table.h"
#include "LSG_TextInput.h"
#include "LSG_TextLabel.h"
#include "LSG_UI.h"
#include "LSG_Window.h"
#include "LSG_XML.h"

#endif
