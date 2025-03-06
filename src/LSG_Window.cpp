#include "LSG_Window.h"

SDL_Renderer* LSG_Window::renderer  = nullptr;
SDL_SysWMinfo LSG_Window::sysWmInfo = {};
SDL_Window*   LSG_Window::window    = nullptr;

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

	SDL_Quit();
}

float LSG_Window::GetDPI()
{
	float dpi;
	SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(LSG_Window::window), &dpi, nullptr, nullptr);

	return dpi;
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
	SDL_Point mousePosition = {};
	SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	return mousePosition;
}

SDL_Point LSG_Window::GetPosition()
{
	SDL_Point position = {};
	SDL_GetWindowPosition(LSG_Window::window, &position.x, &position.y);

	return position;
}

SDL_Size LSG_Window::GetSize()
{
	auto renderTarget = SDL_GetRenderTarget(LSG_Window::renderer);

	SDL_SetRenderTarget(LSG_Window::renderer, nullptr);

	SDL_Size size = {};
	SDL_GetRendererOutputSize(LSG_Window::renderer, &size.width, &size.height);

	SDL_SetRenderTarget(LSG_Window::renderer, renderTarget);

	return size;
}

SDL_FPoint LSG_Window::GetSizeScale()
{
	auto sizeInPixels = LSG_Window::GetSize();

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

void LSG_Window::InitRenderTarget(SDL_Texture** renderTarget, const SDL_Size& textureSize)
{
	if (*renderTarget)
	{
		auto targetSize = LSG_Graphics::GetTextureSize(*renderTarget);

		if ((textureSize.width != targetSize.width) || (textureSize.height != targetSize.height)) {
			SDL_DestroyTexture(*renderTarget);
			*renderTarget = nullptr;
		}
	}

	if (!*renderTarget) {
		auto format   = SDL_GetWindowPixelFormat(LSG_Window::window);
		*renderTarget = SDL_CreateTexture(LSG_Window::renderer, format, SDL_TEXTUREACCESS_TARGET, textureSize.width, textureSize.height);
	}
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
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE)
	);

	if (!LSG_Window::window)
		throw std::runtime_error(LSG_Text::Format("Failed to create a window: %s", SDL_GetError()));

	#if defined _linux || defined _macosx || defined _windows
		auto iconFile    = LSG_Text::GetFullPath("img/icon.png");
		auto iconSurface = IMG_Load(iconFile.c_str());

		SDL_SetWindowIcon(LSG_Window::window, iconSurface);
		SDL_FreeSurface(iconSurface);
	#endif

	SDL_SetWindowMinimumSize(LSG_Window::window, LSG_Window::MinSize, LSG_Window::MinSize);

    SDL_VERSION(&LSG_Window::sysWmInfo.version);
    SDL_GetWindowWMInfo(LSG_Window::window, &LSG_Window::sysWmInfo);

	LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_ACCELERATED);

	if (!LSG_Window::renderer)
		LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_SOFTWARE);

	if (!LSG_Window::renderer)
		throw std::runtime_error(LSG_Text::Format("Failed to create a renderer: %s", SDL_GetError()));

	return LSG_Window::renderer;
}

#if defined _windows && defined _DEBUG
/**
 * @throws runtime_error
 */
void LSG_Window::OpenTest()
{
	auto surface = SDL_CreateRGBSurfaceWithFormat(0, 800, 600, 24, SDL_PIXELFORMAT_RGB24);

	if (!surface)
		throw std::runtime_error(LSG_Text::Format("Failed to create a surface: %s", SDL_GetError()));

	LSG_Window::renderer = SDL_CreateSoftwareRenderer(surface);

	if (!LSG_Window::renderer)
		throw std::runtime_error(LSG_Text::Format("Failed to create a renderer: %s", SDL_GetError()));
}
#endif

#if defined _linux
LSG_Strings LSG_Window::openFiles(bool openFolder, bool allowMultipleSelection, const LSG_Strings& filters)
{
	if (std::strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv("DISPLAY", ":0", 1);

	if (!gtk_init_check(0, nullptr))
		return {};

	auto dialog = gtk_file_chooser_dialog_new(
		(openFolder ? "Select a folder" : "Select a file"),
		nullptr,
		(openFolder ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN,
		GTK_RESPONSE_ACCEPT,
		nullptr
	);

	if (!filters.empty())
	{
		auto fileFilter = gtk_file_filter_new();

		for (const auto& filter : filters)
			gtk_file_filter_add_pattern(fileFilter, filter.c_str());

		gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), fileFilter);
	}

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), allowMultipleSelection);

	LSG_Strings filePaths;

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GSList* paths = nullptr;

		for (paths = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog)); paths != nullptr; paths = paths->next)
		{
			auto selectedPath = (gchar*)paths->data;

			if (!selectedPath)
				continue;

			auto filePath = std::string(selectedPath);

			if (filePath.substr(0, 7) == "file://")
				filePath = filePath.substr(7);

			if (!filePath.empty())
				filePaths.push_back(filePath);

			g_free(selectedPath);
		}

		if (paths)
			g_slist_free(paths);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	while (gtk_events_pending())
		gtk_main_iteration();

	return filePaths;
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
	auto jniIsPickingContent = jniEnvironment->GetStaticFieldID(jniActivity,  "IsPickingContent", "Z");
	auto jniContentPath      = jniEnvironment->GetStaticFieldID(jniActivity,  "ContentPath",      "Ljava/lang/String;");
	auto jniOpenFolder       = jniEnvironment->GetStaticMethodID(jniActivity, "OpenFolder",       "()V");

	jniEnvironment->CallStaticVoidMethod(jniActivity, jniOpenFolder);

	while (jniEnvironment->GetStaticBooleanField(jniActivity, jniIsPickingContent))
		SDL_Delay(10);

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
	auto jniIsPickingContent = jniEnvironment->GetStaticFieldID(jniActivity, "IsPickingContent", "Z");
	auto jniContentPath      = jniEnvironment->GetStaticFieldID(jniActivity, "ContentPath",      "Ljava/lang/String;");
	auto jniPickMethod       = (saveFile ? "SaveFile" : "OpenFile");
	auto jniPickFile         = jniEnvironment->GetStaticMethodID(jniActivity, jniPickMethod, "(Ljava/lang/String;)V");

	std::string filter = "";

	for (size_t i = 0; i < filters.size(); i++)
		filter.append(filters[i]).append(i < (filters.size() - 1) ? " " : "");

	auto jniFilter = jniEnvironment->NewStringUTF(filter.c_str());

	jniEnvironment->CallStaticVoidMethod(jniActivity, jniPickFile, jniFilter);

	while (jniEnvironment->GetStaticBooleanField(jniActivity, jniIsPickingContent))
		SDL_Delay(10);

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

    auto viewController = LSG_Window::sysWmInfo.info.uikit.window.rootViewController;

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

    auto viewController = LSG_Window::sysWmInfo.info.uikit.window.rootViewController;

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
    
    auto viewController = LSG_Window::sysWmInfo.info.uikit.window.rootViewController;

    [viewController presentViewController: picker animated: true completion: nil];
}

void LSG_Window::OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback)
{
    auto documentPicker = [[MyDocumentPicker alloc] init];

    documentPicker.resultsCallback = resultsCallback;

    auto picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes: [NSArray arrayWithObject: UTTypeFolder]];

    picker.delegate = documentPicker;

    auto viewController = LSG_Window::sysWmInfo.info.uikit.window.rootViewController;

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

SDL_Texture* LSG_Window::RotateTexture(SDL_Texture* texture, const LSG_ImageOrientation& orientation, const SDL_Size& size, uint32_t format)
{
	auto renderTarget = SDL_GetRenderTarget(LSG_Window::renderer);
	auto newTexture   = SDL_CreateTexture(LSG_Window::renderer, format, SDL_TEXTUREACCESS_TARGET, size.width, size.height);

	SDL_SetRenderTarget(LSG_Window::renderer, newTexture);

	SDL_RenderCopyEx(LSG_Window::renderer, texture, nullptr, nullptr, orientation.rotation, nullptr, orientation.flip);

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
std::string LSG_Window::SaveFile(const LSG_Strings& filters)
{
	if (std::strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv("DISPLAY", ":0", 1);

	if (!gtk_init_check(0, nullptr))
		return "";

	auto dialog = gtk_file_chooser_dialog_new(
		"Save File",
		nullptr,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE_AS,
		GTK_RESPONSE_ACCEPT,
		nullptr
	);

	if (!filters.empty())
	{
		auto fileFilter = gtk_file_filter_new();

		for (const auto& filter : filters)
			gtk_file_filter_add_pattern(fileFilter, filter.c_str());

		gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), fileFilter);
	}

	std::string filePath = "";

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		auto selectedPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		filePath = std::string(selectedPath);

		if (filePath.substr(0, 7) == "file://")
			filePath = filePath.substr(7);

		g_free(selectedPath);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	while (gtk_events_pending())
		gtk_main_iteration();

	return filePath;
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

SDL_Texture* LSG_Window::ToTexture(const std::string& imageFile)
{
	if (imageFile.empty())
		throw std::invalid_argument("imageFile cannot be empty.");

	auto filePath = LSG_Text::GetFullPath(imageFile);
	auto texture  = IMG_LoadTexture(LSG_Window::renderer, filePath.c_str());
	
	if (!texture)
		throw std::runtime_error(LSG_Text::Format("Failed to create texture from image '%s': %s", filePath.c_str(), SDL_GetError()));

	return texture;
}

SDL_Texture* LSG_Window::ToTexture(SDL_Surface* surface)
{
	if (!surface)
		throw std::invalid_argument("surface cannot be null.");

	auto texture = SDL_CreateTextureFromSurface(LSG_Window::renderer, surface);

	if (!texture)
		throw std::runtime_error(LSG_Text::Format("Failed to create texture from surface: %s", SDL_GetError()));

	return texture;
}
