#include "LSG_Window.h"

float         LSG_Window::dpiScale  = 1.0f;
SDL_Renderer* LSG_Window::renderer  = nullptr;
SDL_Window*   LSG_Window::window    = nullptr;

#if defined _ios
    UIWindow* LSG_Window::uiWindow = nil;
#elif defined _linux
	std::string LSG_Window::path  = "";
	LSG_Strings LSG_Window::paths = {};
#endif

void LSG_Window::Close()
{
	if (LSG_Window::renderer) {
		SDL_DestroyRenderer(LSG_Window::renderer);
		LSG_Window::renderer = nullptr;
	}

	if (LSG_Window::window) {
		SDL_DestroyWindow(LSG_Window::window);
		LSG_Window::window = nullptr;
	}
}

int LSG_Window::GetDPIScaled(int value, bool inverse)
{
	if (inverse)
		return (int)((float)value / LSG_Window::dpiScale);

	return (int)((float)value * LSG_Window::dpiScale);
}

#if defined _windows
std::vector<std::wstring> LSG_Window::getFiltersWide(const LSG_Strings& filters)
{
	std::vector<std::wstring> filtersWide;

	for (const auto& filter : filters)
	{
		auto filterWide = (wchar_t*)SDL_iconv_string("WCHAR_T", "UTF-8", filter.c_str(), (filter.size() + 1));

		filtersWide.push_back(std::wstring(filterWide));

		SDL_free(filterWide);
	}

	return filtersWide;
}
#endif

SDL_Size LSG_Window::GetMinimumSize()
{
	SDL_Size size = {};
	SDL_GetWindowMinimumSize(LSG_Window::window, &size.width, &size.height);

	return size;
}

SDL_Point LSG_Window::GetMousePosition()
{
	if (SDL_HasMouse())
	{
		float x, y;
		SDL_GetMouseState(&x, &y);

		SDL_ShowSimpleMessageBox(0, "MOUSE_POSITION", std::format("{} {}", x, y).c_str(), LSG_Window::window);

		return { (int)x, (int)y, };
	}

	SDL_ShowSimpleMessageBox(0, "TOUCH_POSITION", "1", LSG_Window::window);

	int  touchDeviceCount;
	auto touchDevices = SDL_GetTouchDevices(&touchDeviceCount);

	if (!touchDevices || (touchDeviceCount < 1))
		return {};

	SDL_ShowSimpleMessageBox(0, "TOUCH_POSITION", "2", LSG_Window::window);

	int  fingerCount;
	auto fingers = SDL_GetTouchFingers(touchDevices[0], &fingerCount);

	if (!fingers || (fingerCount < 1)) {
		SDL_free(touchDevices);
		return {};
	}

	SDL_ShowSimpleMessageBox(0, "TOUCH_POSITION", "3", LSG_Window::window);

	auto windowSize = LSG_Window::GetSizeInPixels();

	SDL_Point position = {
		(int)(fingers[0]->x * (float)windowSize.width),
		(int)(fingers[0]->y * (float)windowSize.height)
	};

	SDL_free(fingers);
	SDL_free(touchDevices);

	SDL_ShowSimpleMessageBox(0, "TOUCH_POSITION", std::format("{} {}", position.x, position.y).c_str(), LSG_Window::window);

	return position;
}

SDL_Point LSG_Window::GetPosition()
{
	SDL_Point position = {};
	SDL_GetWindowPosition(LSG_Window::window, &position.x, &position.y);

	return position;
}

/**
 * @throws runtime_error
 */
SDL_Size LSG_Window::GetSizeInPixels()
{
	auto renderTarget = SDL_GetRenderTarget(LSG_Window::renderer);

	SDL_Size size = {};

	if (!SDL_SetRenderTarget(LSG_Window::renderer, nullptr))
		throw std::runtime_error(std::format("Failed to set render target: {}", SDL_GetError()));

	SDL_GetRenderOutputSize(LSG_Window::renderer, &size.width, &size.height);

	SDL_SetRenderTarget(LSG_Window::renderer, renderTarget);

	return size;
}

SDL_FPoint LSG_Window::GetSizeScale()
{
	if (!LSG_Window::window)
		return { 1.0f, 1.0f };

	auto sizeInPixels = LSG_Window::GetSizeInPixels();

	SDL_Size size = {};
	SDL_GetWindowSize(LSG_Window::window, &size.width, &size.height);

	SDL_FPoint scale = {
		((float)sizeInPixels.width  / (float)size.width),
		((float)sizeInPixels.height / (float)size.height)
	};

	return scale;
}

std::string LSG_Window::GetTitle()
{
	return SDL_GetWindowTitle(LSG_Window::window);
}

/**
 * @throws runtime_error
 */
void LSG_Window::InitRenderTarget(SDL_Texture* &renderTarget, const SDL_Size& textureSize)
{
	if (renderTarget)
	{
		auto targetSize = LSG_Graphics::GetTextureSize(renderTarget);

		if ((textureSize.width != targetSize.width) || (textureSize.height != targetSize.height)) {
			SDL_DestroyTexture(renderTarget);
			renderTarget = nullptr;
		}
	}

	if (!renderTarget)
	{
		auto format = SDL_GetWindowPixelFormat(LSG_Window::window);

		if (format == SDL_PIXELFORMAT_UNKNOWN)
			throw std::runtime_error(std::format("Failed to get window pixel format: {}", SDL_GetError()));

		renderTarget = SDL_CreateTexture(LSG_Window::renderer, format, SDL_TEXTUREACCESS_TARGET, textureSize.width, textureSize.height);
	}

	if (!renderTarget)
		throw std::runtime_error(std::format("Failed to create a render target: {}", SDL_GetError()));
}

bool LSG_Window::IsMaximized()
{
	return (SDL_GetWindowFlags(LSG_Window::window) & SDL_WINDOW_MAXIMIZED);
}

/**
 * @throws runtime_error
 */
SDL_Renderer* LSG_Window::Open(const std::string& title, int width, int height)
{
	LSG_Window::window = SDL_CreateWindow(
		title.c_str(),
		width,
		height,
		(SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE)
	);

	if (!LSG_Window::window)
		throw std::runtime_error(std::format("Failed to create a window: {}", SDL_GetError()));

	#if defined _linux || defined _macosx || defined _windows
		auto iconFile    = LSG_Text::GetFullPath("img/icon.png");
		auto iconSurface = IMG_Load(iconFile.c_str());

		SDL_SetWindowIcon(LSG_Window::window, iconSurface);
		SDL_DestroySurface(iconSurface);
	#endif

    #if defined _ios
		SDL_PropertiesID windowProperties = SDL_GetWindowProperties(LSG_Window::window);

		LSG_Window::uiWindow = (__bridge UIWindow*)SDL_GetPointerProperty(
			windowProperties,
			SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER,
			nullptr
		);
    #endif

	LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, nullptr);

	if (!LSG_Window::renderer)
		throw std::runtime_error(std::format("Failed to create a renderer: {}", SDL_GetError()));

	LSG_Window::SetDPIScale();

	return LSG_Window::renderer;
}

#if defined _windows && defined _DEBUG
/**
 * @throws runtime_error
 */
void LSG_Window::OpenTest()
{
	auto surface = SDL_CreateSurface(800, 600, SDL_PIXELFORMAT_RGB24);

	if (!surface)
		throw std::runtime_error(std::format("Failed to create a surface: {}", SDL_GetError()));

	LSG_Window::renderer = SDL_CreateSoftwareRenderer(surface);

	if (!LSG_Window::renderer)
		throw std::runtime_error(std::format("Failed to create a renderer: {}", SDL_GetError()));

	LSG_Window::SetDPIScale();
}
#endif

#if defined _linux
LSG_Strings LSG_Window::getPaths(GFile* file)
{
	if (!file)
		return {};

	LSG_Strings paths = { std::string(g_file_get_path(file)) };

	g_object_unref(file);

	return paths;
}

LSG_Strings LSG_Window::getPaths(GListModel* files)
{
	if (!files)
		return {};

	LSG_Strings paths;

	for (auto i = 0; i < g_list_model_get_n_items(files); i++)
	{
		auto item = g_list_model_get_object(files, i);
		auto path = std::string(g_file_get_path(G_FILE(item)));

		paths.push_back(path);

		g_object_unref(item);
	}

	return paths;
}

void LSG_Window::openFileCB(GObject* source, GAsyncResult* result, gpointer user_data)
{
	auto file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, nullptr);

	LSG_Window::paths = LSG_Window::getPaths(file);
}

void LSG_Window::openFileMultipleCB(GObject* source, GAsyncResult* result, gpointer user_data)
{
	auto files = gtk_file_dialog_open_multiple_finish(GTK_FILE_DIALOG(source), result, nullptr);

	LSG_Window::paths = LSG_Window::getPaths(files);
}

void LSG_Window::openFolderCB(GObject* source, GAsyncResult* result, gpointer user_data)
{
	auto folder = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, nullptr);

	LSG_Window::paths = LSG_Window::getPaths(folder);
}

void LSG_Window::openFolderMultipleCB(GObject* source, GAsyncResult* result, gpointer user_data)
{
	auto folders = gtk_file_dialog_select_multiple_folders_finish(GTK_FILE_DIALOG(source), result, nullptr);

	LSG_Window::paths = LSG_Window::getPaths(folders);
}

void LSG_Window::setFilters(const LSG_Strings& filters, GtkFileDialog* dialog)
{
	if (filters.empty())
		return;

	auto filterList = g_list_store_new(GTK_TYPE_FILE_FILTER);

	for (const auto& filter : filters)
	{
		auto fileFilter = gtk_file_filter_new();

		gtk_file_filter_add_pattern(fileFilter, filter.c_str());

		g_list_store_append(filterList, fileFilter);
	}

	gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filterList));
}

LSG_Strings LSG_Window::openFiles(bool openFolder, bool allowMultipleSelection, const LSG_Strings& filters)
{
	if (std::strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv_unsafe("DISPLAY", ":0", 1);

	if (!gtk_init_check())
		return {};

	auto dialog = gtk_file_dialog_new();

	LSG_Window::setFilters(filters, dialog);

	LSG_Window::paths.clear();

	if (openFolder) {
		if (allowMultipleSelection)
			gtk_file_dialog_select_multiple_folders(dialog, nullptr, nullptr, openFolderMultipleCB, nullptr);
		else
			gtk_file_dialog_select_folder(dialog, nullptr, nullptr, openFolderCB, nullptr);
	} else {
		if (allowMultipleSelection)
			gtk_file_dialog_open_multiple(dialog, nullptr, nullptr, openFileMultipleCB, nullptr);
		else
			gtk_file_dialog_open(dialog, nullptr, nullptr, openFileCB, nullptr);
	}

	g_object_unref(dialog);

	while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0)
		g_main_context_iteration(nullptr, TRUE);

	return LSG_Window::paths;
}
#elif defined _macosx
LSG_Strings LSG_Window::openFiles(bool openFolder, bool allowMultipleSelection, const LSG_Strings& filters)
{
	auto panel = [NSOpenPanel openPanel];

	if (!panel)
		return {};

	[panel setAllowsMultipleSelection: (allowMultipleSelection ? YES : NO)];
	[panel setCanChooseDirectories:    (openFolder ? YES : NO)];
	[panel setCanChooseFiles:          (openFolder ? NO : YES)];

	if (!filters.empty())
	{
		auto types = [NSMutableArray arrayWithCapacity: (NSUInteger)filters.size()];

		for (const auto& filter : filters)
			[types addObject: [UTType typeWithFilenameExtension: [NSString stringWithFormat: @"%s", filter.c_str()]]];

		[panel setAllowedContentTypes: types];
	}

	if ([panel runModal] != NSModalResponseOK)
		return {};

	const int MAX_FILE_PATH = 260;

	LSG_Strings filePaths;

	for (id url in [panel URLs])
	{
		auto selectedURL = (CFURLRef)url;

		if (!selectedURL)
			continue;

		char selectedPath[MAX_FILE_PATH] = {};

		if (!CFURLGetFileSystemRepresentation(selectedURL, TRUE, (UInt8*)selectedPath, MAX_FILE_PATH))
			continue;

		auto filePath = std::string(selectedPath);

		if (filePath.substr(0, 7) == "file://")
			filePath = filePath.substr(7);

		if (!filePath.empty())
			filePaths.push_back(filePath);
	}

	return filePaths;
}
#elif defined _windows
std::vector<std::wstring> LSG_Window::openFiles(bool allowMultipleSelection, const LSG_Strings& filters)
{
	IFileOpenDialog* browseDialog = nullptr;

	// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog

	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browseDialog))) || !browseDialog)
		return {};

	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions

	auto options = (FOS_DONTADDTORECENT | FOS_FILEMUSTEXIST | FOS_NOCHANGEDIR | FOS_NODEREFERENCELINKS);

	if (allowMultipleSelection)
		options |= FOS_ALLOWMULTISELECT;

	browseDialog->SetOptions(options);

	std::vector<COMDLG_FILTERSPEC> filterSpecs;

	auto filtersWide = LSG_Window::getFiltersWide(filters);

	for (const auto& filterType : filtersWide)
		filterSpecs.push_back({ .pszName = filterType.c_str(), .pszSpec = filterType.c_str() });

	if (!filterSpecs.empty())
		browseDialog->SetFileTypes(filterSpecs.size(), filterSpecs.data());

	std::vector<std::wstring> filePaths;

	IShellItemArray* shellItems     = nullptr;
	DWORD            shellItemCount = 0;

	if (SUCCEEDED(browseDialog->Show(nullptr)) && SUCCEEDED(browseDialog->GetResults(&shellItems)) && SUCCEEDED(shellItems->GetCount(&shellItemCount)))
	{
		for (DWORD i = 0; i < shellItemCount; i++)
		{
			IShellItem* shellItem = nullptr;

			if (FAILED(shellItems->GetItemAt(i, &shellItem)))
				continue;

			LPWSTR selectedPath = nullptr;

			if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath)) && selectedPath)
				filePaths.push_back(std::wstring(selectedPath));

			if (selectedPath)
				CoTaskMemFree(selectedPath);

			if (shellItem)
				shellItem->Release();
		}
	}

	if (shellItems)
		shellItems->Release();

	if (browseDialog)
		browseDialog->Release();

	return filePaths;
}
#endif

#if defined _windows
std::wstring LSG_Window::OpenFile(const LSG_Strings& filters)
{
	auto files = LSG_Window::openFiles(false, filters);

	return (!files.empty() ? files[0] : L"");
}

std::vector<std::wstring> LSG_Window::OpenFiles(const LSG_Strings& filters)
{
	return LSG_Window::openFiles(true, filters);
}

std::vector<std::wstring> LSG_Window::openFolders(bool allowMultipleSelection)
{
	IFileOpenDialog* browseDialog = nullptr;

	// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog

	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browseDialog))) || !browseDialog)
		return {};

	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions

	auto options = (FOS_DONTADDTORECENT | FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_PICKFOLDERS);

	if (allowMultipleSelection)
		options |= FOS_ALLOWMULTISELECT;

	browseDialog->SetOptions(options);

	std::vector<std::wstring> folderPaths;

	IShellItemArray* shellItems     = nullptr;
	DWORD            shellItemCount = 0;

	if (SUCCEEDED(browseDialog->Show(nullptr)) && SUCCEEDED(browseDialog->GetResults(&shellItems)) && SUCCEEDED(shellItems->GetCount(&shellItemCount)))
	{
		for (DWORD i = 0; i < shellItemCount; i++)
		{
			IShellItem* shellItem = nullptr;

			if (FAILED(shellItems->GetItemAt(i, &shellItem)))
				continue;

			LPWSTR selectedPath = nullptr;

			if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath)) && selectedPath)
				folderPaths.push_back(std::wstring(selectedPath));

			if (selectedPath)
				CoTaskMemFree(selectedPath);

			if (shellItem)
				shellItem->Release();
		}
	}

	if (shellItems)
		shellItems->Release();

	if (browseDialog)
		browseDialog->Release();

	return folderPaths;
}

std::wstring LSG_Window::OpenFolder()
{
	auto folders = LSG_Window::openFolders(false);

	return (!folders.empty() ? folders[0] : L"");
}

std::vector<std::wstring> LSG_Window::OpenFolders()
{
	return LSG_Window::openFolders(true);
}
#elif defined _linux || defined _macosx
std::string LSG_Window::OpenFile(const LSG_Strings& filters)
{
	auto files = LSG_Window::openFiles(false, false, filters);

	return (!files.empty() ? files[0] : "");
}

LSG_Strings LSG_Window::OpenFiles(const LSG_Strings& filters)
{
	return LSG_Window::openFiles(false, true, filters);
}

std::string LSG_Window::OpenFolder()
{
	auto folders = LSG_Window::openFiles(true, false, {});

	return (!folders.empty() ? folders[0] : "");
}

LSG_Strings LSG_Window::OpenFolders()
{
	return LSG_Window::openFiles(true, true, {});
}
#elif defined _android
std::string LSG_Window::OpenFile(const LSG_Strings& filters)
{
	return LSG_Window::pickFile(filters);
}

std::string LSG_Window::OpenFolder()
{
	auto jniEnvironment      = LSG_AndroidJNI::GetEnvironment();
	auto jniActivity         = LSG_AndroidJNI::GetClass(LSG_ConstAndroid::ActivityClassPath, jniEnvironment);
	//auto jniIsPickingContent = jniEnvironment->GetStaticFieldID(jniActivity,  "IsPickingContent", "Z");
	auto jniContentPath      = jniEnvironment->GetStaticFieldID(jniActivity,  "ContentPath",      "Ljava/lang/String;");
	auto jniOpenFolder       = jniEnvironment->GetStaticMethodID(jniActivity, "OpenFolder",       "()V");

	jniEnvironment->CallStaticVoidMethod(jniActivity, jniOpenFolder);

	//while (jniEnvironment->GetStaticBooleanField(jniActivity, jniIsPickingContent))
	//    SDL_Delay(10);

	auto openedFolder   = (jstring)jniEnvironment->GetStaticObjectField(jniActivity, jniContentPath);
	auto folderUTF8     = jniEnvironment->GetStringUTFChars(openedFolder, nullptr);
	auto selectedFolder = std::string(folderUTF8);

	jniEnvironment->ReleaseStringUTFChars(openedFolder, folderUTF8);
	jniEnvironment->DeleteLocalRef(jniActivity);

	return selectedFolder;
}

std::string LSG_Window::pickFile(const LSG_Strings& filters, bool saveFile)
{
	auto jniEnvironment      = LSG_AndroidJNI::GetEnvironment();
	auto jniActivity         = LSG_AndroidJNI::GetClass(LSG_ConstAndroid::ActivityClassPath, jniEnvironment);
	//auto jniIsPickingContent = jniEnvironment->GetStaticFieldID(jniActivity, "IsPickingContent", "Z");
	auto jniContentPath      = jniEnvironment->GetStaticFieldID(jniActivity, "ContentPath",      "Ljava/lang/String;");
	auto jniPickMethod       = (saveFile ? "SaveFile" : "OpenFile");
	auto jniPickFile         = jniEnvironment->GetStaticMethodID(jniActivity, jniPickMethod, "(Ljava/lang/String;)V");

	std::string filter = "";

	for (size_t i = 0; i < filters.size(); i++)
		filter.append(filters[i]).append(i < (filters.size() - 1) ? " " : "");

	auto jniFilter = jniEnvironment->NewStringUTF(filter.c_str());

	jniEnvironment->CallStaticVoidMethod(jniActivity, jniPickFile, jniFilter);

	//while (jniEnvironment->GetStaticBooleanField(jniActivity, jniIsPickingContent))
	//    SDL_Delay(10);

	auto pickedFile   = (jstring)jniEnvironment->GetStaticObjectField(jniActivity, jniContentPath);
	auto fileUTF8     = jniEnvironment->GetStringUTFChars(pickedFile, nullptr);
	auto selectedFile = std::string(fileUTF8);

	jniEnvironment->ReleaseStringUTFChars(pickedFile, fileUTF8);
	jniEnvironment->DeleteLocalRef(jniFilter);
	jniEnvironment->DeleteLocalRef(jniActivity);

	return selectedFile;
}
#elif defined _ios
@interface MyDocumentPicker : UIViewController<UIDocumentPickerDelegate>
@property std::function<void(NSArray<NSURL*>*)> resultsCallback;
@end

@implementation MyDocumentPicker
- (void)documentPicker: (UIDocumentPickerViewController*)picker didPickDocumentsAtURLs: (NSArray<NSURL*>*)urls
{
    [picker dismissViewControllerAnimated: true completion: nil];

    self.resultsCallback(urls);
}

- (void)documentPickerWasCancelled: (UIDocumentPickerViewController*)picker
{
    [picker dismissViewControllerAnimated: true completion: nil];

    self.resultsCallback([NSArray array]);
}
@end

@interface MyMediaPicker : UIViewController<MPMediaPickerControllerDelegate>
@property std::function<void(NSArray<MPMediaItem*>*)> resultsCallback;
@end

@implementation MyMediaPicker
- (void)mediaPicker: (MPMediaPickerController*)picker didPickMediaItems: (MPMediaItemCollection*)itemCollection
{
    [picker dismissViewControllerAnimated: true completion: nil];

    self.resultsCallback(itemCollection.items);
}

- (void)mediaPickerDidCancel: (MPMediaPickerController*)picker
{
    [picker dismissViewControllerAnimated: true completion: nil];

    self.resultsCallback([NSArray array]);
}
@end

@interface MyPhotoPicker : UIViewController<PHPickerViewControllerDelegate>
@property std::function<void(NSArray<PHPickerResult*>*)> resultsCallback;
@end

@implementation MyPhotoPicker
- (void)picker: (PHPickerViewController*)picker didFinishPicking: (NSArray<PHPickerResult*>*)results
{
	[picker dismissViewControllerAnimated: true completion: nil];

	self.resultsCallback(results);
}
@end

void LSG_Window::OpenFileDocuments(std::function<void(NSArray<NSURL*>*)> resultsCallback, bool allowMultipleSelection)
{
    auto documentPicker = [[MyDocumentPicker alloc] init];

    documentPicker.resultsCallback = resultsCallback;

    auto picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes: [NSArray arrayWithObject: UTTypeItem]];

    picker.allowsMultipleSelection = (allowMultipleSelection ? YES : NO);

    picker.delegate = documentPicker;

    auto viewController = LSG_Window::uiWindow.rootViewController;

    [viewController presentViewController: picker animated: true completion: nil];
}

void LSG_Window::OpenFileMedia(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback, bool allowMultipleSelection)
{
    auto authStatus = [SKCloudServiceController authorizationStatus];
    
    if (authStatus == SKCloudServiceAuthorizationStatusNotDetermined)
    {
        [SKCloudServiceController requestAuthorization: ^(SKCloudServiceAuthorizationStatus status) {}];

        do {
            SDL_Delay(10);
            authStatus = [SKCloudServiceController authorizationStatus];
        } while (authStatus == SKCloudServiceAuthorizationStatusNotDetermined);
    }

    if ((authStatus != SKCloudServiceAuthorizationStatusAuthorized) && (authStatus != SKCloudServiceAuthorizationStatusRestricted)) {
        resultsCallback([NSArray array]);
        return;
    }

    auto mediaPicker = [[MyMediaPicker alloc] init];

    mediaPicker.resultsCallback = resultsCallback;

    auto picker = [[MPMediaPickerController alloc] init];

    picker.allowsPickingMultipleItems = (allowMultipleSelection ? YES : NO);

    picker.delegate = mediaPicker;

	auto viewController = LSG_Window::uiWindow.rootViewController;

    [viewController presentViewController: picker animated: true completion: nil];
}

void LSG_Window::OpenFilePhotos(std::function<void(NSArray<PHPickerResult*>* results)> resultsCallback, bool allowMultipleSelection)
{
    auto authStatus = [PHPhotoLibrary authorizationStatusForAccessLevel: PHAccessLevelReadWrite];

    if (authStatus == PHAuthorizationStatusNotDetermined)
    {
        [PHPhotoLibrary requestAuthorizationForAccessLevel: PHAccessLevelReadWrite handler: ^(PHAuthorizationStatus status) {}];

        do {
            SDL_Delay(10);
            authStatus = [PHPhotoLibrary authorizationStatusForAccessLevel: PHAccessLevelReadWrite];
        } while (authStatus == PHAuthorizationStatusNotDetermined);
    }

    if ((authStatus != PHAuthorizationStatusAuthorized) && (authStatus != PHAuthorizationStatusLimited)) {
        resultsCallback([NSArray array]);
        return;
    }

    auto photoPicker = [[MyPhotoPicker alloc] init];

    photoPicker.resultsCallback = resultsCallback;

    auto pickerConfig = [[PHPickerConfiguration alloc] initWithPhotoLibrary: [PHPhotoLibrary sharedPhotoLibrary]];

    pickerConfig.selectionLimit = (allowMultipleSelection ? 0 : 1);

    auto picker = [[PHPickerViewController alloc] initWithConfiguration: pickerConfig];

    picker.delegate = photoPicker;
    
	auto viewController = LSG_Window::uiWindow.rootViewController;

    [viewController presentViewController: picker animated: true completion: nil];
}

void LSG_Window::OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    auto documentPicker = [[MyDocumentPicker alloc] init];

    documentPicker.resultsCallback = resultsCallback;

    auto picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes: [NSArray arrayWithObject: UTTypeFolder]];

    picker.delegate = documentPicker;

	auto viewController = LSG_Window::uiWindow.rootViewController;

    [viewController presentViewController: picker animated: true completion: nil];
}
#endif

void LSG_Window::Present()
{
	LSG_UI::Present(LSG_Window::renderer);
}

void LSG_Window::Render()
{
	SDL_SetRenderTarget(LSG_Window::renderer, nullptr);
	SDL_SetRenderDrawColor(LSG_Window::renderer, 0, 0, 0, 0);
	SDL_RenderClear(LSG_Window::renderer);

	LSG_UI::Render(LSG_Window::renderer);
}

/**
 * @throws runtime_error
 */
SDL_Texture* LSG_Window::RotateTexture(SDL_Texture* texture, const LSG_ImageOrientation& orientation, const SDL_Size& size)
{
	auto format = (SDL_PixelFormat)SDL_GetNumberProperty(
		SDL_GetTextureProperties(texture),
		SDL_PROP_TEXTURE_FORMAT_NUMBER,
		0
	);

	auto renderTarget = SDL_GetRenderTarget(LSG_Window::renderer);
	auto newTexture   = SDL_CreateTexture(LSG_Window::renderer, format, SDL_TEXTUREACCESS_TARGET, size.width, size.height);

	if (!SDL_SetRenderTarget(LSG_Window::renderer, newTexture))
		throw std::runtime_error(std::format("Failed to set render target: {}", SDL_GetError()));

	LSG_Graphics::RenderTextureRotated(LSG_Window::renderer, texture, nullptr, nullptr, orientation.rotation, nullptr, orientation.flip);

	SDL_SetRenderTarget(LSG_Window::renderer, renderTarget);

	SDL_DestroyTexture(texture);

	return newTexture;
}

#if defined _android
std::string LSG_Window::SaveFile(const LSG_Strings& filters)
{
	return LSG_Window::pickFile(filters, true);
}
#elif defined _linux
void LSG_Window::saveFileCB(GObject* source, GAsyncResult* result, gpointer user_data)
{
	auto file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

	if (!file)
		return;

	LSG_Window::path = std::string(g_file_get_path(file));

	g_object_unref(file);
}

std::string LSG_Window::SaveFile(const LSG_Strings& filters)
{
	if (std::strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv_unsafe("DISPLAY", ":0", 1);

	if (!gtk_init_check())
		return "";

	auto dialog = gtk_file_dialog_new();

	LSG_Window::setFilters(filters, dialog);

	LSG_Window::path = "";

	gtk_file_dialog_save(dialog, nullptr, nullptr, saveFileCB, nullptr);

	g_object_unref(dialog);

	while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0)
		g_main_context_iteration(nullptr, TRUE);

	return LSG_Window::path;
}
#elif defined _macosx
std::string LSG_Window::SaveFile(const LSG_Strings& filters)
{
	NSSavePanel* panel = [NSSavePanel savePanel];

	if (!panel)
		return "";

	if (!filters.empty())
	{
		auto types = [NSMutableArray arrayWithCapacity: (NSUInteger)filters.size()];

		for (const auto& filter : filters)
			[types addObject: [UTType typeWithFilenameExtension: [NSString stringWithFormat: @"%s", filter.c_str()]]];

		[panel setAllowedContentTypes: types];
	}

	if ([panel runModal] != NSModalResponseOK)
		return "";

	CFURLRef selectedURL = (CFURLRef)[panel URL];

	if (!selectedURL)
		return "";

	const int MAX_FILE_PATH = 260;
	char      selectedPath[MAX_FILE_PATH] = {};

	if (!CFURLGetFileSystemRepresentation(selectedURL, TRUE, (UInt8*)selectedPath, MAX_FILE_PATH))
		return "";

	auto filePath = std::string(selectedPath);

	if (filePath.substr(0, 7) == "file://")
		filePath = filePath.substr(7);

	return filePath;
}
#elif defined _windows
std::wstring LSG_Window::SaveFile(const LSG_Strings& filters)
{
	IFileSaveDialog* browseDialog = nullptr;

	// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog

	if (FAILED(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browseDialog))) || !browseDialog)
		return {};

	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions

	auto options = (FOS_CREATEPROMPT | FOS_DONTADDTORECENT | FOS_NOCHANGEDIR | FOS_NODEREFERENCELINKS | FOS_OVERWRITEPROMPT);

	if (!filters.empty())
		options |= FOS_STRICTFILETYPES;

	browseDialog->SetOptions(options);

	std::vector<COMDLG_FILTERSPEC> filterSpecs;

	auto filtersWide = LSG_Window::getFiltersWide(filters);

	for (const auto& filterType : filtersWide)
		filterSpecs.push_back({ .pszName = filterType.c_str(), .pszSpec = filterType.c_str() });

	if (!filterSpecs.empty())
		browseDialog->SetFileTypes(filterSpecs.size(), filterSpecs.data());

	std::wstring filePath  = L"";
	IShellItem*  shellItem = nullptr;

	if (SUCCEEDED(browseDialog->Show(nullptr)) && SUCCEEDED(browseDialog->GetResult(&shellItem)))
	{
		LPWSTR selectedPath = nullptr;

		if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath)) && selectedPath)
			filePath = std::wstring(selectedPath);

		if (selectedPath)
			CoTaskMemFree(selectedPath);
	}

	if (shellItem)
		shellItem->Release();

	if (browseDialog)
		browseDialog->Release();

	return filePath;
}
#endif

void LSG_Window::SetDPIScale()
{
	#if defined _android
		LSG_Window::dpiScale = SDL_GetWindowDisplayScale(LSG_Window::window);
	#else
		LSG_Window::dpiScale = LSG_Window::GetSizeScale().x;
	#endif
}

void LSG_Window::SetMaximized(bool maximized)
{
	if (maximized)
		SDL_MaximizeWindow(LSG_Window::window);
	else
		SDL_RestoreWindow(LSG_Window::window);
}

void LSG_Window::SetMinimumSize(int width, int height)
{
	SDL_SetWindowMinimumSize(LSG_Window::window, width, height);
}

void LSG_Window::SetPosition(int x, int y)
{
	SDL_SetWindowPosition(LSG_Window::window, x, y);
}

void LSG_Window::SetSize(int width, int height)
{
	SDL_SetWindowSize(LSG_Window::window, width, height);
}

void LSG_Window::SetTitle(const std::string& title)
{
	SDL_SetWindowTitle(LSG_Window::window, title.c_str());
}

void LSG_Window::ShowMessage(const std::string& message, uint32_t flags)
{
	SDL_ShowSimpleMessageBox(flags, LSG_Window::GetTitle().c_str(), message.c_str(), LSG_Window::window);
}

void LSG_Window::StartTextInput(const SDL_Rect* area, int cursor)
{
	SDL_SetTextInputArea(LSG_Window::window, area, cursor);
	SDL_StartTextInput(LSG_Window::window);
}

void LSG_Window::StopTextInput()
{
	SDL_StopTextInput(LSG_Window::window);
}

SDL_Texture* LSG_Window::ToTexture(const std::string& imageFile)
{
	if (imageFile.empty())
		return nullptr;

	auto filePath = LSG_Text::GetFullPath(imageFile);
	auto texture  = IMG_LoadTexture(LSG_Window::renderer, filePath.c_str());
	
	if (!texture)
		throw std::runtime_error(std::format("Failed to create texture from image '{}': {}", filePath, SDL_GetError()));

	return texture;
}

SDL_Texture* LSG_Window::ToTexture(SDL_Surface* surface)
{
	if (!surface)
		return nullptr;

	auto texture = SDL_CreateTextureFromSurface(LSG_Window::renderer, surface);

	if (!texture)
		throw std::runtime_error(std::format("Failed to create texture from surface: {}", SDL_GetError()));

	return texture;
}
