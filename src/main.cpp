#include "main.h"

const char ERROR_NOT_STARTED[] = "libsdl2gui has not been started, call LSG_Start.";

char* basePath  = nullptr;
bool  isRunning = false;

const char* LSG_GetBasePath()
{
	return basePath;
}

static std::string getErrorNoID(const std::string& component, const std::string& id)
{
	return LSG_Text::Format("Failed to find a %s component with ID '%s'.", component.c_str(), id.c_str());
}

#if defined _android
/**
 * @throws runtime_error
 */
static void initBasePath()
{
	if (basePath)
		return;

	basePath = SDL_GetPrefPath(nullptr, nullptr);

	if (!basePath)
		throw std::runtime_error("Failed to get an app-specific location where files can be written.");

	auto jniAssetManager = LSG_AndroidJNI::GetAssetManager();
	auto dirs            = { "img", "ui" };

	for (auto dir : dirs)
	{
		auto dirPath = LSG_Text::Format("%s%s", basePath, dir);
		auto result  = mkdir(dirPath.c_str(), (S_IRWXU | S_IRWXG));

		if ((result != 0) && (errno != EEXIST))
			throw std::runtime_error(LSG_Text::Format("Failed to create asset directory '%s': %s", dirPath.c_str(), std::strerror(errno)));

		auto        assetDir  = AAssetManager_openDir(jniAssetManager, dir);
		const char* assetFile = nullptr;

		while ((assetFile = AAssetDir_getNextFileName(assetDir)))
		{
			auto sourcePath  = LSG_Text::Format("%s/%s", dir, assetFile);
			auto sourceAsset = AAssetManager_open(jniAssetManager, sourcePath.c_str(), AASSET_MODE_STREAMING);

			if (!sourceAsset)
				throw std::runtime_error(LSG_Text::Format("Failed to open asset: %s", sourcePath.c_str()));

			auto destinationPath = LSG_Text::Format("%s%s", basePath, sourcePath.c_str());
			auto destinationFile = SDL_RWFromFile(destinationPath.c_str(), "w");

			if (!destinationFile)
				throw std::runtime_error(LSG_Text::Format("Failed to write file '%s': %s", destinationPath.c_str(), SDL_GetError()));

			char destinationBuffer[BUFSIZ] = {};
			int  fileReadSize = 0;

			while ((fileReadSize = AAsset_read(sourceAsset, destinationBuffer, BUFSIZ)) > 0)
				SDL_RWwrite(destinationFile, destinationBuffer, fileReadSize, 1);

			SDL_RWclose(destinationFile);
			AAsset_close(sourceAsset);
		}

		AAssetDir_close(assetDir);
	}
}
#else
static void initBasePath()
{
	basePath = SDL_GetBasePath();
}
#endif

static SDL_Renderer* init(const std::string& title, int width, int height)
{
	if (isRunning)
		LSG_Quit();

	#if defined _macosx
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
	#elif defined _linux
		SDL_setenv("SDL_VIDEO_X11_LEGACY_FULLSCREEN", "0", 1);

		if (!std::getenv("DISPLAY"))
			SDL_setenv("DISPLAY", ":0", 1);
	#elif defined _windows
		#if (WINVER >= 0x0605)
			SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		#else
			SetProcessDPIAware();
		#endif
	#endif
	
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS,           "0");
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,           "0");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,         "2");
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED,       "0");
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

	if ((SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) || (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0))
		throw std::runtime_error(LSG_Text::Format("Failed to initialize SDL2: %s", SDL_GetError()));

	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

	if (IMG_Init(IMG_INIT_PNG) < IMG_INIT_PNG)
		throw std::runtime_error(LSG_Text::Format("Failed to initialize SDL2_image: %s", IMG_GetError()));

	if (TTF_Init() < 0)
		throw std::runtime_error(LSG_Text::Format("Failed to initialize SDL2_ttf: %s", TTF_GetError()));

	auto renderer = LSG_Window::Open(title, width, height);

	SDL_StopTextInput();

	isRunning = true;

	return renderer;
}

void LSG_AddListItem(const std::string& id, const std::string& item)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->AddItem(item);
}

void LSG_AddPanelButton(const std::string& id, const LSG_ButtonItem& button)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsPanel())
		throw std::invalid_argument(getErrorNoID("<panel>", id));

	static_cast<LSG_Panel*>(component)->AddButton(button);

	LSG_UI::LayoutParent(component);
	LSG_UI::SetText(component);
}

void LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSubMenu())
		throw std::invalid_argument(getErrorNoID("<menu-sub>", id));

	static_cast<LSG_MenuSub*>(component)->AddItem(item, itemId);
}

void LSG_AddTableGroup(const std::string& id, const LSG_TableGroup& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->AddGroup(group);
}

void LSG_AddTableRow(const std::string& id, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->AddRow(columns);
}

void LSG_AddTile(const std::string& id, const LSG_TileItem& tile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->AddTile(tile);
}

void LSG_ClearTextInput(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	static_cast<LSG_TextInput*>(component)->Clear();
}

SDL_Color LSG_GetBackgroundColor(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->backgroundColor;
}

std::string LSG_GetColorTheme()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_UI::GetColorTheme();
}

LSG_ExifData LSG_GetImageExif(const std::string& filePath)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	if (filePath.empty())
		throw std::invalid_argument("filePath cannot be empty.");

	return LSG_Exif::Get(filePath);
}

LSG_ImageOrientation LSG_GetImageOrientation(const LSG_ExifTags& tags)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Exif::GetOrientation(tags);
}

SDL_Surface* LSG_GetImageThumbnail(const std::string& filePath, const SDL_Size& maxSize)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	if (filePath.empty())
		throw std::invalid_argument("filePath cannot be empty.");

	return LSG_Graphics::GetThumbnail(filePath, maxSize);
}

int LSG_GetLastPage(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetLastPage();
}

std::string LSG_GetListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetItem(row);
}

size_t LSG_GetListItemCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetRowCount();
}

LSG_Strings LSG_GetListItems(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetItems();
}

int LSG_GetMargin(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->margin;
}

size_t LSG_GetNavigationItemCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	return static_cast<LSG_Navigation*>(component)->GetItemsTotal();
}

int LSG_GetNavigationPosition(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	return static_cast<LSG_Navigation*>(component)->GetPosition();
}

int LSG_GetPadding(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->padding;
}

int LSG_GetPage(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetPage();
}

std::string LSG_GetPageListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetPageItem(row);
}

LSG_Strings LSG_GetPageListItems(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetPageItems();
}

LSG_TableGroups LSG_GetPageTableGroups(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageGroups();
}

LSG_Strings LSG_GetPageTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageRow(row);
}

LSG_TableRows LSG_GetPageTableRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageRows();
}

SDL_Point LSG_GetPosition(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	SDL_Point position = { component->background.x, component->background.y };

	return position;
}

double LSG_GetProgressValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsProgressBar())
		throw std::invalid_argument(getErrorNoID("<progress-bar>", id));

	auto value = static_cast<LSG_ProgressBar*>(component)->GetValue();

	return value;
}

int LSG_GetScrollHorizontal(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsList())
		return static_cast<LSG_List*>(component)->GetScrollX();
	else if (component->IsPanel())
		return static_cast<LSG_Panel*>(component)->GetScrollX();
	else if (component->IsTable())
		return static_cast<LSG_Table*>(component)->GetScrollX();
	else if (component->IsTextLabel())
		return static_cast<LSG_TextLabel*>(component)->GetScrollX();
	else if (component->IsTiles())
		return static_cast<LSG_Tiles*>(component)->GetScrollX();

	return 0;
}

int LSG_GetScrollVertical(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table> or <text><table>, <text> or <tiles>", id));

	if (component->IsList())
		return static_cast<LSG_List*>(component)->GetScrollY();
	else if (component->IsPanel())
		return static_cast<LSG_Panel*>(component)->GetScrollY();
	else if (component->IsTable())
		return static_cast<LSG_Table*>(component)->GetScrollY();
	else if (component->IsTextLabel())
		return static_cast<LSG_TextLabel*>(component)->GetScrollY();
	else if (component->IsTiles())
		return static_cast<LSG_Tiles*>(component)->GetScrollY();

	return 0;
}

std::vector<int> LSG_GetSelectedRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetSelectedRows();
}

std::vector<int> LSG_GetSelectedTiles(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetSelectedTiles();
}

SDL_Size LSG_GetSize(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	SDL_Size size = { component->background.w, component->background.h };

	return size;
}

double LSG_GetSliderValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	auto value = static_cast<LSG_Slider*>(component)->GetValue();

	return value;
}

int LSG_GetSortColumn(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetSortColumn();
}

LSG_SortOrder LSG_GetSortOrder(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetSortOrder();
}

int LSG_GetSpacing(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->GetSpacing();
}

int LSG_GetTableColumnWidth(const std::string& id, int column)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetColumnWidth(column);
}

LSG_TableGroup LSG_GetTableGroup(const std::string& id, const std::string& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetGroup(group);
}

LSG_TableGroups LSG_GetTableGroups(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetGroups();
}

LSG_Strings LSG_GetTableHeader(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetHeader();
}

LSG_Strings LSG_GetTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRow(row);
}

size_t LSG_GetTableRowCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRowCount();
}

LSG_TableRows LSG_GetTableRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRows();
}

std::string LSG_GetText(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id));

	return component->text;
}

std::string LSG_GetTextInputValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	return static_cast<LSG_TextInput*>(component)->GetValue();
}

LSG_TileItem LSG_GetTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTile(index);
}

LSG_TileItems LSG_GetTiles(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTiles();
}

size_t LSG_GetTilesCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTilesCount();
}

std::string LSG_GetTitle(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsModal() && !component->IsMenu() && !component->IsSubMenu()))
		throw std::invalid_argument(getErrorNoID("<modal>, <menu> or <menu-sub>", id));

	return LSG_XML::GetAttribute(component->GetXmlNode(), "title");
}

SDL_Size LSG_GetWindowMinimumSize()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::GetMinimumSize();
}

SDL_Point LSG_GetWindowPosition()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::GetPosition();
}

SDL_Size LSG_GetWindowSize()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::GetSize();
}

std::string LSG_GetWindowTitle()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::GetTitle();
}

bool LSG_IsEnabled(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->enabled;
}

bool LSG_IsMenuItemSelected(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	return static_cast<LSG_MenuItem*>(component)->IsSelected();
}

bool LSG_IsMenuOpen(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenu())
		throw std::invalid_argument(getErrorNoID("<menu>", id));

	return static_cast<LSG_Menu*>(component)->IsOpen();
}

bool LSG_IsPreferredDarkMode()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_UI::IsDarkMode();
}

bool LSG_IsRunning()
{
	return isRunning;
}

bool LSG_IsToggledOn(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsToggle())
		throw std::invalid_argument(getErrorNoID("<toggle>", id));

	return static_cast<LSG_Toggle*>(component)->IsOn();
}

bool LSG_IsVisible(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->visible;
}

bool LSG_IsWindowMaximized()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::IsMaximized();
}

void LSG_Layout()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_UI::LayoutRoot();
}

void LSG_NavigateBack(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateBack(text);
}

void LSG_NavigateEnd(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateEnd(text);
}

void LSG_NavigateForward(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateForward(text);
}

void LSG_NavigateHome(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateHome(text);
}

void LSG_NavigateTo(const std::string& id, int position, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateTo(position, text);
}

#if defined _windows
std::string LSG_OpenFile(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto filePathWide = LSG_Window::OpenFile(filters);
	auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());
	auto filePath     = std::string(filePathUTF8);

	SDL_free(filePathUTF8);

	return filePath;
}
#elif defined _android || defined _linux || defined _macosx
std::string LSG_OpenFile(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::OpenFile(filters);
}
#endif

#if defined _windows
LSG_Strings LSG_OpenFiles(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Strings filePaths;

	auto filePathsWide = LSG_Window::OpenFiles(filters);

	for (const auto& filePathWide : filePathsWide)
	{
		auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());

		filePaths.push_back(std::string(filePathUTF8));

		SDL_free(filePathUTF8);
	}

	return filePaths;
}
#elif defined _linux || defined _macosx
LSG_Strings LSG_OpenFiles(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::OpenFiles(filters);
}
#endif

#if defined _android || defined _linux || defined _macosx || defined _windows
std::string LSG_OpenFolder()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

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
#endif

#if defined _linux || defined _macosx || defined _windows
LSG_Strings LSG_OpenFolders()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	#if defined _windows
		LSG_Strings folderPaths;

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
#endif

#if defined _ios
void LSG_OpenFile(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileDocuments(resultsCallback, false);
}
#endif

#if defined _ios
void LSG_OpenFiles(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileDocuments(resultsCallback, true);
}
#endif

#if defined _ios
void LSG_OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFolder(resultsCallback);
}
#endif

#if defined _ios
void LSG_OpenMediaFile(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileMedia(resultsCallback, false);
}
#endif

#if defined _ios
void LSG_OpenMediaFiles(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileMedia(resultsCallback, true);
}
#endif

#if defined _ios
void LSG_OpenPhotoFile(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFilePhotos(resultsCallback, false);
}
#endif

#if defined _ios
void LSG_OpenPhotoFiles(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFilePhotos(resultsCallback, true);
}
#endif

void LSG_Present()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

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
	SDL_Quit();
}

void LSG_RemoveListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->RemoveItem(row);
}

void LSG_RemoveMenuItem(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	LSG_UI::RemoveXmlNode(component);
}

void LSG_RemovePageListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->RemovePageItem(row);
}

void LSG_RemovePageTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemovePageRow(row);
}

void LSG_RemoveTableHeader(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveHeader();
}

void LSG_RemoveTableGroup(const std::string& id, const std::string& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveGroup(group);
}

void LSG_RemoveTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveRow(row);
}

void LSG_RemoveTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->RemoveTile(index);
}

std::vector<SDL_Event> LSG_Run()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto events = LSG_Events::Handle();

	LSG_Window::Render();

	return events;
}

#if defined _windows
std::string LSG_SaveFile(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto filePathWide = LSG_Window::SaveFile(filters);
	auto filePathUTF8 = SDL_iconv_wchar_utf8(filePathWide.c_str());
	auto filePath     = std::string(filePathUTF8);

	SDL_free(filePathUTF8);

	return filePath;
}
#elif defined _android || defined _linux || defined _macosx
std::string LSG_SaveFile(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::SaveFile(filters);
}
#endif

void LSG_ScrollHorizontal(const std::string& id, int scroll)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsScrollable()))
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollHorizontal(scroll, true);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollHorizontal(scroll, true);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollHorizontal(scroll, true);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollHorizontal(scroll, true);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollHorizontal(scroll, true);
}

void LSG_ScrollVertical(const std::string& id, int scroll)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollVertical(scroll, true);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollVertical(scroll, true);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollVertical(scroll, true);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollVertical(scroll, true);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollVertical(scroll, true);
}

void LSG_ScrollToBottom(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollEnd();
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollEnd();
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollEnd();
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollEnd();
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollEnd();
}

void LSG_ScrollToTop(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollHome();
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollHome();
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollHome();
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollHome();
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollHome();
}

void LSG_SelectRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->Select(row);
}

void LSG_SelectRowByOffset(const std::string& id, int offset)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->SelectRow(offset);
}

void LSG_SelectRows(const std::string& id, const std::vector<int>& rows)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->Select(rows);
}

void LSG_SelectTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->Select(index);
}

void LSG_SelectTiles(const std::string& id, const std::vector<int>& indices)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->Select(indices);
}

void LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetAlignmentHorizontal(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetAlignmentVertical(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBackgroundColor(color);
}

void LSG_SetBorder(const std::string& id, int border)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBorder(border);

	LSG_UI::LayoutParent(component);
}

void LSG_SetBorderColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBorderColor(color);
}

void LSG_SetButtonSelected(const std::string& id, bool selected)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsButton())
		throw std::invalid_argument(getErrorNoID("<button>", id));

	static_cast<LSG_Button*>(component)->SetSelected(selected);
}

void LSG_SetColorTheme(const std::string& colorThemeFile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_UI::SetColorTheme(colorThemeFile);
}

void LSG_SetEnabled(const std::string& id, bool enabled)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetEnabled(component, enabled);
}

void LSG_SetFontSize(const std::string& id, int size)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetFontSize(component, size);
	LSG_UI::LayoutParent(component);
	LSG_UI::SetText(component);
}

void LSG_SetFontStyle(const std::string& id, int style)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetFontStyle(component, style);
	LSG_UI::LayoutParent(component);
	LSG_UI::SetText(component);
}

void LSG_SetHeight(const std::string& id, int height, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "height", std::to_string(height));

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetHeight(const std::string& id, double percent, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto height = LSG_Text::Format("%d%%", (int)std::ceil(std::max(0.0, std::min(1.0, percent)) * 100.0));

	LSG_XML::SetAttribute(component->GetXmlNode(), "height", height);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetImage(const std::string& id, const std::string& file, bool fill)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsImage())
		throw std::invalid_argument(getErrorNoID("<image>", id));

	static_cast<LSG_Image*>(component)->SetImage(file, fill);
}

void LSG_SetListItem(const std::string& id, int row, const std::string& item)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetItem(row, item);
}

void LSG_SetListItems(const std::string& id, const LSG_Strings& items)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetItems(items);
}

void LSG_SetMargin(const std::string& id, int margin)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetMargin(margin);

	LSG_UI::LayoutParent(component);
}

void LSG_SetMenuItemIcon(const std::string& id, const std::string& imageFile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "icon", imageFile);

	static_cast<LSG_MenuItem*>(component)->SetMenuItem(component->background);
}

void LSG_SetMenuItemSelected(const std::string& id, bool selected)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	static_cast<LSG_MenuItem*>(component)->SetSelected(selected);
}

void LSG_SetMenuItemValue(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	component->text = value;
}

void LSG_SetNavigationItemCount(const std::string& id, size_t itemsTotal, size_t itemsPerNavigation)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->Set(itemsTotal, itemsPerNavigation);
}

void LSG_SetOrientation(const std::string& id, LSG_Orientation orientation, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetOrientation(orientation);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetPadding(const std::string& id, int padding)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetPadding(padding);

	LSG_UI::LayoutParent(component);
}

void LSG_SetPanelButtons(const std::string& id, const LSG_Buttons& buttons)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsPanel())
		throw std::invalid_argument(getErrorNoID("<panel>", id));

	static_cast<LSG_Panel*>(component)->SetButtons(buttons);

	LSG_UI::LayoutParent(component);
	LSG_UI::SetText(component);
}

void LSG_SetPage(const std::string& id, int page)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	if (component->IsList())
		static_cast<LSG_List*>(component)->SetPage(page);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->SetPage(page);
}

void LSG_SetPageListItem(const std::string& id, int row, const std::string& item)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetPageItem(row, item);
}

void LSG_SetPageTableRow(const std::string& id, int row, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetPageRow(row, columns);
}

void LSG_SetProgressValue(const std::string& id, double percent)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsProgressBar())
		throw std::invalid_argument(getErrorNoID("<progress-bar>", id));

	static_cast<LSG_ProgressBar*>(component)->SetValue(percent);
}

void LSG_SetSize(const std::string& id, const SDL_Size& size, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "width",  std::to_string(size.width));
	LSG_XML::SetAttribute(component->GetXmlNode(), "height", std::to_string(size.height));

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetSize(const std::string& id, double width, double height, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto w = LSG_Text::Format("%d%%", (int)std::ceil(std::max(0.0, std::min(1.0, width))  * 100.0));
	auto h = LSG_Text::Format("%d%%", (int)std::ceil(std::max(0.0, std::min(1.0, height)) * 100.0));

	LSG_XML::SetAttribute(component->GetXmlNode(), "width",  w);
	LSG_XML::SetAttribute(component->GetXmlNode(), "height", h);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetSliderValue(const std::string& id, double percent)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	static_cast<LSG_Slider*>(component)->SetValue(percent);
}

void LSG_SetSpacing(const std::string& id, int spacing)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetSpacing(spacing);

	LSG_UI::LayoutParent(component);
}

void LSG_SetTableColumnWidth(const std::string& id, int column, int width)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetColumnWidth(column, width);
}

void LSG_SetTableGroup(const std::string& id, const LSG_TableGroup& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetGroup(group);
}

void LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetGroups(groups);
}

void LSG_SetTableHeader(const std::string& id, const LSG_Strings& header)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetHeader(header);
}

void LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetRow(row, columns);
}

void LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetRows(rows);
}

void LSG_SetText(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id));

	static_cast<LSG_TextLabel*>(component)->SetText(value);

	LSG_UI::LayoutParent(component);
}

void LSG_SetTextColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetTextColor(component, color);
	LSG_UI::SetText(component);
}

void LSG_SetTextInputValue(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	static_cast<LSG_TextInput*>(component)->SetValue(value);
}

void LSG_SetTile(const std::string& id, int index, const LSG_TileItem& tile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->SetTile(index, tile);
}

void LSG_SetTiles(const std::string& id, const LSG_TileItems& tiles)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->SetTiles(tiles);
}

void LSG_SetTitle(const std::string& id, const std::string& title)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsModal() && !component->IsMenu() && !component->IsSubMenu()))
		throw std::invalid_argument(getErrorNoID("<modal>, <menu> or <menu-sub>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "title", title);

	if (component->IsModal())
		static_cast<LSG_Modal*>(component)->Update();
	else if (component->IsMenu())
		static_cast<LSG_Menu*>(component)->SetMenu();
	else if (component->IsSubMenu())
		static_cast<LSG_MenuSub*>(component)->SetSubMenu(component->background);
}

void LSG_SetToggle(const std::string& id, bool on)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsToggle())
		throw std::invalid_argument(getErrorNoID("<toggle>", id));

	static_cast<LSG_Toggle*>(component)->Set(on);
}

void LSG_SetVisible(const std::string& id, bool visible, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetVisible(visible);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetWidth(const std::string& id, int width, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "width", std::to_string(width));

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetWidth(const std::string& id, double percent, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto width = LSG_Text::Format("%d%%", (int)std::ceil(std::max(0.0, std::min(1.0, percent)) * 100.0));

	LSG_XML::SetAttribute(component->GetXmlNode(), "width", width);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetWindowMaximized(bool maximized)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::SetMaximized(maximized);
}

void LSG_SetWindowMinimumSize(int width, int height)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::SetMinimumSize(width, height);
}

void LSG_SetWindowPosition(int x, int y)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::SetPosition(x, y);
}

void LSG_SetWindowSize(int width, int height)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::SetSize(width, height);
}

void LSG_SetWindowTitle(const std::string& title)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::SetTitle(title);
}

void LSG_ShowColumnBorder(const std::string& id, bool show)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "show-column-border", (show ? "true" : "false"));
}

void LSG_ShowError(const std::string& message)
{
	LSG_Window::ShowMessage(message);
}

void LSG_ShowRowBorder(const std::string& id, bool show)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "show-row-border", (show ? "true" : "false"));
}

void LSG_SortList(const std::string& id, LSG_SortOrder sortOrder)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->Sort(sortOrder);
}

void LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = LSG_UI::GetComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->Sort(sortOrder, sortColumn);
}

SDL_Renderer* LSG_Start(const std::string& xmlFile)
{
	LibXml::xmlInitParser();

	if (!basePath)
		initBasePath();

	auto windowAttribs = LSG_UI::OpenWindow(xmlFile);

	auto title  = (windowAttribs.contains("title")  ? windowAttribs["title"] : "");
	auto width  = (windowAttribs.contains("width")  ? std::atoi(windowAttribs["width"].c_str())  : 0);
	auto height = (windowAttribs.contains("height") ? std::atoi(windowAttribs["height"].c_str()) : 0);

	auto renderer = init(title, width, height);

	auto minWidth  = (windowAttribs.contains("min-width")  ? std::atoi(windowAttribs["min-width"].c_str())  : 0);
	auto minHeight = (windowAttribs.contains("min-height") ? std::atoi(windowAttribs["min-height"].c_str()) : 0);

	if ((minWidth > 0) && (minHeight > 0))
		LSG_SetWindowMinimumSize(minWidth, minHeight);
	else if (minWidth > 0)
		LSG_SetWindowMinimumSize(minWidth, LSG_Window::MinSize);
	else if (minHeight > 0)
		LSG_SetWindowMinimumSize(LSG_Window::MinSize, minHeight);

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

#if defined _windows && defined _DEBUG
void LSG_StartTest(const std::string& xmlFile, const std::string& workingDir)
{
	LibXml::xmlInitParser();

	if (!basePath)
		basePath = (char*)workingDir.c_str();

	auto windowAttribs = LSG_UI::OpenWindow(xmlFile);

	if (IMG_Init(IMG_INIT_PNG) < IMG_INIT_PNG)
		throw std::runtime_error(LSG_Text::Format("Failed to initialize SDL2_image: %s", IMG_GetError()));

	if (TTF_Init() < 0)
		throw std::runtime_error(LSG_Text::Format("Failed to initialize SDL2_ttf: %s", TTF_GetError()));

	LSG_Window::OpenTest();

	auto colorThemeFile = (windowAttribs.contains("color-theme-file") ? windowAttribs["color-theme-file"] : "");

	LSG_UI::Load(colorThemeFile);

	isRunning = true;
}
#endif

#if defined _windows
static BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    return TRUE;
}
#else
static int entry()
{
	return 0;
}
#endif
