#include "main.h"

const char ERROR_NOT_STARTED[] = "libsdlui has not been started, call LSG_Start.";

std::string basePath  = "";
bool        isRunning = false;

std::string LSG_GetBasePath()
{
	return basePath;
}

static LSG_Component* getComponent(const std::string& id)
{
	auto component = LSG_UI::GetComponent(id);

	if (!component)
		component = LSG_UI::GetComponentInModal(id);

	return component;
}

static std::string getErrorNoID(const std::string& component, const std::string& id)
{
	return std::format("Failed to find a {} component with ID '{}'.", component, id);
}

#if defined _android
/**
 * @throws runtime_error
 */
static void initBasePath()
{
	if (!basePath.empty())
		return;

	auto path = SDL_GetAndroidInternalStoragePath();

	if (!path)
		throw std::runtime_error(std::format("Failed to get an app-specific location where files can be written: {}", SDL_GetError()));

	basePath = std::format("{}/", path);

	auto jniAssetManager = LSG_AndroidJNI::GetAssetManager();
	auto dirs            = { "fonts", "img", "ui" };

	for (auto dir : dirs)
	{
		auto dirPath = std::format("{}{}", basePath, dir);
		auto result  = mkdir(dirPath.c_str(), (S_IRWXU | S_IRWXG));

		if ((result != 0) && (errno != EEXIST))
			throw std::runtime_error(std::format("Failed to create asset directory '{}': {}", dirPath, std::strerror(errno)));

		auto        assetDir  = AAssetManager_openDir(jniAssetManager, dir);
		const char* assetFile = nullptr;

		while ((assetFile = AAssetDir_getNextFileName(assetDir)))
		{
			auto sourcePath  = std::format("{}/{}", dir, assetFile);
			auto sourceAsset = AAssetManager_open(jniAssetManager, sourcePath.c_str(), AASSET_MODE_STREAMING);

			if (!sourceAsset)
				throw std::runtime_error(std::format("Failed to open asset: {}", sourcePath));

			auto destinationPath = std::format("{}{}", basePath, sourcePath);
			auto destinationFile = SDL_IOFromFile(destinationPath.c_str(), "w");

			if (!destinationFile)
				throw std::runtime_error(std::format("Failed to write file '{}': {}", destinationPath, SDL_GetError()));

			char destinationBuffer[BUFSIZ] = {};
			int  fileReadSize = 0;

			while ((fileReadSize = AAsset_read(sourceAsset, destinationBuffer, BUFSIZ)) > 0)
				SDL_WriteIO(destinationFile, destinationBuffer, fileReadSize);

			SDL_CloseIO(destinationFile);
			AAsset_close(sourceAsset);
		}

		AAssetDir_close(assetDir);
	}
}
#else
static void initBasePath()
{
	if (!basePath.empty())
		return;

	auto path = SDL_GetBasePath();

	if (!path)
		throw std::runtime_error(std::format("Failed to get an app-specific location where files can be written: {}", SDL_GetError()));

	basePath = std::string(path);
}
#endif

static SDL_Renderer* init(const std::string& title, int width, int height)
{
	if (isRunning)
		LSG_Quit();

    #if defined _android
        SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "0");
    #elif defined _macosx
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
	#elif defined _linux
		SDL_setenv_unsafe("SDL_VIDEO_X11_LEGACY_FULLSCREEN", "0", 1);

		if (!std::getenv("DISPLAY"))
			SDL_setenv_unsafe("DISPLAY", ":0", 1);
	#elif defined _windows
		#if (WINVER >= 0x0605)
			SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		#else
			SetProcessDPIAware();
		#endif
	#endif
	
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS,           "0");
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,           "0");
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO) || !SDL_InitSubSystem(SDL_INIT_EVENTS))
		throw std::runtime_error(std::format("Failed to initialize SDL: {}", SDL_GetError()));

	if (!TTF_Init())
		throw std::runtime_error(std::format("Failed to initialize SDL_ttf: {}", SDL_GetError()));

	auto renderer = LSG_Window::Open(title, width, height);

	SDL_StopTextInput(SDL_GetRenderWindow(renderer));

	#if defined _android
		LSG_Window::InitJNI();
	#endif

	isRunning = true;

	return renderer;
}

void LSG_AddListItem(const std::string& id, const std::string& item)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->AddItem(item);
}

void LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSubMenu())
		throw std::invalid_argument(getErrorNoID("<menu-sub>", id));

	static_cast<LSG_MenuSub*>(component)->AddItem(item, itemId);
}

void LSG_AddTableGroup(const std::string& id, const LSG_TableGroup& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->AddGroup(group);
}

void LSG_AddTableRow(const std::string& id, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->AddRow(columns);
}

void LSG_AddTile(const std::string& id, const LSG_TileItem& tile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->AddTile(tile);
}

void LSG_AddCard(const std::string& id, const LSG_CardItem& card)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->AddCard(card);
}

void LSG_ClearTextInput(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	static_cast<LSG_TextInput*>(component)->Clear();
}

void LSG_CloseModal(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsModal())
		throw std::invalid_argument(getErrorNoID("<modal>", id));

	static_cast<LSG_Modal*>(component)->Close();
}

SDL_Color LSG_GetBackgroundColor(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->backgroundColor;
}

std::string LSG_GetButtonIconPath(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsButton())
		throw std::invalid_argument(getErrorNoID("<button>", id));

	return static_cast<LSG_Button*>(component)->GetIconPath();
}

std::string LSG_GetButtonText(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsButton())
		throw std::invalid_argument(getErrorNoID("<button>", id));

	return static_cast<LSG_Button*>(component)->GetText();
}

LSG_CardItem LSG_GetCard(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	return static_cast<LSG_Cards*>(component)->GetCard(index);
}

LSG_CardItems LSG_GetCards(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	return static_cast<LSG_Cards*>(component)->GetCards();
}

size_t LSG_GetCardsCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	return static_cast<LSG_Cards*>(component)->GetCardsCount();
}

std::string LSG_GetColorTheme()
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_UI::GetColorTheme();
}

int LSG_GetDPIScaled(int value, bool inverse)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::GetDPIScaled(value, inverse);
}

LSG_File LSG_GetFile(const std::string& filePath)
{
    LSG_File file = { .filePath = filePath };

	auto lastSeparator = file.filePath.rfind('/');

    if (lastSeparator == std::string::npos)
        lastSeparator = file.filePath.rfind('\\');

    if (lastSeparator != std::string::npos) {
	    file.pathSep = file.filePath[lastSeparator];
	    file.path    = file.filePath.substr(0, lastSeparator);
        file.file    = file.filePath.substr(lastSeparator + 1);
    } else {
        file.file = file.filePath;
    }

    auto extension = file.file.rfind('.');

    if (extension != std::string::npos) {
        file.name = file.file.substr(0, extension);
        file.ext  = LSG_TextToLower(file.file.substr(extension + 1));
    } else {
        file.name = file.file;
    }

    return file;
}

int LSG_GetFontStyle(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->GetFontStyle();
}

LSG_ExifData LSG_GetImageExif(const std::string& filePath)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	if (filePath.empty())
		throw std::invalid_argument("filePath cannot be empty.");

	return LSG_Exif::Get(filePath);
}

LSG_GPS LSG_GetImageGPS(const LSG_ExifTags& gps)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Exif::GetGPS(gps);
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

SDL_Surface* LSG_GetImageThumbnail(SDL_Surface* surface, const SDL_Size& maxSize)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	if (!surface)
		throw std::invalid_argument("surface cannot be null.");

	return LSG_Graphics::GetThumbnail(surface, maxSize);
}

int LSG_GetLastPage(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetLastPage();
}

std::string LSG_GetListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetItem(row);
}

size_t LSG_GetListItemCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetRowCount();
}

LSG_Strings LSG_GetListItems(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetItems();
}

int LSG_GetMargin(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->margin;
}

size_t LSG_GetNavigationItemCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	return static_cast<LSG_Navigation*>(component)->GetItemsTotal();
}

int LSG_GetNavigationPosition(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	return static_cast<LSG_Navigation*>(component)->GetPosition();
}

LSG_Orientation LSG_GetOrientation(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->GetOrientation();
}

int LSG_GetPadding(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->padding;
}

int LSG_GetPage(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetPage();
}

std::string LSG_GetPageListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetPageItem(row);
}

LSG_Strings LSG_GetPageListItems(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	return static_cast<LSG_List*>(component)->GetPageItems();
}

LSG_TableGroups LSG_GetPageTableGroups(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageGroups();
}

LSG_Strings LSG_GetPageTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageRow(row);
}

LSG_TableRows LSG_GetPageTableRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetPageRows();
}

SDL_Point LSG_GetPosition(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	SDL_Point position = { component->background.x, component->background.y };

	return position;
}

double LSG_GetProgressValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsProgressBar())
		throw std::invalid_argument(getErrorNoID("<progress-bar>", id));

	auto value = static_cast<LSG_ProgressBar*>(component)->GetValue();

	return value;
}

int LSG_GetScrollHorizontal(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		return static_cast<LSG_Cards*>(component)->GetScrollHorizontal();
	else if (component->IsList())
		return static_cast<LSG_List*>(component)->GetScrollHorizontal();
	else if (component->IsPanel())
		return static_cast<LSG_Panel*>(component)->GetScrollHorizontal();
	else if (component->IsTable())
		return static_cast<LSG_Table*>(component)->GetScrollHorizontal();
	else if (component->IsTextLabel())
		return static_cast<LSG_TextLabel*>(component)->GetScrollHorizontal();
	else if (component->IsTiles())
		return static_cast<LSG_Tiles*>(component)->GetScrollHorizontal();

	return 0;
}

int LSG_GetScrollVertical(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		return static_cast<LSG_Cards*>(component)->GetScrollVertical();
	else if (component->IsList())
		return static_cast<LSG_List*>(component)->GetScrollVertical();
	else if (component->IsPanel())
		return static_cast<LSG_Panel*>(component)->GetScrollVertical();
	else if (component->IsTable())
		return static_cast<LSG_Table*>(component)->GetScrollVertical();
	else if (component->IsTextLabel())
		return static_cast<LSG_TextLabel*>(component)->GetScrollVertical();
	else if (component->IsTiles())
		return static_cast<LSG_Tiles*>(component)->GetScrollVertical();

	return 0;
}

std::vector<int> LSG_GetSelectedCards(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	return static_cast<LSG_Cards*>(component)->GetSelectedCards();
}

std::vector<int> LSG_GetSelectedRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetSelectedRows();
}

std::vector<int> LSG_GetSelectedTiles(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetSelectedTiles();
}

SDL_Size LSG_GetSize(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	SDL_Size size = { component->background.w, component->background.h };

	return size;
}

LSG_SliderParts LSG_GetSliderParts(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	auto parts = static_cast<LSG_Slider*>(component)->GetParts();

	return parts;
}

double LSG_GetSliderValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	auto value = static_cast<LSG_ProgressBar*>(component)->GetValue();

	return value;
}

double LSG_GetSliderValue(const std::string& id, const SDL_Point& mousePosition)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	auto value = static_cast<LSG_Slider*>(component)->GetValue(mousePosition);

	return value;
}

int LSG_GetSortColumn(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetSortColumn();
}

LSG_SortOrder LSG_GetSortOrder(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	return static_cast<LSG_List*>(component)->GetSortOrder();
}

int LSG_GetSpacing(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->GetSpacing();
}

int LSG_GetTableColumnWidth(const std::string& id, int column)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetColumnWidth(column);
}

LSG_TableGroup LSG_GetTableGroup(const std::string& id, const std::string& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetGroup(group);
}

LSG_TableGroups LSG_GetTableGroups(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetGroups();
}

LSG_Strings LSG_GetTableHeader(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetHeader();
}

LSG_Strings LSG_GetTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRow(row);
}

size_t LSG_GetTableRowCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRowCount();
}

LSG_TableRows LSG_GetTableRows(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	return static_cast<LSG_Table*>(component)->GetRows();
}

std::string LSG_GetText(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id));

	return component->text;
}

std::string LSG_GetTextInputValue(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	return static_cast<LSG_TextInput*>(component)->GetValue();
}

LSG_TileItem LSG_GetTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTile(index);
}

LSG_TileItems LSG_GetTiles(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTiles();
}

size_t LSG_GetTilesCount(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	return static_cast<LSG_Tiles*>(component)->GetTilesCount();
}

std::string LSG_GetTitle(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsModal() && !component->IsMenu() && !component->IsSubMenu()))
		throw std::invalid_argument(getErrorNoID("<modal>, <menu> or <menu-sub>", id));

	return LSG_XML::GetAttribute(component->GetXmlNode(), "title");
}

std::string LSG_GetTooltip(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->GetTooltip();
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

	return LSG_Window::GetSizeInPixels();
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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	return component->enabled;
}

bool LSG_IsMenuItemSelected(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	return static_cast<LSG_MenuItem*>(component)->IsSelected();
}

bool LSG_IsMenuOpen(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component || !component->IsToggle())
		throw std::invalid_argument(getErrorNoID("<toggle>", id));

	return static_cast<LSG_Toggle*>(component)->IsOn();
}

bool LSG_IsVisible(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateBack(text);
}

void LSG_NavigateEnd(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateEnd(text);
}

void LSG_NavigateHome(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateHome(text);
}

void LSG_NavigateNext(const std::string& id, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->NavigateNext(text);
}

void LSG_NavigateTo(const std::string& id, int position, const std::string& text)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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
#elif defined _linux || defined _macosx
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

#if defined _linux || defined _macosx || defined _windows
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

#if defined _android
void LSG_OpenFile(std::function<void(const std::string&)> resultsCallback, const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::OpenFile(resultsCallback, filters);
}

void LSG_OpenFolder(std::function<void(const std::string&)> resultsCallback)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	LSG_Window::OpenFolder(resultsCallback);
}
#endif

#if defined _ios
void LSG_OpenFile(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileDocuments(resultsCallback, false);
}

void LSG_OpenFiles(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileDocuments(resultsCallback, true);
}

void LSG_OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFolder(resultsCallback);
}

void LSG_OpenMediaFile(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileMedia(resultsCallback, false);
}

void LSG_OpenMediaFiles(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFileMedia(resultsCallback, true);
}

void LSG_OpenPhotoFile(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFilePhotos(resultsCallback, false);
}

void LSG_OpenPhotoFiles(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback)
{
    if (!isRunning)
        throw std::runtime_error(ERROR_NOT_STARTED);

    LSG_Window::OpenFilePhotos(resultsCallback, true);
}
#endif

void LSG_OpenMenu(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenu())
		throw std::invalid_argument(getErrorNoID("<menu>", id));

	static_cast<LSG_Menu*>(component)->Open();
}

void LSG_OpenModal(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsModal())
		throw std::invalid_argument(getErrorNoID("<modal>", id));

	static_cast<LSG_Modal*>(component)->Open();
}

void LSG_OpenSubMenu(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSubMenu())
		throw std::invalid_argument(getErrorNoID("<menu-sub>", id));

	static_cast<LSG_MenuSub*>(component)->Open();
}

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

	LSG_Graphics::DestroyTextures();

	LSG_UI::Close();
	LSG_Window::Close();

	TTF_Quit();

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void LSG_RemoveCard(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->RemoveCard(row);
}

void LSG_RemoveListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->RemoveItem(row);
}

void LSG_RemoveMenuItem(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	LSG_UI::RemoveXmlNode(component);
}

void LSG_RemovePageListItem(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->RemovePageItem(row);
}

void LSG_RemovePageTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemovePageRow(row);
}

void LSG_RemoveTableHeader(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveHeader();
}

void LSG_RemoveTableGroup(const std::string& id, const std::string& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveGroup(group);
}

void LSG_RemoveTableRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->RemoveRow(row);
}

void LSG_RemoveTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->RemoveTile(index);
}

void LSG_RenderTextureWithRoundedCorners(
	SDL_Renderer*      renderer,
	SDL_Texture*       texture,
	const SDL_Rect&    destination,
	const SDL_Rect*    clip,
	int                radius,
	const SDL_Color&   color,
	const std::string& id
) {
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	if (!renderer)
		throw std::invalid_argument("'renderer' cannot be NULL");

	if (!texture)
		throw std::invalid_argument("'texture' cannot be NULL");

	LSG_Graphics::RenderTextureWithRoundedCorners(
		renderer,
		texture,
		destination,
		clip,
		LSG_Window::GetDPIScaled(radius),
		color,
		id
	);
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
#elif defined _linux || defined _macosx
std::string LSG_SaveFile(const LSG_Strings& filters)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	return LSG_Window::SaveFile(filters);
}
#endif

void LSG_ScrollByHorizontal(const std::string& id, int offset)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsScrollable()))
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->OnScrollHorizontal(offset, true);
	else if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollHorizontal(offset, true);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollHorizontal(offset, true);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollHorizontal(offset, true);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollHorizontal(offset, true);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollHorizontal(offset, true);
}

void LSG_ScrollByVertical(const std::string& id, int offset)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->OnScrollVertical(offset, true);
	else if (component->IsList())
		static_cast<LSG_List*>(component)->OnScrollVertical(offset, true);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->OnScrollVertical(offset, true);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->OnScrollVertical(offset, true);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->OnScrollVertical(offset, true);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OnScrollVertical(offset, true);
}

void LSG_ScrollToBottom(const std::string& id)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->OnScrollEnd();
	else if (component->IsList())
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

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->OnScrollHome();
	else if (component->IsList())
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

void LSG_ScrollToHorizontal(const std::string& id, int position)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsScrollable()))
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->ScrollToHorizontal(position);
	else if (component->IsList())
		static_cast<LSG_List*>(component)->ScrollToHorizontal(position);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->ScrollToHorizontal(position);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->ScrollToHorizontal(position);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->ScrollToHorizontal(position);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->ScrollToHorizontal(position);
}

void LSG_ScrollToVertical(const std::string& id, int position)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsScrollable())
		throw std::invalid_argument(getErrorNoID("<cards>, <list>, <panel>, <table>, <text> or <tiles>", id));

	if (component->IsCards())
		static_cast<LSG_Cards*>(component)->ScrollToVertical(position);
	else if (component->IsList())
		static_cast<LSG_List*>(component)->ScrollToVertical(position);
	else if (component->IsPanel())
		static_cast<LSG_Panel*>(component)->ScrollToVertical(position);
	else if (component->IsTable())
		static_cast<LSG_Table*>(component)->ScrollToVertical(position);
	else if (component->IsTextLabel())
		static_cast<LSG_TextLabel*>(component)->ScrollToVertical(position);
	else if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->ScrollToVertical(position);
}

void LSG_SelectCard(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->Select(row);
}

void LSG_SelectCardRowByOffset(const std::string& id, int offset)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsCards()))
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->SelectRow(offset);
}

void LSG_SelectCards(const std::string& id, const std::vector<int>& rows)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->Select(rows);
}

void LSG_SelectRow(const std::string& id, int row)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->Select(row);
}

void LSG_SelectRowByOffset(const std::string& id, int offset)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->SelectRow(offset);
}

void LSG_SelectRows(const std::string& id, const std::vector<int>& rows)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	static_cast<LSG_List*>(component)->Select(rows);
}

void LSG_SelectTile(const std::string& id, int index)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->Select(index);
}

void LSG_SelectTiles(const std::string& id, const std::vector<int>& indices)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->Select(indices);
}

void LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetAlignmentHorizontal(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetAlignmentVertical(alignment);

	LSG_UI::LayoutParent(component);
}

void LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBackgroundColor(color);
}

void LSG_SetBorderColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBorderColor(color);
}

void LSG_SetBorderRadius(const std::string& id, int radius)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBorderRadius(radius);

	LSG_UI::LayoutParent(component);
}

void LSG_SetBorderWidth(const std::string& id, int width)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetBorderWidth(width);

	LSG_UI::LayoutParent(component);
}

void LSG_SetButton(const std::string& id, const std::string& text, const std::string& icon)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsButton())
		throw std::invalid_argument(getErrorNoID("<button>", id));

	static_cast<LSG_Button*>(component)->Set(text, icon);
}

void LSG_SetCard(const std::string& id, int index, const LSG_CardItem& card)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->SetCard(index, card);
}

void LSG_SetCards(const std::string& id, const LSG_CardItems& cards)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsCards())
		throw std::invalid_argument(getErrorNoID("<cards>", id));

	static_cast<LSG_Cards*>(component)->SetCards(cards);
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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetEnabled(component, enabled);
}

void LSG_SetFontSize(const std::string& id, int size)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

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

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto height = std::format("{}%", (int)std::ceil(std::max(0.0, std::min(1.0, percent)) * 100.0));

	LSG_XML::SetAttribute(component->GetXmlNode(), "height", height);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetImage(const std::string& id, const std::string& file, bool fill)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsImage())
		throw std::invalid_argument(getErrorNoID("<image>", id));

	static_cast<LSG_Image*>(component)->Set(file, fill);
}

void LSG_SetListItem(const std::string& id, int row, const std::string& item)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetItem(row, item);
}

void LSG_SetListItems(const std::string& id, const LSG_Strings& items)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetItems(items);
}

void LSG_SetMargin(const std::string& id, int margin)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetMargin(margin);

	LSG_UI::LayoutParent(component);
}

void LSG_SetMenuItemIcon(const std::string& id, const std::string& imageFile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "icon", imageFile);

	static_cast<LSG_MenuItem*>(component)->Set();
}

void LSG_SetMenuItemSelected(const std::string& id, bool selected)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	static_cast<LSG_MenuItem*>(component)->SetSelected(selected);
}

void LSG_SetMenuItemValue(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsMenuItem())
		throw std::invalid_argument(getErrorNoID("<menu-item>", id));

	component->text = value;
}

void LSG_SetNavigationItemCount(const std::string& id, size_t itemsTotal, size_t itemsPerNavigation)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsNavigation())
		throw std::invalid_argument(getErrorNoID("<navigation>", id));

	static_cast<LSG_Navigation*>(component)->Set(itemsTotal, itemsPerNavigation);
}

void LSG_SetOrientation(const std::string& id, LSG_Orientation orientation, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetPadding(padding);

	LSG_UI::LayoutParent(component);
}

void LSG_SetPage(const std::string& id, int page)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->SetPageItem(row, item);
}

void LSG_SetPageTableRow(const std::string& id, int row, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetPageRow(row, columns);
}

void LSG_SetProgressValue(const std::string& id, double percent)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsProgressBar())
		throw std::invalid_argument(getErrorNoID("<progress-bar>", id));

	static_cast<LSG_ProgressBar*>(component)->SetValue(percent);
}

void LSG_SetSize(const std::string& id, const SDL_Size& size, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto w = std::format("{}%", (int)std::ceil(std::max(0.0, std::min(1.0, width))  * 100.0));
	auto h = std::format("{}%", (int)std::ceil(std::max(0.0, std::min(1.0, height)) * 100.0));

	LSG_XML::SetAttribute(component->GetXmlNode(), "width",  w);
	LSG_XML::SetAttribute(component->GetXmlNode(), "height", h);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetSliderOnHoverCallback(const std::string& id, const LSG_OnHoverCallback& callback)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	static_cast<LSG_Slider*>(component)->SetOnHoverCallback(callback);
}

void LSG_SetSliderParts(const std::string& id, const LSG_SliderParts& parts)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	static_cast<LSG_Slider*>(component)->SetParts(parts);
}

void LSG_SetSliderValue(const std::string& id, double percent)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsSlider())
		throw std::invalid_argument(getErrorNoID("<slider>", id));

	static_cast<LSG_Slider*>(component)->SetValue(percent);
}

void LSG_SetSpacing(const std::string& id, int spacing)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetSpacing(spacing);

	LSG_UI::LayoutParent(component);
}

void LSG_SetTableColumnWidth(const std::string& id, int column, int width)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetColumnWidth(column, width);
}

void LSG_SetTableGroup(const std::string& id, const LSG_TableGroup& group)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetGroup(group);
}

void LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetGroups(groups);
}

void LSG_SetTableHeader(const std::string& id, const LSG_Strings& header)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetHeader(header);
}

void LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetRow(row, columns);
}

void LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->SetRows(rows);
}

void LSG_SetText(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTextLabel())
		throw std::invalid_argument(getErrorNoID("<text>", id));

	static_cast<LSG_TextLabel*>(component)->Set(value);

	LSG_UI::LayoutParent(component);
}

void LSG_SetTextColor(const std::string& id, const SDL_Color& color)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	LSG_UI::SetTextColor(component, color);
	LSG_UI::SetText(component);
}

void LSG_SetTextInputValue(const std::string& id, const std::string& value)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTextInput())
		throw std::invalid_argument(getErrorNoID("<text-input>", id));

	static_cast<LSG_TextInput*>(component)->SetValue(value);
}

void LSG_SetTile(const std::string& id, int index, const LSG_TileItem& tile)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->SetTile(index, tile);
}

void LSG_SetTiles(const std::string& id, const LSG_TileItems& tiles)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTiles())
		throw std::invalid_argument(getErrorNoID("<tiles>", id));

	static_cast<LSG_Tiles*>(component)->SetTiles(tiles);
}

void LSG_SetTitle(const std::string& id, const std::string& title)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsModal() && !component->IsMenu() && !component->IsSubMenu()))
		throw std::invalid_argument(getErrorNoID("<modal>, <menu> or <menu-sub>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "title", title);

	if (component->IsModal())
		static_cast<LSG_Modal*>(component)->Set();
	else if (component->IsMenu())
		static_cast<LSG_Menu*>(component)->SetMenu();
	else if (component->IsSubMenu())
		static_cast<LSG_MenuSub*>(component)->Set();
}

void LSG_SetToggle(const std::string& id, bool on)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsToggle())
		throw std::invalid_argument(getErrorNoID("<toggle>", id));

	static_cast<LSG_Toggle*>(component)->Set(on);
}

void LSG_SetTooltip(const std::string& id, const std::string& tooltip)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	component->SetTooltip(tooltip);
}

void LSG_SetVisible(const std::string& id, bool visible, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	if (component->IsModal())
		throw std::invalid_argument("Use LSG_OpenModal or LSG_CloseModal to show or hide a modal.");

	component->SetVisible(visible);

	if (layout)
		LSG_UI::LayoutRoot();
}

void LSG_SetWidth(const std::string& id, int width, bool layout)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

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

	auto component = getComponent(id);

	if (!component)
		throw std::invalid_argument(getErrorNoID("", id));

	auto width = std::format("{}%", (int)std::ceil(std::max(0.0, std::min(1.0, percent)) * 100.0));

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

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "show-column-border", (show ? "true" : "false"));
}

void LSG_ShowError(const std::string& message)
{
	#if _DEBUG
		std::fprintf(stderr, "%s\n", message.c_str());
	#endif

	LSG_Window::ShowMessage(message);
}

void LSG_ShowRowBorder(const std::string& id, bool show)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || (!component->IsList() && !component->IsTable()))
		throw std::invalid_argument(getErrorNoID("<list> or <table>", id));

	LSG_XML::SetAttribute(component->GetXmlNode(), "show-row-border", (show ? "true" : "false"));
}

void LSG_SortList(const std::string& id, LSG_SortOrder sortOrder)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsList())
		throw std::invalid_argument(getErrorNoID("<list>", id));

	static_cast<LSG_List*>(component)->Sort(sortOrder);
}

void LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn)
{
	if (!isRunning)
		throw std::runtime_error(ERROR_NOT_STARTED);

	auto component = getComponent(id);

	if (!component || !component->IsTable())
		throw std::invalid_argument(getErrorNoID("<table>", id));

	static_cast<LSG_Table*>(component)->Sort(sortOrder, sortColumn);
}

std::string LSG_TextJoin(const LSG_Strings& strings, const std::string& separator)
{
	return LSG_Text::Join(strings, separator);
}

std::string LSG_TextReplace(const std::string& text, const std::string& oldSubstring, const std::string& newSubstring)
{
	return LSG_Text::Replace(text, oldSubstring, newSubstring);
}

LSG_Strings LSG_TextSplit(const std::string& text, char separator)
{
	return LSG_Text::Split(text, separator);
}

std::string LSG_TextToLower(const std::string& text)
{
	return LSG_Text::ToLower(text);
}

std::string LSG_TextToUpper(const std::string& text)
{
	return LSG_Text::ToUpper(text);
}

std::string LSG_TextToUTF8(const std::wstring& wide)
{
	return LSG_Text::ToUTF8(wide);
}

std::wstring LSG_TextToWide(const std::string& text)
{
	return LSG_Text::ToWide(text);
}

std::string LSG_TextTrim(const std::string& text)
{
	return LSG_Text::Trim(text);
}

SDL_Renderer* LSG_Start(const std::string& xmlFile)
{
	LibXml::xmlInitParser();

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
		LSG_SetWindowMinimumSize(minWidth, minWidth);
	else if (minHeight > 0)
		LSG_SetWindowMinimumSize(minHeight, minHeight);

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

	if (basePath.empty())
		basePath = std::string(workingDir);

	auto windowAttribs = LSG_UI::OpenWindow(xmlFile);

	if (!TTF_Init())
		throw std::runtime_error(std::format("Failed to initialize SDL3_ttf: {}", SDL_GetError()));

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
